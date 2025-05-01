#include <string>
#include <memory>
#include "JsonAdapter.h"
#include "RestClient.h"
#include "LEDs.h"

#define RECONNECT_INTERVAL 1000  //one second


constexpr const char* DEVICE_NAME = "Balkon";

LedStripeObject gelaender = {
  { "Geländer", "OFF", 0, 255, 0, 100, DEVICE_NAME },
  Adafruit_NeoPixel(300, 21, NEO_GRB + NEO_KHZ800),
  xSemaphoreCreateMutex(),
  true
};

std::vector<LedStripeObject> globalLedStripeObjects;

unsigned long lastTime = 0;

void setup() {
  globalLedStripeObjects.push_back(gelaender);
  Serial.begin(115200);
  connectToWiFi();
  connectClient(DEVICE_NAME);
  /* POST LED stripe objects */
  for (const auto& globalLedStripeObject : globalLedStripeObjects)
    postLedStripeObject(serializeSLedStripeObject(globalLedStripeObject.object));

  xTaskCreatePinnedToCore(RestClientTask, "RestClientTask", 1000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(GelaenderTask, "GelaenderTask", 1000, NULL, 1, NULL, 1);
}

void loop() {
}

void GelaenderTask(void* pvParameters) {
  runLedStripe(gelaender);
}

void runLedStripe(LedStripeObject ledStripe) {
  std::string mode;
  unsigned int red;
  unsigned int green;
  unsigned int blue;
  unsigned int brightness;

  // call back to stop never ending led routines
  const StopCallBack checkChanged = [&ledStripe]() -> bool {
    bool changed = false;
    if (xSemaphoreTake(ledStripe.mutex, portMAX_DELAY)) {
      changed = ledStripe.changed;
      xSemaphoreGive(ledStripe.mutex);
    }
    return changed;
  };

  while (1) {
    if (xSemaphoreTake(ledStripe.mutex, portMAX_DELAY)) {
      if (ledStripe.changed) {
        mode = ledStripe.object.mode;
        red = ledStripe.object.red;
        green = ledStripe.object.green;
        blue = ledStripe.object.blue;
        brightness = ledStripe.object.brightness;
        ledStripe.changed = false;
      }
      xSemaphoreGive(ledStripe.mutex);
    }
    if (mode == "OFF") {
      colorAll(ledStripe.strip, 0, 0, 0, 0);
    } else if (mode == "COLOR") {
      colorAll(ledStripe.strip, red, green, blue, brightness);
    } else if (mode == "THEATER") {
      theaterChase(ledStripe.strip, red, green, blue, brightness, checkChanged);
    } else if (mode == "RAINBOW") {
      rainbow(ledStripe.strip, brightness, checkChanged);
    }
  }
}

void RestClientTask(void* pvParameters) {
  while (1) {
    const unsigned long currentTime = millis();
    if (currentTime - lastTime >= RECONNECT_INTERVAL) {
      connectClient(DEVICE_NAME);
      const std::vector<LedStripeObjectTO> ledStripeObjects = parseLedStripeObjectArrayJson(getUpdate(DEVICE_NAME).c_str());
      for (const auto& ledStripeObject : ledStripeObjects)
        for (const auto& globalLedStripeObject : globalLedStripeObjects)
          if (ledStripeObject.name == globalLedStripeObject.object.name)
            updateLedStripeObject(ledStripeObject, globalLedStripeObject);
    }
  }
}

void updateLedStripeObject(const LedStripeObjectTO srcObject, LedStripeObject targetObject) {
  if (xSemaphoreTake(targetObject.mutex, portMAX_DELAY)) {
    if (targetObject.object.mode != srcObject.mode) {
      targetObject.object.mode = srcObject.mode;
      targetObject.changed = true;
    }
    if (targetObject.object.red != srcObject.red) {
      targetObject.object.red = srcObject.red;
      targetObject.changed = true;
    }
    if (targetObject.object.green != srcObject.green) {
      targetObject.object.green = srcObject.green;
      targetObject.changed = true;
    }
    if (targetObject.object.blue != srcObject.blue) {
      targetObject.object.blue = srcObject.blue;
      targetObject.changed = true;
    }
    if (targetObject.object.brightness != srcObject.brightness) {
      targetObject.object.brightness = srcObject.brightness;
      targetObject.changed = true;
    }
    xSemaphoreGive(targetObject.mutex);
  }
}
