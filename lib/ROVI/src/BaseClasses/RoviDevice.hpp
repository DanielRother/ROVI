#ifndef __ROVIDEVICE_H__
#define __ROVIDEVICE_H__

#include <Arduino.h>

#include <Basecamp.hpp>

#include <ArduinoIostream.hpp>

class RoviDevice {
  enum class MQTTQoSClasses {
    AT_MOST_ONE   = 0,
    AT_LEAST_ONE  = 1,
    EXACTLY_ONE   = 2
  };

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
  std::string addVariableToIotConfig(std::string name, std::string defaultValue);

  Basecamp& iot;
  std::string name;
  std::string room;

  std::string baseTopic;
  std::string statusTopic;
};

#endif /* __ROVIDEVICE_H__ */