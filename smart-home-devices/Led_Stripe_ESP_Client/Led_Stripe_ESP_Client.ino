#include <string>
#include <memory>
#include <vector>
#include "JsonAdapter.h"
#include "RestClient.h"
#include "LEDs.h"

#define RECONNECT_INTERVAL 1000  // One second

constexpr const char* DEVICE_NAME = "Balkon";

LedStripeObject gelaender = {
  { "Gelaender", "OFF", 0, 255, 0, 100, DEVICE_NAME },
  Adafruit_NeoPixel(100, 21, NEO_GRB + NEO_KHZ800),
  xSemaphoreCreateMutex(),
  true
};

std::vector<LedStripeObject*> globalLedStripeObjects;

unsigned long lastTime = 0;

void setup() {
  globalLedStripeObjects.push_back(&gelaender);
  Serial.begin(115200);
  connectToWiFi();
  connectClient(DEVICE_NAME);

  // POST LED stripe objects
  for (const auto& globalLedStripeObject : globalLedStripeObjects)
    postLedStripeObject(serializeSLedStripeObject(globalLedStripeObject->object));
  gelaender.strip.begin();
  gelaender.strip.show();
  xTaskCreatePinnedToCore(RestClientTask, "RestClientTask", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(GelaenderTask, "GelaenderTask", 40960, &gelaender, 1, NULL, 1);
}

void loop() {
  // Nothing needed here. Work is done in tasks.
}

void GelaenderTask(void* pvParameters) {
  LedStripeObject* led = static_cast<LedStripeObject*>(pvParameters);
  runLedStripe(*led);
  vTaskDelete(NULL);
}

void runLedStripe(LedStripeObject& ledStripe) {
  Serial.printf("[GelaenderTask] Stack watermark: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));

  std::string mode;
  unsigned int red, green, blue, brightness;

  // Callback to stop never-ending LED routines
  const StopCallBack checkChanged = [&ledStripe]() -> bool {
    bool changed = false;
    if (xSemaphoreTake(ledStripe.mutex, portMAX_DELAY)) {
      changed = ledStripe.changed;
      xSemaphoreGive(ledStripe.mutex);
    }
    return changed;
  };
  Serial.printf("[GelaenderTask] Stack watermark (post-effect): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  while (true) {
    if (xSemaphoreTake(ledStripe.mutex, portMAX_DELAY)) {
      Serial.println("Checking for local change");
      if (ledStripe.changed) {
        Serial.println("changed!");
        Serial.printf("[GelaenderTask] Stack watermark (post-effect): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
        mode = ledStripe.object.mode;
        red = ledStripe.object.red;
        green = ledStripe.object.green;
        blue = ledStripe.object.blue;
        brightness = ledStripe.object.brightness;
        ledStripe.changed = false;
      }
      Serial.println("here1");
      xSemaphoreGive(ledStripe.mutex);
      Serial.println("here2");
      Serial.printf("[GelaenderTask] Stack watermark (post-effect): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
    }

    Serial.println("here3");
    if (mode == "OFF") {
      Serial.println("OFF");
      colorAll(ledStripe.strip, 100, 0, 0, 100);
    } else if (mode == "COLOR") {
      Serial.println("COLOR");
      colorAll(ledStripe.strip, red, green, blue, brightness);
    } else if (mode == "THEATER") {
      Serial.println("THEATER");
      theaterChase(ledStripe.strip, red, green, blue, brightness, checkChanged);
    } else if (mode == "RAINBOW") {
      Serial.println("RAINBOW");
      rainbow(ledStripe.strip, brightness, checkChanged);
    } else {
      Serial.println("NO MODE!!!");
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void RestClientTask(void* pvParameters) {
  while (true) {
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= RECONNECT_INTERVAL) {
      lastTime = currentTime;

      connectClient(DEVICE_NAME);

      std::vector<LedStripeObjectTO> ledStripeObjects =
        parseLedStripeObjectArrayJson(getUpdate(DEVICE_NAME).c_str());

      for (const auto& ledStripeObject : ledStripeObjects) {
        for (auto* globalLedStripeObject : globalLedStripeObjects) {
          if (ledStripeObject.name == globalLedStripeObject->object.name) {
            updateLedStripeObject(ledStripeObject, *globalLedStripeObject);
          }
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void updateLedStripeObject(const LedStripeObjectTO& srcObject, LedStripeObject& targetObject) {
  if (xSemaphoreTake(targetObject.mutex, portMAX_DELAY)) {
    if (targetObject.object.mode != srcObject.mode) {
      Serial.print("Mode: ");
      Serial.println(srcObject.mode.c_str());
      targetObject.object.mode = srcObject.mode;
      targetObject.changed = true;
    }
    if (targetObject.object.red != srcObject.red) {
      targetObject.object.red = srcObject.red;
      targetObject.changed = true;
      Serial.print("red: ");
      Serial.println(srcObject.red);
    }
    if (targetObject.object.green != srcObject.green) {
      targetObject.object.green = srcObject.green;
      targetObject.changed = true;
      Serial.print("green: ");
      Serial.println(srcObject.green);
    }
    if (targetObject.object.blue != srcObject.blue) {
      targetObject.object.blue = srcObject.blue;
      targetObject.changed = true;
      Serial.print("blue: ");
      Serial.println(srcObject.blue);
    }
    if (targetObject.object.brightness != srcObject.brightness) {
      targetObject.object.brightness = srcObject.brightness;
      targetObject.changed = true;
      Serial.print("brightness: ");
      Serial.println(srcObject.brightness);
    }
    xSemaphoreGive(targetObject.mutex);
  }
}
