#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include "LocalLan.h"
#include "ServerInfo.h"
#include "RootCa.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

constexpr const char* CONNECT_API = "device/connect";
constexpr const char* UPDATE_API = "ledstripe/update";
constexpr const char* POST_API = "ledstripe/stripeobject";
const String connectApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(CONNECT_API);
const String updateApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(UPDATE_API) + "/";
const String postApi = String(SERVER_URL) + ":" + HTTPS_PORT + "/" + String(POST_API);

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
}

void connectClient(const char* clientId) {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    //httpsClient.setInsecure();

    HTTPClient http;
    http.begin(httpsClient, connectApi);
    http.addHeader("Content-Type", "application/json");

    const String httpRequestData = "{\"name\":\"" + String(clientId) + "\"}";
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      const String response = http.getString();
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
#endif  // not TEST_MODE
}

void postLedStripeObject(const String& ledStripeObject) {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    httpsClient.setCACert(root_ca);

    // Insecure mode for development/testing (don't use in production)
    // httpsClient.setInsecure();

    HTTPClient http;
    http.begin(httpsClient, postApi);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(ledStripeObject);

    if (httpResponseCode > 0) {
      String response = http.getString();
    } else {
      Serial.print("POST request failed. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
#endif  // not TEST_MODE
}

String getUpdate(const char* clientId) {
#ifndef TEST_MODE
  String json = "";
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    // httpsClient.setInsecure();

    HTTPClient http;
    http.begin(httpsClient, updateApi + String(clientId));
    const int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      json = http.getString();
    } else {
      Serial.print("Error on sending GET: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
  return json;
#else
  return "{\"name\": \"Wand_1\", \"mode\": "THEATER", \"red\": 255, \"green\": 0, \"blue\": 255, \"brightness\": 100, \"deviceName\": \"Raum_1\"}";
#endif  // not TEST_MODE
}

#endif  // REST_CLIENT_H