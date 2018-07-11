#include "LEDEffect.hpp"

#include <Arduino.h>

#include <chrono>

#include <arduinoIostream.hpp>

// LEDEffect::LEDEffect(std::shared_ptr<LEDComponent> led)
LEDEffect::LEDEffect(LEDComponent* led)
: led(led) {}

void LEDEffect::run() {
    // TODO: Move to subclass
    Serial << "LED Effect Start" << endl;

    // Check if thread is requested to stop ?
    // while (stopRequested() == false)
    // {
        Serial << "Doing Some Work" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        for(double h = 0.0; h < 360.0, stopRequested() == false; ++h) {
            // if(stopRequested() == true) {
            //     break;
            // }

            led->setColor(std::make_shared<HSVColor>(h, 1.0, 0.5));
            delay(100);
        }

        Serial << "colorFlow finished" << endl;
    // }
    Serial << "Task End" << endl;
}