

#include <ArduinoIostream.hpp>

#include <functional>
#include <iostream>
#include <string>

#include <Common/MqttAdapter.hpp>
#include <Common/MqttConnection.hpp>
#include <Common/SPIFFSSettingsInterface.hpp>
#include <Common/WifiEventHandler.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Config/RoviWiFiManager.hpp>
#include <Devices/LedDevice.hpp>

const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

std::shared_ptr<
    Rovi::Devices::LedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>>
    xmastree;
Rovi::Common::MqttConnection mqttConnection;
std::shared_ptr<Rovi::Common::MqttAdapter> mqttAdapter;

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(wiFiEventHandler);

  auto rwm = Rovi::Config::RoviWiFiManager();

  auto leds =
      std::make_shared<Rovi::Components::FastLedComponent<pin, nbPixel>>();
  auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
  xmastree = std::make_shared<Rovi::Devices::LedDevice<
      Rovi::Components::FastLedComponent<pin, nbPixel>>>(leds, effects, name);

  mqttAdapter = std::make_shared<Rovi::Common::MqttAdapter>(
      *std::static_pointer_cast<Rovi::Devices::BasicDevice>(xmastree),
      mqttConnection, rwm);
  mqttConnection.start(rwm);
}

auto start = millis();

void loop() {
  // xmastree->update();    // Only required, if no MQTT adapter is used.
  mqttAdapter->update();
}
