#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <Devices/SimpleAbsoluteHue.h>

// RotaryEncoder
uint8_t rotaryPinA = 12;
uint8_t rotaryPinB = 13;
uint8_t rotaryPinButton = 14;

// LED
uint8_t  neoPixelPin    = 15;
uint16_t nbNeoPixelLEDs = 20;

std::shared_ptr<Rovi::Devices::SimpleAbsoluteHue> absolutHue;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
    std::cout << "--- setup() ---" << std::endl;

    absolutHue = std::make_shared<Rovi::Devices::SimpleAbsoluteHue>(
      rotaryPinA, rotaryPinB, rotaryPinButton,
      neoPixelPin, nbNeoPixelLEDs);

    // Setup LED
    auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
    for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
      swapRGValues[pixelIdx] = 1;
    }
    absolutHue->setSwapRGValues(swapRGValues);
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  absolutHue->update();
}

