#ifndef LED_STRIPE_OBJECT
#define LED_STRIPE_OBJECT

#include <string>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

/**
 * @brief Struc representing an LED stripe object to transport stripe configurations.
 *
 * Example JSON representation:
 * {
 *   "name": "Device_One",
 *   "name": "THEATER",
 *   "red": 255;
 *   "green": 255;
 *   "blue": 0;
 *   "brightness": 255;
 *   "deviceName": "Device One"
 * }
 */
struct LedStripeObjectTO {

  std::string name;

  std::string mode;

  unsigned int red;

  unsigned int green;

  unsigned int blue;

  unsigned int brightness;

  std::string deviceName;
};

struct LedStripeObject {

  LedStripeObjectTO object;

  Adafruit_NeoPixel strip;

  SemaphoreHandle_t mutex;

  bool changed;
};

#endif //LED_STRIPE_OBJECT