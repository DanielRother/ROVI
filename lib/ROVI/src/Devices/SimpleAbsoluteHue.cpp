#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>

#include "../LED/LEDEffectFactory.hpp"

SimpleAbsoluteHue::SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
: rotary{std::make_shared<RotaryEncoderWithButton>(pinA, pinB, pinButton)},
  leds{std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin)} {

      setupNormal();
      setupClick();
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

    auto valueChangeCallback = [this](int brightness) {
        Serial << "NORMAL value change callback - New value = " << brightness << endl;
        leds->setBrightness(brightness);

        // TEST: Still required?
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


void SimpleAbsoluteHue::setupClick() {
    auto activatedCallback = [this]() {
        Serial << "CLICKED state activation callback" << endl;

        m_on = !m_on;
        leds->setPower(m_on);
        if(m_on) {
            leds->startEffect();
        } else {
            leds->stopEffect();
        }
    };

    auto valueChangeCallback = [&](int value) {
        Serial << "CLICKED value change callback - Do nothing" << endl;
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 10;
    const auto increment = 1;
    const auto preventOverflow = false;
    rotary->setupState(RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
}


void SimpleAbsoluteHue::setupDoubleClick() {
    auto activatedCallback = [this]() {
        Serial << "DOUBLE_CLICKED state activation callback" << endl;

        if(m_on) {
            leds->stopEffect();
            doubleBlink();
        }
    };

    auto valueChangeCallback = [&](int hue) {
        Serial << "DOUBLE_CLICKED value change callback - New value = " << hue << endl;

        leds->setColor(std::make_shared<HSVColor>(hue, 1.0f, 1.0f));   // TODO: getCurrentBrightness()?
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 360;
    const auto increment = 10;
    const auto preventOverflow = false;
    rotary->setupState(RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
}


void SimpleAbsoluteHue::setupHold() {
    auto activatedCallback = [this]() {
        Serial << "HOLDED state activation callback" << endl;
        blink(500);
    };

    auto valueChangeCallback = [&](int value) {
        Serial << "HOLDED value change callback - New value = " << value << endl;
        
        auto selectedEffect = LEDEffectFactory::getEffect(value, leds.get());
        leds->setEffect(selectedEffect);
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = LEDEffectFactory::getNumberOfEffects() - 1;;
    const auto increment = 1;
    const auto preventOverflow = false;
    rotary->setupState(RotaryEncoderWithButton::ButtonStates::HOLDED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
}


void SimpleAbsoluteHue::blink(const uint32_t delay_ms) {
    leds->setPower(false);
    delay(delay_ms);
    leds->setPower(true);
}

void SimpleAbsoluteHue::doubleBlink(const uint32_t delay_ms) {
    blink(delay_ms);
    delay(delay_ms);
    blink(delay_ms);
}
