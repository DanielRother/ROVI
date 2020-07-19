#ifndef __LEDEFFECT_RAINBOW__
#define __LEDEFFECT_RAINBOW__

#include "../LEDEffect.hpp"

namespace Rovi {
    class Rainbow : public LEDEffect {
    public:
        Rainbow(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms), currentStartColor(0)
            {
                m_name = std::string{"rainbow"};
                Serial << "Rainbow - delay: " << delay_ms << endl;
            }

        void step() {
            Serial << "Rainbow - step; currentStartColor = " << currentStartColor << endl;

            if(!led->isAdressable()) {
                Serial << "Effect not available for this kind of led" << endl;
                return;
            }

            auto brightness = getCurrentBrightness();
            auto nbPixel = led->nbPixel();
            auto hueIncrement = 360.0f / (float) nbPixel;

            for(size_t pixelIdx = 0; pixelIdx < nbPixel; ++pixelIdx) {
                auto curHue = (int32_t) ((float) currentStartColor + pixelIdx * hueIncrement ) % 360;
                led->setColor(std::make_shared<HSVColor>(curHue, 1.0f, brightness), pixelIdx);
            }
            led->show();

            currentStartColor = (++currentStartColor) % 360u;

            Serial << "step() - brightness: " << brightness << " colorIdx: " << currentStartColor << endl;
        }

    protected:
        uint32_t currentStartColor;  
    };
}

#endif /* __LEDEFFECT_RAINBOW__ */