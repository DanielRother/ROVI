#ifndef __LEDEFFECT_COLORFLOW__
#define __LEDEFFECT_COLORFLOW__

#include "../LEDEffect.hpp"

class ColorFlow : public LEDEffect {
public:
    ColorFlow(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms), currentColor(0)
        {}

    void step() {
        auto brightness = getCurrentBrightness();
        led->setColor(std::make_shared<HSVColor>(currentColor, 1.0f, brightness));

        currentColor = (++currentColor) % 360;
    }

protected:
    uint32_t currentColor;  
};


#endif /* __LEDEFFECT_COLORFLOW__ */