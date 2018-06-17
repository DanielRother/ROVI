#ifndef __ROVIDEVICE_H__
#define __ROVIDEVICE_H__

#include <Arduino.h>

#include <Basecamp.hpp>

#include <arduinoIostream.hpp>

class RoviDevice {
public:
  RoviDevice(Basecamp& iot);

  void setupRovi();

  //****************************************//
  // MQTT interface methods
  //****************************************//
  void mqttConnected(bool sessionPresent);
  void mqttSubscribed(uint16_t packetId, uint8_t qos);
  void mqttPublished(uint16_t packetId);
  void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

// private:
  String addVariableToIotConfig(String name, String defaultValue);

  Basecamp& iot;
  String name;
  String room;

  String baseTopic;
  String statusTopic;
};

#endif /* __ROVIDEVICE_H__ */