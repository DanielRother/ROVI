#include <Arduino.h>

#include <ArduinoIostream.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <Config/RoviWiFiManager.hpp>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  auto rwm = Rovi::Config::RoviWiFiManager();
}

void loop() {
  // put your main code here, to run repeatedly:
}