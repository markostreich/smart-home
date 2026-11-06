#include "Sensor.h"
#include "Config.h"
#include "SharedData.h"
#include <DHT.h>
static DHT dht(Config::DHT_PIN, DHT11);
void initSensor() { dht.begin(); }
void SensorTask(void*) {
  TickType_t wake = xTaskGetTickCount();
  for (;;) {
    const float h = dht.readHumidity();
    const float t = dht.readTemperature();
    Sample sample = {h, t, isfinite(h) && isfinite(t), time(nullptr)};
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      currentSample = sample;
      xSemaphoreGive(dataMutex);
    }
    if (sample.valid) Serial.printf("DHT11: %.1f %% / %.1f C\n", h, t);
    else Serial.println("DHT11: Lesefehler");
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(Config::SENSOR_MS));
  }
}
