#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <Wire.h>
#define DHTPIN    23
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
int value1, value2;

String server = "http://maker.ifttt.com";
String eventName = "new_data";
String IFTTT_Key = "oLPl3VTknlSUN7TkYGosXWbJR10xMF3Onmf9_ws9-Cy";
String IFTTTUrl="https://maker.ifttt.com/trigger/new_data/with/key/oLPl3VTknlSUN7TkYGosXWbJR10xMF3Onmf9_ws9-Cy";
const char * ssid = "Dip-Home";
const char * password = "abcd1234";
String GOOGLE_SCRIPT_ID = "AKfycbxXNpgxs1ZliKbKTrLJf4nQ8TjSqnGN2r0vM0fBaXPO13kI7B0CikaBh5PrhJcYFR8sNg";

void setup() {

  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Wifi Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println("Wifi Connected !!!");  
}

void loop() {
  value1 = dht.readHumidity();
  value2 = dht.readTemperature();
  Serial.println("sensor data sending..........");
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String((int)value1) + "&value2="+String((int)value2);  
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0){
   if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      Serial.println("data send successfully");
    }
  }
  else Serial.printf("HTTP Request failed, exception: %s\n", http.errorToString(httpCode).c_str());
  Serial.println("command receiving from cloud");
  String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read";
  Serial.println("Making a request");
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  String payload;
  if (httpCode > 0) {
       if(httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
  }
  }
  else Serial.printf("HTTP Request failed, error: %s\n", http.errorToString(httpCode).c_str());
  http.end();
}
