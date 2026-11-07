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
    Serial.println("Mutex error");
    while (true) delay(1000);
  }
  initSensor();
  if (!initHistory()) {
    Serial.println("LittleFS error");
    while (true) delay(1000);
  }
  setenv("TZ", Config::TIMEZONE, 1);
  tzset();

  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);
  if (!WiFi.softAP(Config::AP_SSID, Config::AP_PASSWORD)) {
    Serial.println("ERROR: Could not start access point!");
    while (true) delay(1000);
  }
  Serial.print("Access point: ");
  Serial.println(Config::AP_SSID);
  Serial.print("CSV page: http://");
  Serial.println(WiFi.softAPIP());
  initCsvServer();
  initApiClient();

  if (xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, nullptr, 2, nullptr, 1) != pdPASS || xTaskCreatePinnedToCore(HistoryTask, "HistoryTask", 4096, nullptr, 1, nullptr, 0) != pdPASS || xTaskCreatePinnedToCore(CsvServerTask, "CsvServerTask", 6144, nullptr, 1, nullptr, 0) != pdPASS) {
    Serial.println("Task creation failed");
    while (true) delay(1000);
  }
  if (xTaskCreatePinnedToCore(ApiTask, "ApiTask", 8192, nullptr, 1, nullptr, 0) != pdPASS) {
    Serial.println("Could not start API task");
    while (true) delay(1000);
  }
  Serial.println("Data logger and API client started; set the clock through NTP or the web page.");
}
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
