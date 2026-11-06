#include "CsvServer.h"
#include "Config.h"
#include "SharedData.h"
#include "History.h"
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

static WebServer server(80);
static HistoryData csvSnapshot;

static void handleIndex() {
  String html = R"HTML(<!doctype html><html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Klima</title><style>body{font-family:system-ui;max-width:520px;margin:2em auto;padding:0 1em;line-height:1.5}button,a.btn{display:inline-block;padding:.75em 1em;background:#1464a0;color:white;border:0;border-radius:6px;text-decoration:none;cursor:pointer}#status{margin:1em 0}</style></head>
<body><h1>ESP32 Klima</h1><p>7 Tage Luftfeuchte und Temperatur, ein Wert je 15 Minuten (maximal 672).</p>
<p>Die Uhrzeit wird im Heim-WLAN per NTP synchronisiert. Ohne Netzwerk bitte die Uhrzeit von diesem Geraet uebertragen. Ohne Uhrzeit keine neuen gespeicherten Messwerte oder API-Uploads.</p>
<button onclick="syncClock()">Uhrzeit jetzt uebertragen</button><p id="status">Pruefe Uhrzeit ...</p>
<p><a class="btn" href="/history.csv">CSV herunterladen</a></p>
<p>Direktadresse: <code>/history.csv</code></p>
<script>
async function syncClock(){
 const status=document.getElementById('status');
 status.textContent='Synchronisiere ...';
 try {
  const r=await fetch('/settime?epoch='+Math.floor(Date.now()/1000),{method:'POST',cache:'no-store'});
  status.textContent=await r.text();
 }catch(e){status.textContent='Fehler: '+e.message;}
}
async function checkClock(){
 try{const r=await fetch('/status',{cache:'no-store'});document.getElementById('status').textContent=await r.text();}catch(e){}
}
checkClock();
</script></body></html>)HTML";
  server.send(200, "text/html; charset=utf-8", html);
}

static void handleStatus() {
  if (getCurrentSlot() < 0) {
    server.send(200, "text/plain; charset=utf-8", "Uhrzeit noch nicht synchronisiert. Bitte Taste druecken.");
    return;
  }
  time_t now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);
  char stamp[48];
  strftime(stamp, sizeof(stamp), "%d.%m.%Y %H:%M:%S %Z", &local);
  server.send(200, "text/plain; charset=utf-8", String("ESP32-Uhrzeit: ") + stamp);
}

static void handleSetTime() {
  if (!server.hasArg("epoch")) {
    server.send(400, "text/plain", "epoch fehlt"); return;
  }
  String arg=server.arg("epoch");
  if (arg.length() < 10 || arg.length() > 11) {
    server.send(400, "text/plain", "Ungueltiger Zeitwert"); return;
  }
  for (unsigned int i=0; i<arg.length(); ++i) {
    if (arg[i]<'0' || arg[i]>'9') {server.send(400,"text/plain","Ungueltiger Zeitwert");return;}
  }
  int64_t epoch = atoll(arg.c_str());
  if (epoch < 1700000000LL || epoch > 4102444800LL) {
    server.send(400, "text/plain", "Zeit ausserhalb des Bereichs"); return;
  }
  struct timeval tv;
  tv.tv_sec = static_cast<time_t>(epoch);
  tv.tv_usec = 0;
  if (settimeofday(&tv, nullptr) != 0) {
    server.send(500, "text/plain", "Uhrzeit konnte nicht gesetzt werden"); return;
  }
  Serial.printf("Uhrzeit vom Browser gesetzt: %lld\n", (long long)epoch);
  handleStatus();
}

static void handleCsv() {
  if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    server.send(503, "text/plain", "Daten gerade nicht verfuegbar"); return;
  }
  csvSnapshot = historyData;
  xSemaphoreGive(dataMutex);

  // CSV auch ohne aktuelle Uhrzeit verfuegbar, z.B. direkt nach Neustart.
  // Alle gueltigen Slots chronologisch sortieren (Ringpuffer).
  int64_t latest = -1;
  for (int i=0;i<Config::HISTORY_SIZE;++i) {
    if (csvSnapshot.slots[i] > latest && isfinite(csvSnapshot.values[i])) latest=csvSnapshot.slots[i];
  }
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Disposition", "attachment; filename=esp32_klima_7tage.csv");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/csv; charset=utf-8", "");
  server.sendContent("Zeit_Deutschland;Luftfeuchtigkeit_Prozent;Temperatur_Celsius\r\n");
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
  server.on("/settime", HTTP_POST, handleSetTime);
  server.on("/history.csv", HTTP_GET, handleCsv);
  server.begin();
  Serial.println("CSV-Webserver gestartet");
}
void CsvServerTask(void*) {
  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
