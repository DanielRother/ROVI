#ifndef __LEDEFFECT_WHITESTATIC__
#define __LEDEFFECT_WHITESTATIC__

#include "../LEDEffect.hpp"

class WhiteStatic : public LEDEffect {
public:
    WhiteStatic(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms)
        {}

    void run() {
        Serial << "WhiteStatic start" << endl;

        float brightness = getCurrentBrightness();
        led->setColor(std::make_shared<HSVColor>(0.0f, 0.0f, brightness));

        Serial << "WhiteStatic End" << endl;
    }

};


#endif /* __LEDEFFECT_WHITESTATIC__ */