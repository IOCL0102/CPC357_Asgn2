#pragma once

#include <Arduino_JSON.h>
#include "NewPing.h"
#include "GcpMqttClient.h"

#define LIGHT_UP_DISTANCE 3

static const String CAR_MODULE_MQTT_TOPIC_NAME = "CAR_DETECTOR_MODULE";

class CarDetectorModule{
  private:
    int triggerPin, echo1Pin, echo2Pin, detectedPin;
    NewPing frontSonar, backSonar;
    bool isCarDetected;
    int prevFrontDistance, prevBackDistance;
    int parkingLotId;

  public:
    CarDetectorModule(int triggerPin, int echo1Pin, int echo2Pin): 
    frontSonar(triggerPin, echo1Pin), backSonar(triggerPin, echo2Pin) {isCarDetected = false; prevFrontDistance=0; prevBackDistance=0;}

    void setup(int detectedPin, int parkingLotId){
      this->detectedPin = detectedPin;
      this->parkingLotId = parkingLotId;
      pinMode(detectedPin, OUTPUT); 
    }

    void monitor(GcpMqttClient &gcpClient, bool isNear, bool verbose = true){
      JSONVar payload;
      payload["parkingLotId"] = parkingLotId;

      if(!isNear){
        // Send data to server if the car leaves
        if(isCarDetected){
          payload["status"] = "leave";
          gcpClient.publish(CAR_MODULE_MQTT_TOPIC_NAME, payload);
        }
        isCarDetected = false;
        digitalWrite(detectedPin, LOW);
        return;
      }

      int frontAvg= 0, backAvg = 0;
      int avgCount = 10;
      for(int i = 0 ; i < avgCount; i++){
          frontAvg += frontSonar.ping_cm();
          backAvg += backSonar.ping_cm();
      }

      int distanceFront = (frontAvg/avgCount) * 0.4 + prevFrontDistance * 0.6;
      int distanceBack = (backAvg/avgCount)* 0.4 + prevBackDistance * 0.6;
      prevFrontDistance = distanceFront;
      prevBackDistance = distanceBack;

      if(verbose){
        Serial.print("Distance Front: ");
        Serial.print(distanceFront);
        Serial.print(" Distance back: ");
        Serial.println(distanceBack);
      }

      if(distanceFront <= LIGHT_UP_DISTANCE && distanceBack <= LIGHT_UP_DISTANCE && distanceFront != 0 && distanceBack != 0 && !isCarDetected){
        // Send data to server
        payload["status"] = "park";
        
        gcpClient.publish(CAR_MODULE_MQTT_TOPIC_NAME, payload);
        digitalWrite(detectedPin, HIGH);
        isCarDetected = true;
        Serial.println("..................................................CAR DETECTED !");
      }
    }

};