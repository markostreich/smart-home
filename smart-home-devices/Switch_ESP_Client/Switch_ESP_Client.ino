#include <string>
#include <memory>
#include "JsonAdapter.h"
#include "RestClient.h"

#define RECONNECT_INTERVAL 1000  //one second

constexpr const char* DEVICE_NAME = "Balkon";

SwitchObject Pump_1 = { { "Pump_1", false, 0, DEVICE_NAME }, 0, 16 };
SwitchObject Pump_2 = { { "Pump_2", false, 0, DEVICE_NAME }, 0, 18 };
SwitchObject Pump_3 = { { "Pump_3", false, 0, DEVICE_NAME }, 0, 19 };
SwitchObject Big_Pump_1 = { { "Big_Pump_1", false, 0, DEVICE_NAME }, 0, 17 };
SwitchObject Big_Pump_2 = { { "Big_Pump_2", false, 0, DEVICE_NAME }, 0, 21 };

unsigned long stopTimeOregano = 0;
unsigned long stopTimeSchnittlauch = 0;

std::vector<SwitchObject> globalSwitchObjects;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  globalSwitchObjects.push_back(Pump_1);
  globalSwitchObjects.push_back(Pump_2);
  globalSwitchObjects.push_back(Pump_3);
  globalSwitchObjects.push_back(Big_Pump_1);
  globalSwitchObjects.push_back(Big_Pump_2);
  connectToWiFi();
  connectClient(DEVICE_NAME);
  for (const auto& switchObject : globalSwitchObjects) {
    postSwitchObject(serializeSwitchObject(switchObject.object));
    pinMode(switchObject.pin, OUTPUT);
  }
}

void loop() {
  const unsigned long currentTime = millis();
  for (auto& globalSwitchObject : globalSwitchObjects)
    handleDuration(globalSwitchObject, currentTime);
  if (currentTime - lastTime >= RECONNECT_INTERVAL) {
    connectClient(DEVICE_NAME);
    const std::vector<SwitchObjectTO> switchObjectTOs = parseSwitchObjectArrayJson(getUpdate(DEVICE_NAME).c_str());
    for (const auto& switchObjectTO : switchObjectTOs)
      for (auto& globalSwitchObject : globalSwitchObjects)
        updateSwitch(switchObjectTO, globalSwitchObject);
    lastTime = currentTime;
  }
  delay(100);
}

void updateSwitch(const SwitchObjectTO& sourceObject, SwitchObject& targetObject) {
  if (sourceObject.name == targetObject.object.name) {
    if (sourceObject.state)
      digitalWrite(targetObject.pin, HIGH);
    else
      digitalWrite(targetObject.pin, LOW);
    targetObject.object.state = sourceObject.state;
    targetObject.object.duration = sourceObject.duration;
  }
}

void handleDuration(SwitchObject& switchObject, const unsigned long currentTime) {
  if (switchObject.object.state && switchObject.object.duration) {
    if (!switchObject.stopTime) {
      switchObject.stopTime = currentTime + switchObject.object.duration * 1000;
    }
    if (switchObject.stopTime && switchObject.stopTime <= currentTime) {
      switchObject.stopTime = 0;
      switchObject.object.state = false;
      digitalWrite(switchObject.pin, LOW);
      postSwitchObject(serializeSwitchObject(switchObject.object));
    }
  } else if (switchObject.stopTime)
    switchObject.stopTime = 0;
}
