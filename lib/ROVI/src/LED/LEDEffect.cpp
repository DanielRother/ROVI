#include "LEDEffect.hpp"

#include <Arduino.h>

#include <chrono>

#include <arduinoIostream.hpp>

// LEDEffect::LEDEffect(std::shared_ptr<LEDComponent> led)
LEDEffect::LEDEffect(LEDComponent* led, uint32_t delay_ms)
: led(led), delay_ms(delay_ms), lastUpdate_ms(millis()), isStopped(false) {}

void LEDEffect::update() {
    // Serial << "Dummy LEDEffect. Doesn't do anything" << endl;
    auto now_ms = millis();
    if(!isStopped && now_ms - lastUpdate_ms > delay_ms) {
        lastUpdate_ms = now_ms;
        step();
    }
}

float LEDEffect::getCurrentBrightness() {
    return led->lastColor->toHSV()->v;
}

void LEDEffect::stop() {
    isStopped = true;
}

void LEDEffect::start() {
    isStopped = false;
}