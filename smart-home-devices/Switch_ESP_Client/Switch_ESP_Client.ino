#include <string>
#include <memory>
#include "JsonAdapter.h"
#include "RestClient.h"

#define RECONNECT_INTERVAL 1000  //one second

constexpr const char* DEVICE_NAME = "Pumpe_1";

SwitchObject oregano = { "Oregano", false, 0, DEVICE_NAME };
SwitchObject schnittlauch = { "Schnittlauch", false, 0, DEVICE_NAME };

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
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= RECONNECT_INTERVAL) {
    connectClient(DEVICE_NAME);
    std::vector<SwitchObject> switchObjects = parseSwitchObjectArrayJson(getUpdate(DEVICE_NAME).c_str());
    for (const auto& switchObject : switchObjects)
      handleSwitchObject(switchObject);
  }
  delay(100);
}

void handleSwitchObject(const SwitchObject switchObject) {
  if (switchObject.name == oregano.name)
    handleSwitchObject(switchObject, 21);
  else if (switchObject.name == schnittlauch.name)
    handleSwitchObject(switchObject, 19);
}

void handleSwitchObject(const SwitchObject switchObject, const unsigned int pin) {
  if (switchObject.state)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}
