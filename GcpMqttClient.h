#pragma once

#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include "time.h"


const char *WIFI_SSID = "test";
const char *WIFI_PASSWORD = "test1234";
const char *MQTT_SERVER = "34.42.64.55";
const int MQTT_PORT = 1883;
// const char *MQTT_TOPIC = "iot";


// Configuration of NTP 
// const char* NTPServer = "pool.ntp.org";
// const long  gmtOffset_sec = 28800;
// const int   daylightOffset_sec = 0;

class GcpMqttClient{

private:
  WiFiClient espClient;
  PubSubClient client;

public:

  GcpMqttClient() : client(espClient) {}

  void setup_wifi() {     
    delay(10);     
    Serial.println("Connecting to WiFi...");     
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);     
    while (WiFi.status() != WL_CONNECTED){         
      delay(500);         
      Serial.print(".");     
    }     
    
    Serial.println("WiFi connected");     
    Serial.println("IP address: ");     
    Serial.println(WiFi.localIP()); 
  } 

  void reconnect() {     
    while (!this->client.connected()){         
      Serial.println("Attempting MQTT connection...");         
      if (client.connect("ESP32Client")){             
        Serial.println("Connected to MQTT server");         
      }         
      else {             
        Serial.print("Failed, rc=");             
        Serial.print(client.state());             
        Serial.println(" Retrying in 5 seconds...");             
        delay(5000);         
      }     
    } 
  } 

  void setup() {
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);

    // configTime(gmtOffset_sec, daylightOffset_sec, NTPServer); 
  }

  String getDateTime() {
      struct tm tm;
      if(!getLocalTime(&tm)){
        Serial.println("Failed to obtain time");
        String currentTime1 = "1999-01-01 00:00:00.000000+08:00";
        return currentTime1;
      }
    
      int monthDay = tm.tm_mday;   
      int currentMonth = tm.tm_mon +1;  
      int currentYear = tm.tm_year +1900;  
      
      String currentTime = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + String(tm.tm_hour) + ":" + String(tm.tm_min) + ":" + String(tm.tm_sec) + ".000000+08:00";
      return currentTime;
  }

  void loop() {
    if(!this->client.connected()){
      this->reconnect();
    }
    this->client.loop();
  }

  void publish(String topic, JSONVar& payloadObject){
    // payloadObject["timestamp"] = this->getDateTime();
    String jsonString = JSON.stringify(payloadObject);   
    Serial.print("Sending data to server , payload : ");
    Serial.println(jsonString);
    this->client.publish(topic.c_str(), jsonString.c_str());
  }


};
