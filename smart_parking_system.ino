#include <Arduino_JSON.h>
#include "IRModule.h"
#include "GateModule.h"
#include "GcpMqttClient.h"
#include "CarDetectorModule.h"

int irPin = A0, irRedLEDPin = 21, irGreenLEDPin = 47;
int inGateServoPin = 39, inGateButtonPin = 48;
int outGateServoPin = 38, outGateButtonPin = 40;
int triggerPin = A1, echo1Pin = A2, echo2Pin = A3, detectedPin = A4;
int parkingLotId = 1;

IRModule irModule;
GateModule inGate, outGate;
GcpMqttClient gcpClient;
CarDetectorModule carDetector(triggerPin, echo1Pin, echo2Pin);

void setup(){
  Serial.begin(115200); 
  gcpClient.setup();
  irModule.setup(irPin, irRedLEDPin, irGreenLEDPin); 
  inGate.setup(inGateServoPin,inGateButtonPin, GATE_IN);
  outGate.setup(outGateServoPin,outGateButtonPin, GATE_OUT);
  carDetector.setup(detectedPin, parkingLotId);
  pinMode(12, OUTPUT);
}

void loop(){
  digitalWrite(12, LOW);
  gcpClient.loop();
  inGate.monitor(gcpClient, false);
  outGate.monitor(gcpClient, false);
  bool isNear = irModule.monitor(false);
  carDetector.monitor(gcpClient, isNear, false);

  delay(500);
}