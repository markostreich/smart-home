#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <math.h>
#include <time.h>
#include "Config.h"
struct Sample { float humidity; float temperature; bool valid; time_t timestamp; };
struct HistoryData {
  uint32_t magic;
  uint32_t version;
  int64_t slots[Config::HISTORY_SIZE];
  float values[Config::HISTORY_SIZE];
  float temperatures[Config::HISTORY_SIZE];
};
extern SemaphoreHandle_t dataMutex;
extern Sample currentSample;
extern HistoryData historyData;
extern bool lastUploadOK;
bool initSharedData();
bool readSnapshot(Sample& sample, HistoryData& history, bool& cloudOK);
