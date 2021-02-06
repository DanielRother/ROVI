#ifndef __WIFI_EVENT_HANDLER_H__
#define __WIFI_EVENT_HANDLER_H__

#include <Arduino.h>
#include <ArduinoIostream.hpp>
#include <WiFi.h>

#include <iostream>

void wiFiEventHandler(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_CONNECTED:
    std::cout << "WiFi connected" << std::endl;
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    std::cerr << "WiFi lost connection -> Restart ESP" << std::endl;
    delay(2000);
    ESP.restart();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    std::cout << "Got IP address: " << WiFi.localIP().toString().c_str()
              << std::endl;
    break;
  }
}

#endif /* __WIFI_EVENT_HANDLER_H__ */