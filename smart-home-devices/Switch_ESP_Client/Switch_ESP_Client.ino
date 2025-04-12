#include <string>
#include <memory>
#include "JsonAdapter.h"
#include "RestClient.h"

#define RECONNECT_INTERVAL 1000  //one second

constexpr const char* DEVICE_NAME = "Pumpe_1";

SwitchObject oregano = { "Oregano", false, 0, DEVICE_NAME };
SwitchObject schnittlauch = { "Schnittlauch", false, 0, DEVICE_NAME };

unsigned long stopTimeOregano = 0;
unsigned long stopTimeSchnittlauch = 0;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  connectClient(DEVICE_NAME);
  postSwitchObject(serializeSwitchObject(oregano));
  postSwitchObject(serializeSwitchObject(schnittlauch));

  pinMode(21, OUTPUT);
  pinMode(19, OUTPUT);
}

void loop() {
  const unsigned long currentTime = millis();
  if (currentTime - lastTime >= RECONNECT_INTERVAL) {
    connectClient(DEVICE_NAME);
    const std::vector<SwitchObject> switchObjects = parseSwitchObjectArrayJson(getUpdate(DEVICE_NAME).c_str());
    for (const auto& switchObject : switchObjects)
      handleSwitchObject(switchObject, currentTime);
  }
  delay(100);
}

void handleSwitchObject(const SwitchObject switchObject, const unsigned long currentTime) {
  if (switchObject.name == oregano.name) {
    stopTimeOregano = handleDuration(switchObject, currentTime, stopTimeOregano);
    doSwitchObject(switchObject, 21);
  } else if (switchObject.name == schnittlauch.name) {
    stopTimeSchnittlauch = handleDuration(switchObject, currentTime, stopTimeSchnittlauch);
    doSwitchObject(switchObject, 19);
  }
}

void doSwitchObject(const SwitchObject switchObject, const unsigned int pin) {
  if (switchObject.state)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

unsigned long handleDuration(SwitchObject switchObject, const unsigned long currentTime, unsigned long stopTime) {
  if (switchObject.state && switchObject.duration != 0) {
    if (stopTime == 0) {
      stopTime = currentTime + switchObject.duration * 1000;
    }
    if (stopTime != 0 && stopTime <= currentTime) {
      stopTime = 0;
      switchObject.state = false;
      postSwitchObject(serializeSwitchObject(switchObject));
    }
  }
  return stopTime;
}
