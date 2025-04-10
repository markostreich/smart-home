#ifndef LED_PANEL_OBJECT
#define LED_PANEL_OBJECT

#include <memory>
#include <string>

/**
 * @brief Struct representing a LED panel object.
 *
 * This struct holds the data for a LED panel object, including its name, position,
 * rotation point, and image data. The image data is stored as a unique pointer to a
 * byte array, with the length of the array stored in `imageData_length`.
 *
 * Example JSON representation:
 * {
 *   "name": "",
 *   "pos_x":"",
 *   "pos_y":"",
 *   "rotationPoint_x":"",
 *   "rotationPoint_y":"",
 *   "imageData":"" //hexData
 * }
 */
struct LedPanelObject {
  /**
   * @brief The name of the LED panel object.
   */
  std::string name;

  /**
   * @brief The x-coordinate of the LED panel object's position.
   */
  int pos_x;

  /**
   * @brief The y-coordinate of the LED panel object's position.
   */
  int pos_y;

  /**
   * @brief The x-coordinate of the LED panel object's rotation point.
   */
  int rotationPoint_x;

  /**
   * @brief The y-coordinate of the LED panel object's rotation point.
   */
  int rotationPoint_y;

  /**
   * @brief Unique pointer to the byte array containing the image data.
   * A chunk of 5 hex numbers represents an image point in the
   * LedPanelObject, with the first two numbers indicating the x and y positions relative to
   * the whole object position on the panel, and the next three numbers representing the RGB
   * values.
   */
  std::unique_ptr<byte[]> imageData;

  /**
   * @brief The length of the image data byte array.
   */
  std::size_t imageData_length;
};

#endif  //LED_PANEL_OBJECT