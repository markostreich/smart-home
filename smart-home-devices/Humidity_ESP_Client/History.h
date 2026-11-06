#pragma once
#include <stdint.h>
bool initHistory();
int64_t getCurrentSlot();
void HistoryTask(void* parameter);
