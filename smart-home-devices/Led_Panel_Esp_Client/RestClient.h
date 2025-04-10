#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include "LocalLan.h"
#include "ServerInfo.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

const String connectApi = String(SERVER_URL) + "/" + String(CONNECT_API);
const String updateApi = String(SERVER_URL) + "/" + String(UPDATE_API) + "/" + String(CLIENT_ID);

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

void postClientId() {
#ifndef TEST_MODE
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    // httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    // httpsClient.setInsecure();

    HTTPClient http;
    http.begin(httpsClient, connectApi);
    http.addHeader("Content-Type", "application/json");

    const String httpRequestData = "{\"clientId\":\"" + String(CLIENT_ID) + "\"}";
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      const String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
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

String getUpdate() {
#ifndef TEST_MODE
  String json = "";
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure httpsClient;
    // httpsClient.setCACert(root_ca);

    // For testing purposes, you can bypass SSL certificate verification (NOT recommended for production).
    // httpsClient.setInsecure();

    HTTPClient http;
    http.begin(httpsClient, updateApi);
    const int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      json = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(json);
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
  return "{\"name\": \"restJsonName\",\"pos_x\":0,\"pos_y\":0,\"rotationPoint_x\":5,\"rotationPoint_y\":5,\"imageData\":\"1112A7A2D70A12A7A277\"}";
#endif  // not TEST_MODE
}

#endif // REST_CLIENT_H