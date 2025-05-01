#ifndef LEDS_LED_STRIPE_OBJECTS
#define LEDS_LED_STRIPE_OBJECTS

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

typedef std::function<bool()> StopCallBack;

void colorAll(Adafruit_NeoPixel strip, const uint8_t red, const uint8_t green, const int8_t blue, const uint8_t brightn) {
  for (uint8_t i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, red, green, blue);    //  Set pixel's color (in RAM)
  }
  strip.setBrightness(brightn);
  strip.show();
  delay(50);
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(Adafruit_NeoPixel strip, const int8_t red, const int8_t green, const int8_t blue, const uint8_t brightn, const StopCallBack stopCallback) {
  uint8_t a = 0;
  bool repeate = true;
  while (repeate) {
    a++;
    for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
      strip.clear();               //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, red, green, blue);  // Set pixel 'c' to value 'color'
      }
      strip.setBrightness(brightn);
      strip.show();  // Update strip with new contents
      delay(100);    // Pause for a moment
    }
    if (a % 2 && stopCallback) {
      repeate = false;
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(Adafruit_NeoPixel strip, const uint8_t brightn, const StopCallBack stopCallback) {
  bool repeate = true;
  while (repeate) {
    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this loop:
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      // strip.rainbow() can take a single argument (first pixel hue) or
      // optionally a few extras: number of rainbow repetitions (default 1),
      // saturation and value (brightness) (both 0-255, similar to the
      // ColorHSV() function, default 255), and a true/false flag for whether
      // to apply gamma correction to provide 'truer' colors (default true).
      strip.rainbow(firstPixelHue);
      strip.setBrightness(brightn);
      // Above line is equivalent to:
      // strip.rainbow(firstPixelHue, 1, 255, 255, true);
      strip.show();  // Update strip with new contents
      delay(50);     // Pause for a moment
      if (firstPixelHue % 2 == 0 && stopCallback) {
        repeate = false;
        break;
      }
    }
  }
}

#endif  // LEDS_LED_STRIPE_OBJECTS