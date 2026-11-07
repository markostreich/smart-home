#pragma once
#include <Arduino.h>
namespace Config {
constexpr char DEVICE_NAME[] = "Humidity_ESP_Client";
constexpr uint32_t UPLOAD_MS = 60000;
constexpr uint32_t WIFI_RETRY_MS = 10000;
constexpr char AP_SSID[] = "ESP32-Klima";
constexpr char AP_PASSWORD[] = "Klima2026"; // at least 8 characters; please change
constexpr uint8_t DHT_PIN = 25;
constexpr int HISTORY_SIZE = 672;
constexpr uint32_t HISTORY_SECONDS = 900;
constexpr uint32_t SENSOR_MS = 2500;
constexpr char TIMEZONE[] = "CET-1CEST,M3.5.0,M10.5.0/3";
}
