#include "History.h"
#include "Config.h"
#include "SharedData.h"
#include <LittleFS.h>
#include <time.h>
#include <math.h>
#include <string.h>

static constexpr uint32_t MAGIC = 0xD4712401;
static constexpr uint32_t VERSION = 3; // humidity and temperature
struct LegacyHistoryData {
  uint32_t magic;
  uint32_t version;
  int64_t slots[Config::HISTORY_SIZE];
  float values[Config::HISTORY_SIZE];
};
static const char* FILE_PATH = "/history.bin";
static const char* TMP_PATH = "/history.tmp";

static void resetHistory() {
  historyData.magic = MAGIC;
  historyData.version = VERSION;
  for (int i = 0; i < Config::HISTORY_SIZE; ++i) {
    historyData.slots[i] = -1;
    historyData.values[i] = NAN;
    historyData.temperatures[i] = NAN;
  }
}

bool initHistory() {
  if (!LittleFS.begin(false)) {
    Serial.println("Could not mount LittleFS. Attempting to format.");
    if (!LittleFS.begin(true)) return false;
  }
  resetHistory();
  File f = LittleFS.open(FILE_PATH, "r");
  if (!f) return true;
  if (f.size() == sizeof(LegacyHistoryData)) {
    static LegacyHistoryData legacy;
    const size_t n = f.read(reinterpret_cast<uint8_t*>(&legacy), sizeof(legacy));
    f.close();
    if (n == sizeof(legacy) && legacy.magic == MAGIC && legacy.version == 2) {
      memcpy(historyData.slots, legacy.slots, sizeof(legacy.slots));
      memcpy(historyData.values, legacy.values, sizeof(legacy.values));
      Serial.println("Loaded history v2; previous temperature values remain empty.");
    } else {
      Serial.println("Invalid history file; starting a new history.");
    }
    return true;
  }
  if (f.size() != sizeof(historyData)) {
    Serial.println("Old or invalid history file size; starting a new history.");
    f.close();
    return true;
  }
  // Keep the large restore buffer off the task stack.
  static HistoryData restored;
  size_t n = f.read(reinterpret_cast<uint8_t*>(&restored), sizeof(restored));
  f.close();
  if (n != sizeof(restored) || restored.magic != MAGIC || restored.version != VERSION) {
    Serial.println("Invalid history file; starting a new history.");
    return true;
  }
  historyData = restored;
  Serial.println("Loaded seven-day history from LittleFS.");
  return true;
}

int64_t getCurrentSlot() {
  time_t now;
  time(&now);
  if (now < 1700000000) return -1;
  return static_cast<int64_t>(now) / Config::HISTORY_SECONDS;
}

// Only HistoryTask writes to LittleFS. On power failure, the old file
// normally survives because a temporary file is written and then renamed.
static bool persistHistory(const HistoryData& snapshot) {
  File f = LittleFS.open(TMP_PATH, "w");
  if (!f) return false;
  const size_t n = f.write(reinterpret_cast<const uint8_t*>(&snapshot), sizeof(snapshot));
  f.flush();
  f.close();
  if (n != sizeof(snapshot)) {
    LittleFS.remove(TMP_PATH);
    return false;
  }
  // LittleFS rename normally replaces an existing file atomically on ESP32.
  if (!LittleFS.rename(TMP_PATH, FILE_PATH)) {
    LittleFS.remove(TMP_PATH);
    return false;
  }
  return true;
}

void HistoryTask(void*) {
  // Only this task uses the snapshot; keep the large array off the stack.
  static HistoryData snapshot;
  for (;;) {
    const int64_t slot = getCurrentSlot();
    if (slot >= 0) {
      bool changed = false;
      if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        const int index = static_cast<int>(slot % Config::HISTORY_SIZE);
        if (historyData.slots[index] != slot && currentSample.valid) {
          historyData.slots[index] = slot;
          historyData.values[index] = currentSample.humidity;
          historyData.temperatures[index] = currentSample.temperature;
          snapshot = historyData;
          changed = true;
        }
        xSemaphoreGive(dataMutex);
      }
      if (changed) {
        if (persistHistory(snapshot)) {
          Serial.printf("Flash: saved slot %lld\n", (long long)slot);
        } else {
          Serial.println("ERROR: Could not write to flash!");
          // Retry in one second; reset the RAM slot if it is still the same.
          if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
            const int index = static_cast<int>(slot % Config::HISTORY_SIZE);
            if (historyData.slots[index] == slot) {
              historyData.slots[index] = -1;
              historyData.values[index] = NAN;
              historyData.temperatures[index] = NAN;
            }
            xSemaphoreGive(dataMutex);
          }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
