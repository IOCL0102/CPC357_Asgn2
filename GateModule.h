#pragma once

#include <Arduino_JSON.h>
#include <ESP32Servo.h>
#include "GcpMqttClient.h"


static const String GATE_MODULE_MQTT_TOPIC_NAME = "GATE_MODULE";

enum GateMode { GATE_IN, GATE_OUT };

class GateModule{
  private:
    int servoPin, buttonPin;
    GateMode mode;
    Servo gate;
    static int currCustomerCount;

  public:

  GateModule(){}

  void setup(int servoPin, int buttonPin, GateMode mode){
    this->servoPin = servoPin;
    this->buttonPin = buttonPin;
    gate.attach(servoPin);
    pinMode(buttonPin, INPUT); // Push Button Pin Output
    this->mode = mode;
  }

  void monitor(GcpMqttClient &gcpClient, bool verbose = true){
    bool isPressed = digitalRead(buttonPin);
    // Serial.print("Current customer count : ");
    // Serial.println(currCustomerCount);
    if(verbose){
      Serial.print(mode);
      Serial.print("-> monitoring :");
      Serial.println(isPressed);
    }

    if(isPressed){
      if(verbose){
        Serial.println("Button is pressed, gate opens now");
      }
      this->move(120, 0);

      JSONVar payload;
      if(this->mode == GATE_IN){
        currCustomerCount++;
      } else if (this->mode == GATE_OUT){
        currCustomerCount--;
      }
      payload["count"] = currCustomerCount;
      gcpClient.publish(GATE_MODULE_MQTT_TOPIC_NAME, payload);

      delay(3000);

      if(verbose){
        Serial.println("Gate is closing");
      }

      this->move(0, 120);
    }
  }

  void move(int initPos, int endPos){
    int step = 3;

    if(initPos >= endPos){
      for (int pos = initPos; pos >= endPos; pos -= 3) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        gate.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    } else {
      for (int pos = initPos; pos <= endPos; pos += 3) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        gate.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
  }

};

int GateModule::currCustomerCount = 0;