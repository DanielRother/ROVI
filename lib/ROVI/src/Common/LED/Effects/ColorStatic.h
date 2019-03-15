#ifndef __LEDEFFECT_COLORSTATIC__
#define __LEDEFFECT_COLORSTATIC__

#include "../LEDEffect.hpp"

namespace Rovi {
    class ColorStatic : public LEDEffect {
    public:
        ColorStatic(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms)
            {}

        void step() {
            auto brightness = getCurrentBrightness();
            led->setColor(std::make_shared<HSVColor>(0.0f, 1.0f, brightness));
        }
    };
}

#endif /* __LEDEFFECT_COLORSTATIC__ */