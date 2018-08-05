#ifndef __LEDEFFECT_COLORFLOW__
#define __LEDEFFECT_COLORFLOW__

#include "../LEDEffect.hpp"

class ColorFlow : public LEDEffect {
public:
    ColorFlow(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms)
        {}

    void run() {
        // TODO: Move to subclass
        Serial << "ColorFlow start" << endl;

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
                delay(delay_ms);
            }

            Serial << "ColorFlow finished" << endl;
        }
        Serial << "Task End" << endl;
    }

};


#endif /* __LEDEFFECT_COLORFLOW__ */