#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <Wire.h>
#define DHTPIN    35
#define DHTTYPE DHT11
#define GAS_SENSOR A0
int value1, value2, value3, value4;
int buzzer = 13;
const int flame_sensor = 34; 

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
     
int sensorThreshold = 400;
String server = "http://maker.ifttt.com";
String eventName = "new_data";
String IFTTT_Key = "oLPl3VTknlSUN7TkYGosXWbJR10xMF3Onmf9_ws9-Cy";
const char * ssid = "Dip-Home";
const char * password = "abcd1234";
String GOOGLE_SCRIPT_ID = "AKfycbxXNpgxs1ZliKbKTrLJf4nQ8TjSqnGN2r0vM0fBaXPO13kI7B0CikaBh5PrhJcYFR8sNg";
void setup() {

  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
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
  value3 = analogRead(GAS_SENSOR);
  value4 = digitalRead(flame_sensor);
  Serial.println(value1);
  Serial.println(value2);
  Serial.println(value3);
  Serial.println(value4);

  if (value3 > sensorThreshold){
   Serial.print("GAS besi");
  digitalWrite(buzzer, HIGH);
  }
  if(value4 == 1) {           
  Serial.print("FIRE DETECTED!"); 
  digitalWrite(buzzer, HIGH);
  send_event("fireprevention");  
  }
  data_send(value2,value3,value4);
}
void data_send(int value1,int value2,int value3)
{

  Serial.println("sensor data sending..........");
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + 
               "?value1=" + String((int)value1) + "&value2="+String((int)value2) + "&value3="+String((int)value3);  
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

}
void data_read(){
    Serial.println("command receiving from cloud");
  String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read";
  Serial.println("Making a request");
   HTTPClient http;
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
void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(server);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += IFTTT_Key;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
