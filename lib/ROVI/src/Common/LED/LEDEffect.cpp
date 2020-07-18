#include "LEDEffect.hpp"

#include <Arduino.h>

#include <chrono>

#include <arduinoIostream.hpp>

#include "Components/Output/LEDComponent.hpp"

namespace Rovi {
    // LEDEffect::LEDEffect(std::shared_ptr<Components::LEDComponent> led)
    LEDEffect::LEDEffect(Components::LEDComponent* led, uint32_t delay_ms)
    : led(led), delay_ms(delay_ms), lastUpdate_ms(millis()), isStopped(false) {}

    void LEDEffect::update() {
        // Serial << "Dummy LEDEffect. Doesn't do anything" << endl;
        auto now_ms = millis();
        if(!isStopped && now_ms - lastUpdate_ms > delay_ms) {
            lastUpdate_ms = now_ms;
            step();
        }
    }

    std::shared_ptr<Color> LEDEffect::getCurrentColor() {
        return led->color();
    }

    float LEDEffect::getCurrentBrightness() {
        return led->color()->toHSV()->v;
    }

    void LEDEffect::stop() {
        isStopped = true;
    }

    void LEDEffect::start() {
        isStopped = false;
    }
}