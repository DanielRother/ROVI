#ifndef __LEDEFFECT_RAINBOW__
#define __LEDEFFECT_RAINBOW__

#include "../LEDEffect.hpp"

namespace Rovi {
    class Rainbow : public LEDEffect {
    public:
        Rainbow(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms), currentStartColor(0)
            {
                Serial << "Rainbow - delay: " << delay_ms << endl;
            }

        void step() {
            // if(!adressableLed) {
            //     Serial << "Effect not available for this kind of led" << endl;
            // }

            auto brightness = getCurrentBrightness();
            // auto nbPixel = adressableLed->asdsd

            led->setColor(std::make_shared<HSVColor>(currentStartColor, 1.0f, brightness));

            currentStartColor = (++currentStartColor) % 360u;

            Serial << "step() - brightness: " << brightness << " colorIdx: " << currentStartColor << endl;
        }

    protected:
        uint32_t currentStartColor;  
    };
}

#endif /* __LEDEFFECT_RAINBOW__ */