

#ifndef __ROVIDEVICE_H__
#define __ROVIDEVICE_H__

#include <Arduino.h>

#include <map>
#include <memory>

#include <Basecamp.hpp>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"

namespace Rovi {
  class RoviDevice : public std::enable_shared_from_this<RoviDevice>{
  public:
    enum class MQTTQoSClasses {
      AT_MOST_ONE   = 0,
      AT_LEAST_ONE  = 1,
      EXACTLY_ONE   = 2
    };

    RoviDevice(Basecamp& iot);
    RoviDevice(const RoviDevice& other) = default;

    void setupRovi();
    void addComponent(std::shared_ptr<RoviComponent> component);

    //****************************************//
    // MQTT interface methods
    //****************************************//
    void mqttConnected(bool sessionPresent);
    void mqttSubscribed(uint16_t packetId, uint8_t qos);
    void mqttPublished(uint16_t packetId);
    void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

    std::string getBaseTopic() const;

  // private:
    std::string addVariableToIotConfig(const std::string&, const std::string& defaultValue);
    std::string removeBaseTopic(const std::string& mqttTopic);
    std::string getResponsibleComponent(const std::string& mqttTopic);

    Basecamp& iot;
    std::string name;
    std::string room;

    std::string baseTopic;
    std::string statusTopic;

    std::map<std::string, std::shared_ptr<RoviComponent>> components;
  };
}

#endif /* __ROVIDEVICE_H__ */