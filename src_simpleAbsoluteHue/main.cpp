#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <Components/Input/RotaryEncoderWithButton.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Devices/SimpleAbsoluteHue.h>

// RotaryEncoder
const uint8_t rotaryPinA = 12;
const uint8_t rotaryPinB = 13;
const uint8_t rotaryPinButton = 14;

// LED
const uint8_t  neoPixelPin    = 15;
const uint16_t nbNeoPixelLEDs = 20;

// std::shared_ptr<Rovi::Devices::SimpleAbsoluteHue> absolutHue;
std::shared_ptr<Rovi::Devices::SimpleAbsoluteHue<
	Rovi::Components::RotaryEncoderWithButton, 
	Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>> absolutHue; 

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
	sleep(5);
    std::cout << "--- setup() ---" << std::endl;

    auto rotary = std::make_shared<Rovi::Components::RotaryEncoderWithButton>(rotaryPinA, rotaryPinB, rotaryPinButton, "rotary");
    
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

    absolutHue = std::make_shared<Rovi::Devices::SimpleAbsoluteHue<
        Rovi::Components::RotaryEncoderWithButton, 
        Rovi::Components::FastLedComponent<neoPixelPin, nbNeoPixelLEDs>>>(
      rotary, leds, effects, "AbsoluteHue");

}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  absolutHue->update();
}

