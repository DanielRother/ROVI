#ifndef __LEDEFFECT_COLORFLOW__
#define __LEDEFFECT_COLORFLOW__

#include "../LEDEffect.hpp"

namespace Rovi {
    class ColorFlow : public LEDEffect {
    public:
        ColorFlow(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms), currentColor(0)
            {
                m_name = std::string{"color_flow"};
                Serial << "Colorflow - delay: " << delay_ms << endl;
            }

        void step() {
            auto brightness = getCurrentBrightness();
            led->setColor(std::make_shared<HSVColor>(currentColor, 1.0f, brightness));

            currentColor = (++currentColor) % 360u;

            Serial << "step() - brightness: " << brightness << " colorIdx: " << currentColor << endl;
        }

    protected:
        uint32_t currentColor;  
    };
}

#endif /* __LEDEFFECT_COLORFLOW__ */