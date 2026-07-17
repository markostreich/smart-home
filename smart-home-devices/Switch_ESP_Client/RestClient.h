#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include "LocalLan.h"
#include "ServerInfo.h"
#include "RootCa.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

constexpr const char* CONNECT_API = "device/connect";
constexpr const char* UPDATE_API = "switch/update";
constexpr const char* POST_API = "switch/object";
const String connectApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(CONNECT_API);
const String updateApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(UPDATE_API) + "/";
const String postApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(POST_API);

WiFiClientSecure httpsClient;
HTTPClient httpsHttpClient;
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
    httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    //httpsClient.setInsecure();

    httpsHttpClient.begin(httpsClient, connectApi);
    httpsHttpClient.setReuse(true);
    httpsHttpClient.addHeader("Content-Type", "application/json");

    const String httpRequestData = "{\"name\":\"" + String(clientId) + "\"}";
    Serial.print("Connecting device: ");
    Serial.println(httpRequestData);
    int httpResponseCode = httpsHttpClient.POST(httpRequestData);
    const String response = httpsHttpClient.getString();

    if (httpResponseCode > 0) {
      logHttpResponse("Device connect", httpResponseCode, response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    httpsHttpClient.end();
  } else {
    Serial.println("Error in WiFi connection");
    httpsClient.stop();
    WiFi.disconnect();
    connectToWiFi();
  }
#endif  // not TEST_MODE
}

void postSwitchObject(const String& switchObject) {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    httpsClient.setCACert(root_ca);

    // Insecure mode for development/testing (don't use in production)
    // httpsClient.setInsecure();

    httpsHttpClient.begin(httpsClient, postApi);
    httpsHttpClient.setReuse(true);
    httpsHttpClient.addHeader("Content-Type", "application/json");

    Serial.print("Posting switch object: ");
    Serial.println(switchObject);
    int httpResponseCode = httpsHttpClient.POST(switchObject);
    String response = httpsHttpClient.getString();

    if (httpResponseCode > 0) {
      logHttpResponse("Switch object POST", httpResponseCode, response);
      if (httpResponseCode != HTTP_CODE_CREATED && httpResponseCode != HTTP_CODE_ACCEPTED) {
        Serial.println("Switch object was not saved or updated successfully.");
      }
    } else {
      Serial.print("POST request failed. Code: ");
      Serial.println(httpResponseCode);
    }

    httpsHttpClient.end();
  } else {
    Serial.println("WiFi not connected");
    httpsClient.stop();
    WiFi.disconnect();
    connectToWiFi();
  }
#endif  // not TEST_MODE
}

String getUpdate(const char* clientId) {
#ifndef TEST_MODE
  String json = "";
  if (WiFi.status() == WL_CONNECTED) {
    httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    // httpsClient.setInsecure();

    httpsHttpClient.begin(httpsClient, updateApi + String(clientId));
    httpsHttpClient.setReuse(true);
    const int httpResponseCode = httpsHttpClient.GET();
    lastUpdateHttpResponseCode = httpResponseCode;

    if (httpResponseCode == HTTP_CODE_NO_CONTENT) {
      logHttpResponse("Switch update GET", httpResponseCode, "");
    } else if (httpResponseCode > 0) {
      const String response = httpsHttpClient.getString();
      logHttpResponse("Switch update GET", httpResponseCode, response);
      json = response;
    } else {
      Serial.print("Error on sending GET: ");
      Serial.println(httpResponseCode);
    }

    httpsHttpClient.end();
  } else {
    Serial.println("Error in WiFi connection");
    lastUpdateHttpResponseCode = 0;
    httpsClient.stop();
    WiFi.disconnect();
    connectToWiFi();
  }
  return json;
#else
  return "{\"name\": \"Schnittlauch\", \"state\": true, \"duration\": 15, \"deviceName\": \"Beet 1\"}";
#endif  // not TEST_MODE
}

#endif  // REST_CLIENT_H
