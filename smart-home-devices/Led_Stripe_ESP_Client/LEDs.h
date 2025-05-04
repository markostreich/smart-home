#ifndef LEDS_LED_STRIPE_OBJECTS
#define LEDS_LED_STRIPE_OBJECTS

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

typedef std::function<bool()> StopCallBack;

void colorAll(Adafruit_NeoPixel& strip, const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t brightn) {
  Serial.printf("[GelaenderTask] Stack watermark (post-effect): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  Serial.printf("Strip buffer ptr: %p\n", strip.getPixels());
  Serial.println("In colorAll");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, red, green, blue);
    vTaskDelay(1);
  }
  Serial.println("After for loop");
  strip.setBrightness(brightn);
  Serial.println("After strip.setBrightness");
  Serial.printf("[GelaenderTask] Stack watermark (post-effect): %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  strip.show();
  Serial.println("After strip.show");
  vTaskDelay(pdMS_TO_TICKS(50));
}

void theaterChase(Adafruit_NeoPixel& strip, const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t brightn, const StopCallBack stopCallback) {
  uint8_t a = 0;
  bool repeat = true;
  while (repeat) {
    a++;
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, red, green, blue);
        vTaskDelay(1);
      }
      strip.setBrightness(brightn);
      strip.show();
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (a % 2 == 0 && stopCallback && stopCallback()) {
      repeat = false;
    }
  }
}

void rainbow(Adafruit_NeoPixel& strip, const uint8_t brightn, const StopCallBack stopCallback) {
  bool repeat = true;
  while (repeat) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      strip.rainbow(firstPixelHue);
      strip.setBrightness(brightn);
      strip.show();
      vTaskDelay(pdMS_TO_TICKS(50));
      if (firstPixelHue % 2 == 0 && stopCallback && stopCallback()) {
        repeat = false;
        break;
      }
    }
  }
}

#endif  // LEDS_LED_STRIPE_OBJECTS
