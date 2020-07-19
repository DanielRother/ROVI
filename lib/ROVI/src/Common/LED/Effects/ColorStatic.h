#ifndef __LEDEFFECT_COLORSTATIC__
#define __LEDEFFECT_COLORSTATIC__

#include "../LEDEffect.hpp"

namespace Rovi {
    class ColorStatic : public LEDEffect {
    public:
        ColorStatic(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms)
            , done(false)
            {
                m_name = std::string{"color_static"};
            }

        void step() {
            // The initialization is done here and not within the ctor because otherwise the application
            // may crash when setting the effect within an device ctor (e.g. SimpleAbsoluteHue).
            // Probably, there is an initialization order and/or null pointer issue
            // TODO: Fix me
            if(!done) {
                auto hue = 0.0f;
                auto brightness = 0.5f;
                const auto saturation = 1.0f; 
                if(led) {
                    auto color = led->color()->toHSV();
                    hue = color->h;
                    brightness = color->v;
                }
                led->setColor(std::make_shared<HSVColor>(hue, saturation, brightness));

                done = true;
            }
        }

        bool done;
    };
}

#endif /* __LEDEFFECT_COLORSTATIC__ */