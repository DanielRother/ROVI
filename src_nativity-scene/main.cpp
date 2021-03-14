#define USE_MQTT

#include <Arduino.h>
#include <ArduinoOTA.h>

#include <Common/LED/ColorTypes.h>
#include <Common/MqttAdapter.hpp>
#include <Common/MqttConnection.hpp>
#include <Common/WifiEventHandler.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
#include <Config/RoviWiFiManager.hpp>
#include <Devices/AbsolutHue.hpp>

const auto nbPixel = 27;
const auto pin = 13;
auto name = "krippenbild";
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
  //   auto leds = std::make_shared<Rovi::Components::NeoPixelComponent>(pin,
  //   nbPixel);

  auto swapRGValues = std::vector<uint32_t>(nbPixel, 0);
  for (size_t pixelIdx = 0; pixelIdx < nbPixel; ++pixelIdx) {
    swapRGValues[pixelIdx] = 1;
  }
  leds->setSwapRGValues(swapRGValues);

  auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_gradient", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("rainbow", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_circle_rgb", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_circle_red", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_circle_blue", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_circle_green", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_circle_purple", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("running_onoff_single", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("running_onoff_multi", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("running_onoff_twocolor", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("running_onoff_rainbow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect(
      "running_onoff_single_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect(
      "running_onoff_multi_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect(
      "running_onoff_twocolor_zufall", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect(
      "running_onoff_rainbow_random", leds.get()));

  // Deactivae automatic effect change
  xmastree->setTimePerEffect(std::chrono::minutes{0});

  xmastree = std::make_shared<Rovi::Devices::LedDevice<
      Rovi::Components::FastLedComponent<pin, nbPixel>>>(leds, effects, name);

  mqttAdapter = std::make_shared<Rovi::Common::MqttAdapter>(
      *std::static_pointer_cast<Rovi::Devices::BasicDevice>(xmastree),
      mqttConnection, rwm);
  mqttConnection.start(rwm);

  ArduinoOTA.setPassword(rwm.otaPassword.c_str());
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  // xmastree->update();    // Only required, if no MQTT adapter is used.
  mqttAdapter->update();
}