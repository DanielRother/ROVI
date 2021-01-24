#include <Arduino.h>
#include <WiFi.h>

#include <ArduinoIostream.hpp>

#include <iostream>
#include <string>

#include <Config/RoviWiFiManager.hpp>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  auto rwm = Rovi::Config::RoviWiFiManager();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    std::cerr << "WiFi isn't connected anymore. Restart ESP" << std::endl;
    delay(2000);
    ESP.restart();
  } // put your main code here, to run repeatedly:
}