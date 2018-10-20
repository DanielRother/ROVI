#ifndef __LEDEFFECT_COLORSTATIC__
#define __LEDEFFECT_COLORSTATIC__

#include "../LEDEffect.hpp"

class ColorStatic : public LEDEffect {
public:
    ColorStatic(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms)
        {}

    void run() {
        Serial << "ColorStatic start" << endl;

        float brightness = getCurrentBrightness();
        led->setColor(std::make_shared<HSVColor>(0.0f, 1.0f, brightness));

        Serial << "ColorStatic End" << endl;
    }

};


#endif /* __LEDEFFECT_COLORSTATIC__ */