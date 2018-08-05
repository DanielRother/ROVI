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

    // Get current brightness
    double brightness = led->m_rgb->toHSV()->v;
    Serial << "Used brightness: " << brightness << endl;

    // Check if thread is requested to stop ?
    while(stopRequested() == false) {
        for(double h = 0.0; h < 360.0; ++h) {
            if(stopRequested() == true) {
                break;
            }

            led->setColor(std::make_shared<HSVColor>(h, 1.0, brightness));
            delay(100);
        }

        Serial << "colorFlow finished" << endl;
    }
    Serial << "Task End" << endl;
}