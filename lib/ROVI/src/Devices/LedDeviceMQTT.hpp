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

  virtual void setOn(bool on) override {
    SimpleLedDevice<LED>::setOn(on);
    this->distributeSettings();
  }

  virtual void setBrightness(uint8_t brightness) override {
    SimpleLedDevice<LED>::setBrightness(brightness);
    this->distributeSettings();
  }

  virtual void setColor(const std::shared_ptr<Color> &color) override {
    SimpleLedDevice<LED>::setColor(color);
    distributeSettings();
  }

  virtual void setColor2(const std::shared_ptr<Color> &color) override {
    SimpleLedDevice<LED>::setColor2(color);
    distributeSettings();
  }

  virtual void setHue(uint32_t hue) override {
    SimpleLedDevice<LED>::setHue(hue);
    distributeSettings();
  }

  virtual void setEffect(const std::shared_ptr<LEDEffect> &effect) override {
    SimpleLedDevice<LED>::setEffect(effect);
    distributeSettings();
  }

  virtual void setEffect(int effect) override {
    SimpleLedDevice<LED>::setEffect(effect);
    distributeSettings();
  }

  virtual void
  setTimePerEffect(const std::chrono::minutes &timePerEffect) override {
    SimpleLedDevice<LED>::setTimePerEffect(timePerEffect);
    distributeSettings();
  }

  virtual void update() override {
    SimpleLedDevice<LED>::update();
    MqttDevice::update();
  }

protected:
  virtual std::string createMqttMessage() override {
    Serial << "LedDeviceMQTT::createMqttMessage() " << endl;

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
    Serial << "LedDeviceMQTT::receiveMqttMessage() " << endl;

    // DynamicJsonBuffer jsonBuffer;
    // JsonObject &json = jsonBuffer.parseObject(payload);
    json.printTo(Serial);
    std::cout << "Currently not restored as this requires are more robust "
                 "implementation"
              << std::endl;
    // See below for such an implementation
    // restoreSettingsImpl(json);
    // Maybe distributeSettings() afterwards?

    // const int capacity = 3000; // JSON_OBJECT_SIZE(10);
    // StaticJsonBuffer<capacity> jb;
    // JsonObject &obj = jb.parseObject(payload);
    // if (obj.success()) {
    //   std::cout << "parseObject() succeeded" << std::endl;
    // } else {
    //   std::cout << "parseObject() failed" << std::endl;
    //   return;
    // }

    // // Check possible inputs
    // if (obj.containsKey("brightness")) {
    //   auto brightness = Utils::clamp(obj["brightness"].as<int>(), 0, 100);
    //   std::cout << "Set brightness to " << brightness << endl;
    //   setBrightness(brightness);
    // }
    // if (obj.containsKey("colorType") && obj.containsKey("color")) {
    //   auto colorType = std::string{obj["colorType"].as<char *>()};
    //   std::cout << "Set color for type " << colorType << std::endl;
    //   if (colorType == "rgb") {
    //     auto color = obj["color"];
    //     auto r = Utils::clamp(color["r"].as<int>(), 0, 255);
    //     auto g = Utils::clamp(color["g"].as<int>(), 0, 255);
    //     auto b = Utils::clamp(color["b"].as<int>(), 0, 255);
    //     auto newColor = std::make_shared<RGBColor>(r, g, b);
    //     std::cout << "    r: " << r << ", g: " << g << ", b: " << b
    //               << std::endl;
    //     std::cout << "    new color received via MQTT is "
    //               << newColor->toString();
    //     setColor(newColor);
    //   } else if (colorType == "hsv") {
    //     auto color = obj["color"];
    //     auto h = Utils::clamp(color["h"].as<int>(), 0, 359);
    //     auto s = Utils::clamp(color["s"].as<float>(), 0.0f, 1.0f);
    //     auto v = Utils::clamp(color["v"].as<float>(), 0.0f, 1.0f);
    //     auto newColor = std::make_shared<HSVColor>(h, s, v);
    //     setColor(newColor);
    //   } else {
    //     std::cout << "Error: Unknown colortype" << std::endl;
    //   }
    // }
    // if (obj.containsKey("colorType") && obj.containsKey("color2")) {
    //   auto colorType = std::string{obj["colorType"].as<char *>()};
    //   std::cout << "Set color2 for type " << colorType << std::endl;
    //   if (colorType == "rgb") {
    //     auto color = obj["color2"];
    //     auto r = Utils::clamp(color["r"].as<int>(), 0, 255);
    //     auto g = Utils::clamp(color["g"].as<int>(), 0, 255);
    //     auto b = Utils::clamp(color["b"].as<int>(), 0, 255);
    //     auto newColor = std::make_shared<RGBColor>(r, g, b);
    //     std::cout << "    r: " << r << ", g: " << g << ", b: " << b
    //               << std::endl;
    //     std::cout << "    new color received via MQTT is "
    //               << newColor->toString();
    //     setColor2(newColor);
    //   } else if (colorType == "hsv") {
    //     auto color = obj["color2"];
    //     auto h = Utils::clamp(color["h"].as<int>(), 0, 359);
    //     auto s = Utils::clamp(color["s"].as<float>(), 0.0f, 1.0f);
    //     auto v = Utils::clamp(color["v"].as<float>(), 0.0f, 1.0f);
    //     auto newColor = std::make_shared<HSVColor>(h, s, v);
    //     setColor2(newColor);
    //   } else {
    //     std::cout << "Error: Unknown colortype" << std::endl;
    //   }
    // }
    // if (obj.containsKey("effect")) {
    //   auto selectedEffect = std::string{obj["effect"].as<char *>()};
    //   std::cout << "Set effect to " << selectedEffect << endl;
    //   for (auto effect : this->m_effects) {
    //     if (effect->name() == selectedEffect) {
    //       setEffect(effect);
    //       break;
    //     }
    //   }
    // }
    // if (obj.containsKey("timePerEffect_m")) {
    //   auto timePerEffect = obj["timePerEffect_m"].as<int>();
    //   std::cout << "Set timePerEffect to " << timePerEffect << endl;
    //   setTimePerEffect(std::chrono::minutes{timePerEffect});
    // }
    // // Check power last as power == false should always turn the bulb off
    // if (obj.containsKey("power")) {
    //   auto power = obj["power"].as<bool>();
    //   std::cout << "Set power to " << power << endl;
    //   setOn(power);
    // }
  }
};

}; // namespace Devices
}; // namespace Rovi

#endif /* __LED_DEVICE_MQTT_H__ */