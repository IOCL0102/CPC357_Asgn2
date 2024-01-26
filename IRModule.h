#pragma once

class IRModule{
  private:
    int irSensorPin, redLedPin, greenLedPin;

  public:

  IRModule(){}

  void setup(int irSensorPin, int redLedPin, int greenLedPin){
    this->irSensorPin = irSensorPin;
    this->redLedPin = redLedPin;
    this->greenLedPin = greenLedPin;

    pinMode(irSensorPin, INPUT); // IR Sensor pin INPUT
    pinMode(redLedPin, OUTPUT); // Red LED Pin Output
    pinMode(greenLedPin, OUTPUT); // Green LED Pin Output
  }

  bool monitor(bool verbose = true){
      bool isNear = !digitalRead(irSensorPin); // if object is near then return 0  

      if(isNear){
        digitalWrite(redLedPin, HIGH);
        digitalWrite(greenLedPin, LOW);
        if(verbose){
          Serial.println("The car is near, red LED ON ! You may stop your car now");
        }
      } else {
        digitalWrite(greenLedPin, HIGH);
        digitalWrite(redLedPin, LOW);
        if(verbose){
          Serial.println("The car is far, green LED ON ! Please move towards here");
        }
      }
      return isNear;
  }

};