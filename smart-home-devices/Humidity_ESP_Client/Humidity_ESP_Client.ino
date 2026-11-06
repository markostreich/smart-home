#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "Config.h"
#include "SharedData.h"
#include "Sensor.h"
#include "History.h"
#include "CsvServer.h"
#include "RestClient.h"

void setup() {
  Serial.begin(115200);
  delay(300);
  if (!initSharedData()) {
    Serial.println("Mutex Fehler");
    while (true) delay(1000);
  }
  initSensor();
  if (!initHistory()) {
    Serial.println("LittleFS Fehler");
    while (true) delay(1000);
  }
  setenv("TZ", Config::TIMEZONE, 1);
  tzset();

  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);
  if (!WiFi.softAP(Config::AP_SSID, Config::AP_PASSWORD)) {
    Serial.println("FEHLER: Access Point konnte nicht gestartet werden!");
    while (true) delay(1000);
  }
  Serial.print("Eigenes WLAN: ");
  Serial.println(Config::AP_SSID);
  Serial.print("CSV-Webseite: http://");
  Serial.println(WiFi.softAPIP());
  initCsvServer();
  initApiClient();

  if (xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, nullptr, 2, nullptr, 1) != pdPASS || xTaskCreatePinnedToCore(HistoryTask, "HistoryTask", 4096, nullptr, 1, nullptr, 0) != pdPASS || xTaskCreatePinnedToCore(CsvServerTask, "CsvServerTask", 6144, nullptr, 1, nullptr, 0) != pdPASS) {
    Serial.println("Task-Erstellung fehlgeschlagen");
    while (true) delay(1000);
  }
  if (xTaskCreatePinnedToCore(ApiTask, "ApiTask", 8192, nullptr, 1, nullptr, 0) != pdPASS) {
    Serial.println("API-Task konnte nicht gestartet werden");
    while (true) delay(1000);
  }
  Serial.println("Datenlogger und API-Client gestartet; Uhrzeit per NTP oder Webseite.");
}
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
