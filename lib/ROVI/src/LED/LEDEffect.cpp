#include "LEDEffect.hpp"

#include <Arduino.h>

#include <chrono>

#include <arduinoIostream.hpp>

// LEDEffect::LEDEffect(std::shared_ptr<LEDComponent> led)
// : led(led) {}

void LEDEffect::run() {
    Serial << "LED Effect Start" << endl;

    // Check if thread is requested to stop ?
    while (stopRequested() == false)
    {
        Serial << "Doing Some Work" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    Serial << "Task End" << endl;
}