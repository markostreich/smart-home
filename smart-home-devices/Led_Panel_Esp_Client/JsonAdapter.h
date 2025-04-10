#ifndef LED_PANEL_JSON_ADAPTER
#define LED_PANEL_JSON_ADAPTER

#include <memory>
#include <string>
#include <ArduinoJson.h>
#include <exception>
#include "LedPanelObject.h"
#include "HexToByte.h"

JsonDocument doc;

/**
 * @brief Custom exception class for memory allocation failure.
 *
 * This exception is thrown when memory allocation for imageData fails.
 */
class MemoryAllocationException : public std::runtime_error {
public:
  MemoryAllocationException()
    : std::runtime_error("Memory allocation for imageData failed") {}
};

/**
 * @brief Custom exception class for parsing of JSON.
 *
 * This exception is thrown when there is an error in parsing JSON data.
 */
class JsonParseException : public std::exception {
public:
  const char* what() const noexcept override {
    return "Unable to parse JSON!";
  }
};

/**
 * @brief Parses a JSON string to populate a LedPanelObject struct.
 *
 * This function takes a JSON string as input, attempts to deserialize it, and then
 * extracts the relevant fields to populate a LedPanelObject struct. The JSON string
 * is expected to contain fields for the name, position (pos_x, pos_y), rotation point
 * (rotationPoint_x, rotationPoint_y), and image data (imageData). The image data is
 * expected to be a hexadecimal string representing byte values, which is then
 * converted to a byte array. A chunk of 5 hex numbers represents an image point in the
 * LedPanelObject, with the first two numbers indicating the x and y positions relative to
 * the whole object position on the panel, and the next three numbers representing the RGB
 * values.
 *
 * @param json A null-terminated string containing the JSON data to parse.
 * @return A LedPanelObject struct populated with the parsed data.
 * @throws MemoryAllocationException If memory allocation for imageData fails.
 * @throws JsonParseException If JSON parsing fails.
 */
LedPanelObject parseLedPanelJson(const char* json) {
  const DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    throw JsonParseException();
  }
  LedPanelObject result;
  result.name = doc["name"].as<std::string>();
  result.pos_x = doc["pos_x"];
  result.pos_y = doc["pos_y"];
  result.rotationPoint_x = doc["rotationPoint_x"];
  result.rotationPoint_y = doc["rotationPoint_y"];
  const char* imageData = doc["imageData"];
  result.imageData_length = strlen(imageData) / 2;  // Size of the byte array
  result.imageData = std::unique_ptr<byte[]>(new byte[result.imageData_length]);
  if (!result.imageData) {
    throw MemoryAllocationException();
  }
  try {
    hexStringToByteArray(imageData, result.imageData.get(), result.imageData_length);
  } catch (const InvalidHexCharacterException& e) {
    result.imageData_length = 0;
    throw JsonParseException();
  }
  return result;
}

#endif  //LED_PANEL_JSON_ADAPTER