#ifndef __LED_DEVICE_MQTT_H__
#define __LED_DEVICE_MQTT_H__

#include <Arduino.h>

#include <ArduinoJson.h>
#include <Common/MqttConnection.hpp>
#include <Config/RoviWiFiManager.hpp>

#include <iostream>
#include <sstream>

#include "MqttDevice.hpp"
#include "SimpleLedDevice.hpp"
#include "UtilFunctions.hpp"

namespace Rovi {
namespace Devices {
template <class LED>
class LedDeviceMQTT : public MqttDevice, public SimpleLedDevice<LED> {
public:
  LedDeviceMQTT(Common::MqttConnection &mqtt, Config::RoviWiFiManager &rwm,
                const std::shared_ptr<LED> led,
                const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects,
                const std::string &name = "led")
      : BasicDevice{}, MqttDevice{mqtt, rwm}, SimpleLedDevice<LED>{led, effects,
                                                                   name} {}

  virtual void update() override {
    SimpleLedDevice<LED>::update();
    MqttDevice::update();
  }

protected:
  virtual std::string createMqttMessage() override {
    std::cout << "LedDeviceMQTT::createMqttMessage() " << std::endl;

    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    SimpleLedDevice<LED>::saveSettingsImpl(json, jsonBuffer);
    // Altough documented other, the library doesn't support std::string... :(
    // auto output = std::string{};
    // std::string output;
    String output;
    json.prettyPrintTo(output); // TODO
    return output.c_str();
  }

  void receiveMqttMessage(const std::string &payload) override {
    std::cout << "LedDeviceMQTT::receiveMqttMessage() " << std::endl;

    DynamicJsonBuffer jsonBuffer;
    String convPayload = payload.c_str();
    JsonObject &json = jsonBuffer.parseObject(convPayload);
    json.prettyPrintTo(Serial);
    SimpleLedDevice<LED>::restoreSettingsImpl(json);
    distributeSettings();

    // distributeSettings() not required, as it is already triggered by the
    // value changes
  }

  virtual std::string getType() override { return this->m_name; }
  virtual void getOptions(JsonObject &options,
                          DynamicJsonBuffer &jsonBuffer) override {
    JsonArray &possibleEffects = jsonBuffer.createArray();
    for (auto e : this->m_effects) {
      String effectS = e->name().c_str();
      possibleEffects.add(effectS);
    }
    options["possibleEffects"] = possibleEffects;
  }
};

}; // namespace Devices
}; // namespace Rovi

#endif /* __LED_DEVICE_MQTT_H__ */