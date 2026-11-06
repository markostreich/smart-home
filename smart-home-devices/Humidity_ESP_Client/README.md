# ESP32 humidity API client

Reads a DHT11 every 2.5 seconds and uploads the latest humidity, temperature and measurement time every 60 seconds. The existing access point, seven-day LittleFS history and CSV downloads remain available.

## Setup

- Select ESP32 Dev Module in Arduino IDE with Espressif's ESP32 board package.
- Install DHT sensor library, Adafruit Unified Sensor and ArduinoJson 7.
- DHT11 wiring: GND to GND, + to 3V3, signal to GPIO 4.
- Set DEVICE_NAME, UPLOAD_MS and AP_PASSWORD in Config.h.
- Add the same private headers as the other ESP clients in this sketch folder. These filenames are ignored by Git:
  - LocalLan.h: LOCAL_SSID and LOCAL_PW as constant character strings.
  - ServerInfo.h: SERVER_URL (for example https://your-api-host, without trailing slash or port) and HTTPS_PORT (for example "443").
  - RootCa.h: root_ca containing the PEM CA certificate trusted for your API server.
- Upload Humidity_ESP_Client.ino. Serial monitor: 115200 baud.
- Preserve the existing LittleFS partition layout to keep saved history.

## API

The device joins home Wi-Fi while keeping its own access point active. It synchronizes time using NTP and the timezone in Config.h (Germany with DST by default). TLS certificate verification is enabled.

Registration: POST /sensordevice/connect

```json
{"name":"ESP32-Klima"}
```

Both 201 (created) and 202 (already registered) are accepted.

Readings: POST /sensor/humidity/object

```json
{"deviceName":"ESP32-Klima","humidity":45.0,"temperature":21.5,"timestamp":"2026-11-06T14:30:00"}
```

The timestamp is the measurement time in the configured local timezone without an offset, matching the API LocalDateTime field. Only 201 counts as a saved reading. Invalid readings, readings older than ten seconds, and unsynchronized timestamps are skipped.

Failures are retried every five seconds. Wi-Fi reconnects automatically, and a 404 triggers registration again. Uploads send the latest reading; readings collected during an outage are not queued for API delivery. Local humidity history continues independently.

## CSV history

Connect to the ESP32-Klima access point using AP_PASSWORD and open http://192.168.4.1/. If NTP is unavailable, use the clock synchronization button. CSV downloads: http://192.168.4.1/history.csv.

LittleFS stores humidity and temperature together per 15-minute slot, up to 672 slots (seven days), and preserves history across restarts. CSV columns are Zeit_Deutschland, Luftfeuchtigkeit_Prozent and Temperatur_Celsius, separated by semicolons. Existing version 2 humidity history is preserved; its temperature cells stay empty because no temperature was recorded. New readings use version 3 storage. CSV uses German local timestamps. Save CSV before changing partitions or erasing flash. The existing history code attempts formatting if LittleFS cannot mount.
