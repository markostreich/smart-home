#ifndef LED_STRIPE_JSON_ADAPTER
#define LED_STRIPE_JSON_ADAPTER

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include <exception>
#include "LedStripeObject.h"

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

LedStripeObjectTO parseLedStripeObjectJson(const char* json) {
  DynamicJsonDocument doc(2048);
  const DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    throw JsonParseException();
  }
  LedStripeObjectTO result;
  result.name = doc["name"].as<std::string>();
  result.mode = doc["mode"].as<std::string>();
  result.red = doc["red"];
  result.green = doc["green"];
  result.blue = doc["blue"];
  result.brightness = doc["brightness"];
  result.deviceName = doc["deviceName"].as<std::string>();
  return result;
}

std::vector<LedStripeObjectTO> parseLedStripeObjectArrayJson(const char* json) {
  DynamicJsonDocument doc(2048);
  const DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    throw JsonParseException();
  }

  std::vector<LedStripeObjectTO> results;
  for (JsonObject item : doc.as<JsonArray>()) {
    LedStripeObjectTO obj;
    obj.name = item["name"].as<std::string>();
    obj.mode = item["mode"].as<std::string>();
    obj.red = item["red"];
    obj.green = item["green"];
    obj.blue = item["blue"];
    obj.brightness = item["brightness"];
    obj.deviceName = item["deviceName"].as<std::string>();
    results.push_back(obj);
  }
  return results;
}

String serializeSLedStripeObject(const LedStripeObjectTO& obj) {
  JsonDocument doc;
  doc["name"] = obj.name;
  doc["mode"] = obj.mode;
  doc["red"] = obj.red;
  doc["green"] = obj.green;
  doc["blue"] = obj.blue;
  doc["brightness"] = obj.brightness;
  doc["deviceName"] = obj.deviceName;

  String output;
  serializeJson(doc, output);
  return output;
}

#endif  //LED_STRIPE_JSON_ADAPTER