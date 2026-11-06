#include "RestClient.h"
#include "Config.h"
#include "SharedData.h"
#include "LocalLan.h"
#include "ServerInfo.h"
#include "RootCa.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

static int postJson(const char* path, const String& payload) {
  WiFiClientSecure client;
  client.setCACert(root_ca);
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  const String url = String(SERVER_URL) + ":" + HTTPS_PORT + path;
  if (!http.begin(client, url)) return -1;
  http.addHeader("Content-Type", "application/json");
  const int code = http.POST(payload);
  Serial.printf("%s HTTP %d\n", path, code);
  http.end();
  return code;
}

static bool registerDevice() {
  JsonDocument document;
  document["name"] = Config::DEVICE_NAME;
  String payload;
  serializeJson(document, payload);
  const int code = postJson("/sensordevice/connect", payload);
  return code == HTTP_CODE_CREATED || code == HTTP_CODE_ACCEPTED;
}

void initApiClient() {
  WiFi.setAutoReconnect(true);
  WiFi.begin(LOCAL_SSID, LOCAL_PW);
  configTzTime(Config::TIMEZONE, "pool.ntp.org", "time.nist.gov");
}

void ApiTask(void*) {
  bool registered = false;
  bool uploaded = false;
  uint32_t lastUpload = 0;
  uint32_t lastWifiRetry = millis();
  for (;;) {
    bool success = false;
    if (WiFi.status() != WL_CONNECTED) {
      registered = false;
      if (millis() - lastWifiRetry >= Config::WIFI_RETRY_MS) {
        WiFi.reconnect();
        lastWifiRetry = millis();
      }
    } else if (time(nullptr) >= 1700000000) {
      // TLS verification and measurement timestamps require a synchronized clock.
      if (!registered) registered = registerDevice();
      if (registered && (!uploaded || millis() - lastUpload >= Config::UPLOAD_MS)) {
        Sample sample = {};
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          sample = currentSample;
          xSemaphoreGive(dataMutex);
        }
        const time_t now = time(nullptr);
        if (sample.valid && sample.timestamp >= 1700000000 &&
            now >= sample.timestamp && now - sample.timestamp <= 10) {
          struct tm local;
          localtime_r(&sample.timestamp, &local);
          char timestamp[20];
          strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &local);
          JsonDocument document;
          document["deviceName"] = Config::DEVICE_NAME;
          document["humidity"] = sample.humidity;
          document["temperature"] = sample.temperature;
          document["timestamp"] = timestamp;
          String payload;
          serializeJson(document, payload);
          const int code = postJson("/sensor/humidity/object", payload);
          success = code == HTTP_CODE_CREATED;
          if (code == HTTP_CODE_NOT_FOUND) registered = false;
          if (success) {
            uploaded = true;
            lastUpload = millis();
          }
        }
      } else {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          success = lastUploadOK;
          xSemaphoreGive(dataMutex);
        }
      }
    }
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      lastUploadOK = success;
      xSemaphoreGive(dataMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
