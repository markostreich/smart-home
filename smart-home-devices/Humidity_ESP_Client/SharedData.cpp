#include "SharedData.h"
SemaphoreHandle_t dataMutex = nullptr;
Sample currentSample = {NAN, NAN, false, 0};
HistoryData historyData = {};
bool lastUploadOK = false;
bool initSharedData() {
  dataMutex = xSemaphoreCreateMutex();
  return dataMutex != nullptr;
}
bool readSnapshot(Sample& sample, HistoryData& history, bool& cloudOK) {
  if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) != pdTRUE) return false;
  sample = currentSample;
  history = historyData;
  cloudOK = lastUploadOK;
  xSemaphoreGive(dataMutex);
  return true;
}
