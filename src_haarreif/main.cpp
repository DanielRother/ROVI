#include <Arduino.h>
#include <ArduinoOTA.h>

#include <Common/MqttAdapter.hpp>
#include <Common/MqttConnection.hpp>
#include <Common/WifiEventHandler.hpp>
#include <Components/Input/RotaryEncoderWithButton.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
#include <Config/RoviWiFiManager.hpp>
#include <Devices/LedDevice.hpp>

const auto nbPixel = 20;
const auto pinLed = 13;
auto name = "haarreif";
auto colorCircleDelay = 500;

const auto pinButton = 15;

void changeBrightness();

std::shared_ptr<Rovi::Devices::LedDevice<
    Rovi::Components::FastLedComponent<pinLed, nbPixel>>>
    xmastree;
std::shared_ptr<Rovi::Components::OneButton> brightnessButton;

Rovi::Common::MqttConnection mqttConnection;
std::shared_ptr<Rovi::Common::MqttAdapter> mqttAdapter;

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(wiFiEventHandler);

  //   auto rwm = Rovi::Config::RoviWiFiManager();

  auto leds =
      std::make_shared<Rovi::Components::FastLedComponent<pinLed, nbPixel>>();
  //   auto leds = std::make_shared<Rovi::Components::NeoPixelComponent>(pin,
  //   nbPixel);

  auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
  //   effects.push_back(
  //       Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
  // effects.push_back(
  //     Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
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

  xmastree = std::make_shared<Rovi::Devices::LedDevice<
      Rovi::Components::FastLedComponent<pinLed, nbPixel>>>(leds, effects,
                                                            name);

  xmastree->setTimePerEffect(std::chrono::minutes(1));
  xmastree->setBrightness(50);

  brightnessButton =
      std::make_shared<Rovi::Components::OneButton>(pinButton, true, true);
  brightnessButton->attachClick(changeBrightness);

  //   mqttAdapter = std::make_shared<Rovi::Common::MqttAdapter>(
  //       *std::static_pointer_cast<Rovi::Devices::BasicDevice>(xmastree),
  //       mqttConnection, rwm);
  //   mqttConnection.start(rwm);

  //   ArduinoOTA.setPassword(rwm.otaPassword.c_str());
  //   ArduinoOTA.begin();
}

void loop() {
  //   ArduinoOTA.handle();
  xmastree->update(); // Only required, if no MQTT adapter is used.
  //   mqttAdapter->update();
  brightnessButton->tick();
}

auto brightnessValues = std::vector<int>{10, 25, 50, 75, 100};
auto curBrightnessIdx = 0;
void changeBrightness() {
  std::cout << "changeBrightness()" << std::endl;
  auto nextBrightness =
      brightnessValues.at(++curBrightnessIdx % brightnessValues.size());
  xmastree->setBrightness(nextBrightness);
}