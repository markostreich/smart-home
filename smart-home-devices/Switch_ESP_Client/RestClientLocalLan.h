#ifndef REST_CLIENT_LOCAL_LAN_H
#define REST_CLIENT_LOCAL_LAN_H

#include "LocalLan.h"
#include "ServerInfo.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <time.h>

constexpr const char* CONNECT_API = "device/connect";
constexpr const char* UPDATE_API = "switch/update";
constexpr const char* POST_API = "switch/object";

const String connectApi = String(SERVER_URL) + ":" + HTTP_PORT + "/" + String(CONNECT_API);
const String updateApi = String(SERVER_URL) + ":" + HTTP_PORT + "/" + String(UPDATE_API) + "/";
const String postApi = String(SERVER_URL) + ":" + HTTP_PORT + "/" + String(POST_API);

int lastUpdateHttpResponseCode = 0;

void logHttpResponse(const char* operation, const int httpResponseCode, const String& response) {
  Serial.print(operation);
  Serial.print(" HTTP ");
  Serial.println(httpResponseCode);
  if (response.length() > 0) {
    Serial.print(operation);
    Serial.print(" response: ");
    Serial.println(response);
  }
}

void syncTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Syncing time");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }

  Serial.println("\nTime synced!");
}

void connectToWiFi() {
#ifndef TEST_MODE
  WiFi.begin(LOCAL_SSID, LOCAL_PW);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
#endif  // not TEST_MODE
  Serial.println("\nConnected to WiFi");
  syncTime();
}

void connectClient(const char* clientId) {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient lanClient;
    HTTPClient http;
    http.begin(lanClient, connectApi);
    http.addHeader("Content-Type", "application/json");

    const String httpRequestData = "{\"name\":\"" + String(clientId) + "\"}";
    Serial.print("Connecting device at ");
    Serial.print(connectApi);
    Serial.print(": ");
    Serial.println(httpRequestData);

    const int httpResponseCode = http.POST(httpRequestData);
    const String response = http.getString();

    if (httpResponseCode > 0) {
      logHttpResponse("Device connect", httpResponseCode, response);
    } else {
      Serial.print("Device connect failed. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
    WiFi.disconnect();
    connectToWiFi();
  }
#endif  // not TEST_MODE
}

void postSwitchObject(const String& switchObject) {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient lanClient;
    HTTPClient http;
    http.begin(lanClient, postApi);
    http.addHeader("Content-Type", "application/json");

    Serial.print("Posting switch object at ");
    Serial.print(postApi);
    Serial.print(": ");
    Serial.println(switchObject);

    const int httpResponseCode = http.POST(switchObject);
    const String response = http.getString();

    if (httpResponseCode > 0) {
      logHttpResponse("Switch object POST", httpResponseCode, response);
      if (httpResponseCode != HTTP_CODE_CREATED && httpResponseCode != HTTP_CODE_ACCEPTED) {
        Serial.println("Switch object was not saved or updated successfully.");
      }
    } else {
      Serial.print("Switch object POST failed. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
    WiFi.disconnect();
    connectToWiFi();
  }
#endif  // not TEST_MODE
}

String getUpdate(const char* clientId) {
#ifndef TEST_MODE
  String json = "";
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient lanClient;
    HTTPClient http;
    const String updateUrl = updateApi + String(clientId);
    http.begin(lanClient, updateUrl);

    Serial.print("Getting switch update from ");
    Serial.println(updateUrl);

    const int httpResponseCode = http.GET();
    lastUpdateHttpResponseCode = httpResponseCode;

    if (httpResponseCode == HTTP_CODE_NO_CONTENT) {
      logHttpResponse("Switch update GET", httpResponseCode, "");
    } else if (httpResponseCode > 0) {
      const String response = http.getString();
      logHttpResponse("Switch update GET", httpResponseCode, response);
      json = response;
    } else {
      Serial.print("Switch update GET failed. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
    lastUpdateHttpResponseCode = 0;
    WiFi.disconnect();
    connectToWiFi();
  }
  return json;
#else
  return "{\"name\": \"Schnittlauch\", \"state\": true, \"duration\": 15, \"deviceName\": \"Beet 1\"}";
#endif  // not TEST_MODE
}

#endif  // REST_CLIENT_LOCAL_LAN_H
