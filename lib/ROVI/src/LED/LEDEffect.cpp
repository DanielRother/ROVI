#include "LEDEffect.hpp"

#include <Arduino.h>

#include <chrono>

#include <arduinoIostream.hpp>

// LEDEffect::LEDEffect(std::shared_ptr<LEDComponent> led)
LEDEffect::LEDEffect(LEDComponent* led, uint32_t delay_ms)
: led(led), delay_ms(delay_ms) {}

void LEDEffect::run() {
    Serial << "Dummy LEDEffect. Doesn't do anything" << endl;
}

float LEDEffect::getCurrentBrightness() {
    return led->lastColor->toHSV()->v;
}