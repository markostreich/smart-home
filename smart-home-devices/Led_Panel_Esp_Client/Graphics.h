#ifndef GRAPHICS_LED_PANEL_OBJECTS
#define GRAPHICS_LED_PANEL_OBJECTS

#include <memory>
#include <string>
#include "LedPanelObject.h"

#ifndef TEST_MODE
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif                  // __AVR__
#endif                  // not TEST_MODE

/* LED Potentiometer */
#define POT 13

#ifndef TEST_MODE
/** @brief Amount of pixels in the LED panel. */
//constexpr uint16_t NUMPIXELS = 600;
constexpr uint16_t NUMPIXELS = 16 * 16;

/** @brief LED data pin. */
constexpr uint8_t PIN = 21;

/** @brief Interface to graphical hardware. */
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#endif  // not TEST_MODE

/** @brief Global variable for LED panel brightness. */
uint16_t brightness = 0;

/** @brief Position of the first LED in the bottommost row. */
constexpr boolean startLeft = true;

/** @brief Size in x-direction of the LED panel. */
constexpr uint8_t size_x = 16;

/** @brief Size in y-direction of the LED panel. */
constexpr uint8_t size_y = 16;

/**
 * @brief Draws a single pixel on a LED panel with specified color.
 *
 * This function sets the color of a single pixel on a LED panel. The pixel's position is determined by the x and y coordinates provided.
 * The color is specified by the red, green, and blue components. The function first checks if the provided coordinates are within the bounds
 * of the LED panel. If the coordinates are out of bounds, the function returns without drawing the pixel.
 *
 * The function also considers the starting position of the LED panel (left or right) to correctly map the x coordinate to the physical
 * layout of the LED panel. This is particularly useful for panels that are not aligned with the conventional left-to-right or right-to-left
 * orientation.
 *
 * @note In test mode, this function prints the pixel drawing details to the serial monitor instead of setting the pixel color.
 *
 * @param x The x-coordinate of the pixel to be drawn.
 * @param y The y-coordinate of the pixel to be drawn.
 * @param red The red component of the color.
 * @param green The green component of the color.
 * @param blue The blue component of the color.
 */
void drawPixel(int8_t x, int8_t y, uint8_t red, uint8_t green, uint8_t blue) {
  // Check if the coordinates are within the bounds of the LED panel
  if (x >= size_x || y >= size_y || x < 0 || y < 0)
    return;

  // Calculate the pixel position based on the starting position (left or right)
  const int8_t pos_x = startLeft ? (y % 2 == 0 ? x : size_x - 1 - x) : (y % 2 == 1 ? x : size_x - 1 - x);
#ifdef TEST_MODE
  Serial.print("Drawing Pixel: ");
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(", y: ");
  Serial.print(y);
  Serial.print(", red: ");
  Serial.print(red);
  Serial.print(", green: ");
  Serial.print(green);
  Serial.print(", blue: ");
  Serial.println(blue);
#else
  pixels.setPixelColor(pos_x + size_x * y, pixels.Color(red, green, blue));
#endif  // TEST_MODE
}

/**
 * @brief Adjusts the brightness of the LED panel based on the potentiometer value.
 *
 * This function reads the potentiometer value and maps it to a range suitable for setting the LED panel's brightness.
 * It then updates the brightness if the new value differs significantly from the current brightness, within a tolerance range.
 * This approach minimizes unnecessary updates to the LED panel, improving efficiency.
 *
 * @note This function is designed to be used in both normal operation and test mode. In test mode, it prints the calculated brightness value to the serial monitor instead of adjusting the LED panel's brightness.
 *
 * @warning The brightness adjustment is sensitive to small changes in the potentiometer value. Ensure the potentiometer is properly connected and calibrated for accurate brightness control.
 *
 * @bug None known.
 */
void adjustBrightness() {
  /* Set Brightness with potentiometer */
  uint16_t potValue = analogRead(POT);
  uint16_t newBrightness = map(potValue, 0, 1023, 1, 255);
  // Update brightness if there's a significant change
  // This threshold can be adjusted based on the desired sensitivity
  const uint8_t brightnessThreshold = 1;
  if (abs(brightness - newBrightness) > brightnessThreshold) {
    brightness = newBrightness;
#ifndef TEST_MODE
    pixels.setBrightness(brightness);
    pixels.show();
#else
    Serial.print("potentio: ");
    Serial.println(brightness);
#endif  // not TEST_MODE
  }
}

/**
 * @brief Draws an object on a LED panel.
 *
 * This function iterates over the image data of a given LED panel object, drawing each pixel
 * at its specified position with the corresponding color. The image data is expected to be
 * in a specific format where each pixel's position and color are encoded in a sequence of
 * five consecutive elements: x position, y position, red color component, green color component,
 * and blue color component.
 *
 * @param ledPanelObject A pointer to the LED panel object containing the image data to be drawn.
 */
void drawObject(const LedPanelObject* ledPanelObject) {
  adjustBrightness();
  const int8_t pos_x = ledPanelObject->pos_x;
  const int8_t pos_y = ledPanelObject->pos_y;
  for (int i = 0; i < ledPanelObject->imageData_length; i += 5) {
    const int8_t x = pos_x + ledPanelObject->imageData[i];
    const int8_t y = pos_y + ledPanelObject->imageData[i + 1];
    const int8_t r = ledPanelObject->imageData[i + 2];
    const int8_t g = ledPanelObject->imageData[i + 3];
    const int8_t b = ledPanelObject->imageData[i + 4];
    drawPixel(x, y, r, g, b);
  }
  pixels.show();
}

/**
 * @brief Draws a pixel on the LED panel with specified color and rotation.
 *
 * This function calculates the position of a pixel on the LED panel after applying a rotation transformation.
 * The rotation is defined by a rotation center point (`rot_x`, `rot_y`) and the rotation angle (`cosTau`, `sinTau`).
 * The function then calls `drawPixel` with the transformed coordinates and the specified RGB color values.
 *
 * @param x The original x-coordinate of the pixel on the LED panel.
 * @param y The original y-coordinate of the pixel on the LED panel.
 * @param rot_x The x-coordinate of the rotation center point.
 * @param rot_y The y-coordinate of the rotation center point.
 * @param cosTau The cosine of the rotation angle.
 * @param sinTau The sine of the rotation angle.
 * @param red The red component of the pixel's color.
 * @param green The green component of the pixel's color.
 * @param blue The blue component of the pixel's color.
 *
 * @note This function assumes that the rotation angle is provided in radians and that the rotation center point is specified in the same coordinate system as the pixel coordinates.
 *
 * @warning The function does not perform bounds checking on the RGB color values. Ensure that the values are within the valid range (0-255).
 *
 * @bug None known.
 */
void drawRotatedPixel(int8_t x, int8_t y, float rot_x, float rot_y, float cosTau, float sinTau, uint8_t red, uint8_t green, uint8_t blue, bool doFill) {
  float new_x = rot_x + cosTau * ((float)x - rot_x) + sinTau * ((float)y - rot_y);
  float new_y = rot_y - sinTau * ((float)x - rot_x) + cosTau * ((float)y - rot_y);
  //float new_x = cosTau * ((float)x - 14.5) + sinTau * ((float)y-9.5);
  //float new_y = -sinTau * ((float)x - 14.5) + cosTau * ((float)y-9.5);
  int8_t int_new_x_up = (int8_t)(new_x + 0.5);
  int8_t int_new_y_up = (int8_t)(new_y + 0.5);
  int8_t int_new_x = (int8_t)new_x;
  int8_t int_new_y = (int8_t)new_y;
  drawPixel(int_new_x_up, int_new_y_up, red, green, blue);
  if (doFill && (int_new_x_up != int_new_x || int_new_y_up != int_new_y))
    drawPixel(int_new_x, int_new_y, red, green, blue);
}

void drawRotatedObject(const LedPanelObject* ledPanelObject, float angle) {
  adjustBrightness();
  float angle_rad = angle * M_PI / 180.0;
  float cosTau = cos(angle_rad);
  float sinTau = sin(angle_rad);
  const int8_t pos_x = ledPanelObject->pos_x;
  const int8_t pos_y = ledPanelObject->pos_y;
  bool doFill = angle == 0.0 || angle == 45.0 || angle == 180.0 || angle == 270.0 || angle == 360.0 ? false : true;
  for (int i = 0; i < ledPanelObject->imageData_length; i += 5) {
    const int8_t x = pos_x + ledPanelObject->imageData[i];
    const int8_t y = pos_y + ledPanelObject->imageData[i + 1];
    const int8_t r = ledPanelObject->imageData[i + 2];
    const int8_t g = ledPanelObject->imageData[i + 3];
    const int8_t b = ledPanelObject->imageData[i + 4];
    drawRotatedPixel(x, y, ledPanelObject->rotationPoint_x, ledPanelObject->rotationPoint_y, cosTau, sinTau, r, g, b, doFill);
  }
  pixels.show();
}

/**
 * @brief Sets the color of all pixels on the LED panel.
 *
 * This function iterates over each pixel on the LED panel and sets its color to the specified RGB values.
 * It first adjusts the brightness of the LED panel using the `adjustBrightness` function, ensuring that the brightness is set according to the current potentiometer value.
 * Then, it calls `drawPixel` for each pixel on the panel, setting its color to the provided RGB values.
 *
 * @param red The red component of the color.
 * @param green The green component of the color.
 * @param blue The blue component of the color.
 *
 * @note This function assumes that the RGB values are within the valid range (0-255).
 *
 * @warning The function does not perform bounds checking on the RGB color values. Ensure that the values are within the valid range (0-255).
 *
 * @bug None known.
 */
void colorAll(uint8_t red, uint8_t green, uint8_t blue) {
  adjustBrightness();
  for (int8_t y = 0; y < size_y; ++y)
    for (int8_t x = 0; x < size_x; ++x)
      drawPixel(x, y, red, green, blue);
}

/**
 * @brief Sets the background color of the LED panel.
 *
 * This function sets the background color of the LED panel to a predefined color, which is a dim gray (5, 5, 5).
 * It calls `colorAll` with the predefined RGB values to set the color of all pixels on the panel.
 *
 * @note This function is typically used to initialize the LED panel or to reset its display to a default state.
 *
 * @bug None known.
 */
void colorBackground() {
  uint8_t red = 5;
  uint8_t green = 5;
  uint8_t blue = 5;
  colorAll(red, green, blue);
}
#endif  // GRAPHICS_LED_PANEL_OBJECTS