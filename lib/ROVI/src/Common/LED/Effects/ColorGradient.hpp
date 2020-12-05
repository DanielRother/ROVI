#ifndef __LEDEFFECT_COLOR_GRADIENT__
#define __LEDEFFECT_COLOR_GRADIENT__

#include "../LEDEffect.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

namespace Rovi {
    class ColorGradient : public LEDEffect {
    public:
        ColorGradient(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms)
            , done(false)
            {
                m_name = std::string{"ColorGradiaent"};
            }

        void step() {

            if(!done) {
                if(!led->isAdressable()) {
                    std::cout << "Effect not available for this kind of led" << std::endl;
                    return;
                }

                auto nbPixel = led->nbPixel();
                auto color = led->color1()->toHSV();
                auto color2 = led->color2()->toHSV();
                // std::cout << "color = " << color->toRGB()->toString() << ", color2 = " << color2->toRGB()->toString() << std::endl;

                auto hueDif = color2->h - color->h;
                auto hueDifAbs = fabs(hueDif);
                auto hueDifSign = hueDif > 0.0f;
                if(hueDifAbs > 180.0f) {
                    hueDif = 360.0f - hueDifAbs;
                    if(hueDifSign) {
                        hueDif *= -1.0f;
                    }
                }

                auto hueStep = hueDif / (float) nbPixel * 2.0f;
                auto curHue = color->h;
                // std::cout << "start hue = " << curHue << ", endHue = " << color2->h << ", pixel = " << nbPixel << ", hueStep = " << hueStep << std::endl;
                for(int i = 0; i < nbPixel / 2; ++i) {
                    led->setColor(std::make_shared<HSVColor>(curHue, 1.0f, 1.0f), i); 
                    curHue += hueStep;              
                }        
                for(int i = nbPixel / 2; i < nbPixel; ++i) {
                    led->setColor(std::make_shared<HSVColor>(curHue, 1.0f, 1.0f), i); 
                    curHue -= hueStep;              
                }        
                led->show();

                // done = true;
            }
        }

    protected:
        bool done;
    };
}

#endif /* __LEDEFFECT_COLOR_GRADIENT__ */