#define USE_MQTT

#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#ifndef USE_MQTT
	#include <Components/Input/RotaryEncoderWithButton.hpp>
#else
	#include <Components/Input/RotaryEncoderWithButtonMQTT.hpp>
#endif
#include <Components/Output/NeoPixelComponent.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Devices/AbsoluteHueMQTT.hpp>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

// RotaryEncoder
const uint8_t rotaryPinA = 12;
const uint8_t rotaryPinB = 13;
const uint8_t rotaryPinButton = 14;

// LED
const uint8_t  neoPixelPin    = 15;
const uint16_t nbNeoPixelLEDs = 20;

const std::string name = "AbsoluteHue";

#ifndef USE_MQTT
std::shared_ptr<Rovi::Devices::SimpleAbsoluteHue<
	Rovi::Components::RotaryEncoderWithButton, 
	Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>> absolutHue; 
#else
std::shared_ptr<Rovi::Devices::AbsoluteHueMQTT<
	Rovi::Components::RotaryEncoderWithButtonMQTT, 
	Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>> absolutHue;
#endif 

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
	sleep(5);
    std::cout << "--- setup() ---" << std::endl;
	iot.begin();

	auto leds = std::make_shared<Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>();
    auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
    // for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
    for(size_t pixelIdx = 12; pixelIdx < nbNeoPixelLEDs; ++pixelIdx) {
      swapRGValues[pixelIdx] = 1;
    }
    leds->setSwapRGValues(swapRGValues);
	auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
    effects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
    effects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
    effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
    effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow_slow", leds.get()));

#ifndef USE_MQTT
    auto rotary = std::make_shared<Rovi::Components::RotaryEncoderWithButton>(rotaryPinA, rotaryPinB, rotaryPinButton);

    absolutHue = std::make_shared<Rovi::Devices::SimpleAbsoluteHue<
        Rovi::Components::RotaryEncoderWithButton, 
        Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>>(
      iot, rotary, leds, effects, name);
#else
    auto rotary = std::make_shared<Rovi::Components::RotaryEncoderWithButtonMQTT>(rotaryPinA, rotaryPinB, rotaryPinButton);

    absolutHue = std::make_shared<Rovi::Devices::AbsoluteHueMQTT<
        Rovi::Components::RotaryEncoderWithButtonMQTT, 
        Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>>(
      iot, rotary, leds, effects, name);
#endif

	// Activate Over-the-Air updates
	String otaPassword = iot.configuration.get("OTAPassword"); // The prefedined variable OTAPassword is always reset to '1'; TODO: Check why/fix?
	std::cout << "*******************************************" << endl
			<< "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
			<< "*******************************************" << endl;

	ArduinoOTA.setPassword(otaPassword.c_str());
	ArduinoOTA.begin();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
	ArduinoOTA.handle();
	absolutHue->update();
}

