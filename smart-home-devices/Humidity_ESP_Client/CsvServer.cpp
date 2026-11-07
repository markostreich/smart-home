#include "CsvServer.h"
#include "Config.h"
#include "SharedData.h"
#include "History.h"
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ArduinoJson.h>

static WebServer server(80);
static HistoryData csvSnapshot;

static void handleIndex() {
  String html = R"HTML(<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Climate</title><style>body{font-family:system-ui;max-width:520px;margin:2em auto;padding:0 1em;line-height:1.5}button,a.btn{display:inline-block;padding:.75em 1em;background:#1464a0;color:white;border:0;border-radius:6px;text-decoration:none;cursor:pointer}#status{margin:1em 0}</style></head>
<body><h1>ESP32 Climate</h1><p>7 days of humidity and temperature, one reading every 15 minutes (up to 672).</p>
<section aria-live="polite"><h2>Current readings</h2>
<p>Humidity: <strong id="humidity">--</strong> %</p>
<p>Temperature: <strong id="temperature">--</strong> &deg;C</p>
<p id="readingStatus">Loading readings ...</p></section>
<p>The clock synchronizes through NTP on your home Wi-Fi. Without a network connection, set the clock from this device. Readings cannot be saved or uploaded until the clock is set.</p>
<button onclick="syncClock()">Set clock now</button><p id="status">Checking clock ...</p>
<p><a class="btn" href="/history.csv">Download CSV</a></p>
<p>Direct URL: <code>/history.csv</code></p>
<script>
async function syncClock(){
 const status=document.getElementById('status');
 status.textContent='Synchronizing ...';
 try {
  const r=await fetch('/settime?epoch='+Math.floor(Date.now()/1000),{method:'POST',cache:'no-store'});
  status.textContent=await r.text();
 }catch(e){status.textContent='Error: '+e.message;}
}
async function checkClock(){
 try{const r=await fetch('/status',{cache:'no-store'});document.getElementById('status').textContent=await r.text();}catch(e){}
}
checkClock();
async function refreshReadings(){
 const humidity=document.getElementById('humidity');
 const temperature=document.getElementById('temperature');
 const status=document.getElementById('readingStatus');
 try {
  const r=await fetch('/readings',{cache:'no-store'});
  if(!r.ok) throw new Error('HTTP '+r.status);
  const reading=await r.json();
  if(!reading.valid) throw new Error('No valid sensor readings');
  humidity.textContent=reading.humidity.toLocaleString('en-GB',{minimumFractionDigits:1,maximumFractionDigits:1});
  temperature.textContent=reading.temperature.toLocaleString('en-GB',{minimumFractionDigits:1,maximumFractionDigits:1});
  status.textContent='Readings refresh automatically.';
 }catch(e){
  humidity.textContent='--';
  temperature.textContent='--';
  status.textContent='Readings unavailable: '+e.message;
 }finally{setTimeout(refreshReadings,2500);}
}
refreshReadings();
</script></body></html>)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

static void handleReadings() {
  Sample sample;
  if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) != pdTRUE) {
    server.send(503, "text/plain", "Sensor readings temporarily unavailable");
    return;
  }
  sample = currentSample;
  xSemaphoreGive(dataMutex);
  JsonDocument document;
  const bool valid = sample.valid && isfinite(sample.humidity) && isfinite(sample.temperature);
  document["valid"] = valid;
  if (valid) {
    document["humidity"] = sample.humidity;
    document["temperature"] = sample.temperature;
  }
  String response;
  serializeJson(document, response);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", response);
}

static void handleStatus() {
  if (getCurrentSlot() < 0) {
    server.send(200, "text/plain; charset=utf-8", "Clock not synchronized yet. Please press the button.");
    return;
  }
  time_t now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);
  char stamp[48];
  strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S %Z", &local);
  server.send(200, "text/plain; charset=utf-8", String("ESP32 time: ") + stamp);
}

static void handleSetTime() {
  if (!server.hasArg("epoch")) {
    server.send(400, "text/plain", "Missing epoch"); return;
  }
  String arg=server.arg("epoch");
  if (arg.length() < 10 || arg.length() > 11) {
    server.send(400, "text/plain", "Invalid timestamp"); return;
  }
  for (unsigned int i=0; i<arg.length(); ++i) {
    if (arg[i]<'0' || arg[i]>'9') {server.send(400,"text/plain","Invalid timestamp");return;}
  }
  int64_t epoch = atoll(arg.c_str());
  if (epoch < 1700000000LL || epoch > 4102444800LL) {
    server.send(400, "text/plain", "Timestamp out of range"); return;
  }
  struct timeval tv;
  tv.tv_sec = static_cast<time_t>(epoch);
  tv.tv_usec = 0;
  if (settimeofday(&tv, nullptr) != 0) {
    server.send(500, "text/plain", "Could not set the clock"); return;
  }
  Serial.printf("Clock set from browser: %lld\n", (long long)epoch);
  handleStatus();
}

static void handleCsv() {
  if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    server.send(503, "text/plain", "Data temporarily unavailable"); return;
  }
  csvSnapshot = historyData;
  xSemaphoreGive(dataMutex);

  // CSV is available without a synchronized clock, including after a restart.
  // Export all valid slots chronologically from the ring buffer.
  int64_t latest = -1;
  for (int i=0;i<Config::HISTORY_SIZE;++i) {
    if (csvSnapshot.slots[i] > latest && isfinite(csvSnapshot.values[i])) latest=csvSnapshot.slots[i];
  }
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Disposition", "attachment; filename=esp32_climate_7days.csv");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/csv; charset=utf-8", "");
  server.sendContent("Time_Germany;Humidity_Percent;Temperature_Celsius\r\n");
  if (latest >= 0) {
    for (int i=0;i<Config::HISTORY_SIZE;++i) {
      int64_t slot=latest-Config::HISTORY_SIZE+1+i;
      if (slot < 0) continue;
      int idx=static_cast<int>(slot % Config::HISTORY_SIZE);
      if (csvSnapshot.slots[idx]!=slot || !isfinite(csvSnapshot.values[idx])) continue;
      time_t ts=static_cast<time_t>(slot*Config::HISTORY_SECONDS);
      struct tm local;
      localtime_r(&ts,&local);
      char stamp[40];
      strftime(stamp,sizeof(stamp),"%Y-%m-%d %H:%M:%S %z",&local);
      const String temperature = isfinite(csvSnapshot.temperatures[idx])
          ? String(csvSnapshot.temperatures[idx], 1) : String("");
      server.sendContent(String(stamp)+";"+String(csvSnapshot.values[idx],1)+";"+temperature+"\r\n");
    }
  }
  server.sendContent("");
}

void initCsvServer() {
  server.on("/", HTTP_GET, handleIndex);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/readings", HTTP_GET, handleReadings);
  server.on("/settime", HTTP_POST, handleSetTime);
  server.on("/history.csv", HTTP_GET, handleCsv);
  server.begin();
  Serial.println("CSV web server started");
}
void CsvServerTask(void*) {
  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
