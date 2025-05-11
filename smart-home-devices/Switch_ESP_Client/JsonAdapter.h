#ifndef SWITCH_JSON_ADAPTER
#define SWITCH_JSON_ADAPTER

#include <string>
#include <vector>
#include <ArduinoJson.h>
#include <exception>
#include "SwitchObject.h"

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

SwitchObjectTO parseSwitchObjectJson(const char* json) {
  JsonDocument doc;
  const DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    throw JsonParseException();
  }
  SwitchObjectTO result;
  result.name = doc["name"].as<std::string>();
  result.state = doc["state"];
  result.duration = doc["duration"];
  result.deviceName = doc["deviceName"].as<std::string>();
  return result;
}

std::vector<SwitchObjectTO> parseSwitchObjectArrayJson(const char* json) {
  JsonDocument doc;
  const DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    throw JsonParseException();
  }

  std::vector<SwitchObjectTO> results;
  for (JsonObject item : doc.as<JsonArray>()) {
    SwitchObjectTO obj;
    obj.name = item["name"].as<std::string>();
    obj.state = item["state"];
    obj.duration = item["duration"];
    obj.deviceName = item["deviceName"].as<std::string>();
    results.push_back(obj);
  }
  return results;
}

String serializeSwitchObject(const SwitchObjectTO& obj) {
  JsonDocument doc;
  doc["name"] = obj.name;
  doc["state"] = obj.state;
  doc["duration"] = obj.duration;
  doc["deviceName"] = obj.deviceName;

  String output;
  serializeJson(doc, output);
  return output;
}

#endif  //SWITCH_JSON_ADAPTER