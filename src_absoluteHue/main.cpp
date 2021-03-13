#include <Arduino.h>
#include <ArduinoOTA.h>

#include <stdint.h>
#include <string>

#include <ArduinoIostream.hpp>
#include <prettyprint.hpp>

#include <Common/MqttAdapter.hpp>
#include <Common/MqttConnection.hpp>
#include <Common/WifiEventHandler.hpp>
#include <Components/Input/RotaryEncoderWithButton.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
#include <Config/RoviWiFiManager.hpp>
#include <Devices/AbsolutHue.hpp>

// RotaryEncoder
const uint8_t rotaryPinA = 12;
const uint8_t rotaryPinB = 13;
const uint8_t rotaryPinButton = 14;

// LED
const uint8_t neoPixelPin = 15;
const uint16_t nbNeoPixelLEDs = 20;

const std::string name = "AbsoluteHue";

std::shared_ptr<Rovi::Devices::AbsolutHue<
    Rovi::Components::RotaryEncoderWithButton,
    Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>>
    absolutHue;
Rovi::Common::MqttConnection mqttConnection;
std::shared_ptr<Rovi::Common::MqttAdapter> mqttAdapter;

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial.begin(115200);
  WiFi.onEvent(wiFiEventHandler);

  auto rwm = Rovi::Config::RoviWiFiManager();

  auto leds = std::make_shared<
      Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>();
  auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
  // for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
  for (size_t pixelIdx = 12; pixelIdx < nbNeoPixelLEDs; ++pixelIdx) {
    swapRGValues[pixelIdx] = 1;
  }
  leds->setSwapRGValues(swapRGValues);
  auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
  effects.push_back(
      Rovi::LEDEffectFactory::getEffect("color_flow_slow", leds.get()));

  auto rotary = std::make_shared<Rovi::Components::RotaryEncoderWithButton>(
      rotaryPinA, rotaryPinB, rotaryPinButton);

  absolutHue = std::make_shared<Rovi::Devices::AbsolutHue<
      Rovi::Components::RotaryEncoderWithButton,
      Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>>(
      rotary, leds, effects, name);
  mqttAdapter = std::make_shared<Rovi::Common::MqttAdapter>(
      *std::static_pointer_cast<Rovi::Devices::BasicDevice>(absolutHue),
      mqttConnection, rwm);

  mqttConnection.start(rwm);

  ArduinoOTA.setPassword(rwm.otaPassword.c_str());
  ArduinoOTA.begin();
}

//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  ArduinoOTA.handle();
  // absolutHue->update();    // Only required, if no MQTT adapter is used.
  mqttAdapter->update();
}
