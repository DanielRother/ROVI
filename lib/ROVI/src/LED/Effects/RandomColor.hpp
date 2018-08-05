#ifndef __LEDEFFECT_RANDOMCOLOR__
#define __LEDEFFECT_RANDOMCOLOR__

#include "../LEDEffect.hpp"

#include <chrono>
#include <random>

class RandomColor : public LEDEffect {
public:
    RandomColor(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms), generator(std::chrono::system_clock::now().time_since_epoch().count()), distribution(0,360)
        {}

    void run() {
        // TODO: Move to subclass
        Serial << "RandomColor start" << endl;

        // Get current brightness
        float brightness = getCurrentBrightness();
        Serial << "brightness: " << brightness << endl;

        // Check if thread is requested to stop ?
        while(stopRequested() == false) {
            int color = distribution(generator);
            Serial << "Color: " << color << endl;

            // Fade in
            Serial << "Fade in" << endl;
            for(float saturation = 0.0f; saturation < 0.5f && stopRequested() == false; saturation += 0.01f) {
                led->setColor(std::make_shared<HSVColor>(color, saturation, brightness));
                delay(delay_ms);
            }
            for(float saturation = 0.5f; saturation < 1.0f && stopRequested() == false; saturation += 0.01f) {
                led->setColor(std::make_shared<HSVColor>(color, saturation, brightness));
                delay(delay_ms*2);
            }

            // Fade out
            Serial << "Fade out" << endl;
            for(float saturation = 1.0f; saturation > 0.5f && stopRequested() == false; saturation -= 0.01f) {
                led->setColor(std::make_shared<HSVColor>(color, saturation, brightness));
                delay(delay_ms*2);
            }
            for(float saturation = 0.5f; saturation > 0.0f && stopRequested() == false; saturation -= 0.01f) {
                led->setColor(std::make_shared<HSVColor>(color, saturation, brightness));
                delay(delay_ms);
            }        
        }
        Serial << "Task End" << endl;
    }

protected:
    std::default_random_engine          generator;
    std::uniform_int_distribution<int>  distribution;
};


#endif /* __LEDEFFECT_RANDOMCOLOR__ */