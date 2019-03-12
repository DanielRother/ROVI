#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>

SimpleAbsoluteHue::SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
: rotary{std::make_shared<RotaryEncoderWithButton>(pinA, pinB, pinButton)},
  leds{std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin)} {

      setupNormal();
      setupDoubleClick();
      setupHold();
};


void SimpleAbsoluteHue::setupNormal() {
    auto activatedCallback = [this]() {
        Serial << "NORMAL state activation callback" << endl;
        if(m_on) {
            blink();
        }
    };

    auto valueChangeCallback = [this](int value) {
        // TODO: Ab hier weiter
      uint8_t brightness = value * 10;

      Serial << "NORMAL value change callback - New value = " << value << endl;
      Serial << "  setBrightness to " << (brightness) << endl;
      leds->setBrightness(brightness);

      auto lastColor = leds->getLastColor();
      auto color = lastColor->toHSV();
      color->v = (float) brightness / 255.0f;
      leds->setColor(color);    // Required, because otherwise the color is not restored when changen from brightness 0 to 1...
                                // TODO: Check, if this is also required and/or working for the RGB_LEDs
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 255;
    const auto increment = 10;
    const auto preventOverflow = true;
    rotary->setupState(RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
}


void SimpleAbsoluteHue::setupDoubleClick() {

}


void SimpleAbsoluteHue::setupHold() {

}


void SimpleAbsoluteHue::blink() {
    leds->setPower(false);
    delay(250);
    leds->setPower(true);
}
