#include <string>
#include <memory>
#include "JsonAdapter.h"
#include "RestClient.h"

constexpr const char* DEVICE_NAME = "Pumpe_1";

const SwitchObject oregano = { "Oregano", false, 0, DEVICE_NAME };

int i = 0;

void setup() {
  Serial.begin(115200);
    connectToWiFi();
    connectClient(DEVICE_NAME);
    postSwitchObject(serializeSwitchObject(oregano));
}

void loop() {
  delay(1000);
  getUpdate(DEVICE_NAME);
}
