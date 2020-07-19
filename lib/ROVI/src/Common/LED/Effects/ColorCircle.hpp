#ifndef __LEDEFFECT_COLORCIRCLE__
#define __LEDEFFECT_COLORCIRCLE__

#include "../LEDEffect.hpp"

namespace Rovi {
    class ColorCircle : public LEDEffect {
    public:
        ColorCircle(Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms)
            , m_startIdx(0)
            , m_colors(0)
            {
                m_name = std::string{"color_circle"};
                Serial << "ColorCircle - delay: " << delay_ms << endl;
            }

        void setColors(const std::vector<std::shared_ptr<RGBColor>>& colors) {
            m_colors = colors;
        }

        void step() {
            Serial << "ColorCircle - step" << endl;

            if(!led->isAdressable()) {
                Serial << "Effect not available for this kind of led" << endl;
                return;
            }
            if(m_colors.empty()) {
                Serial << "No colors specified" << endl;
                return;
            } 

            auto nbPixel = led->nbPixel();

            for(size_t pixelIdx = 0; pixelIdx < nbPixel; pixelIdx += m_colors.size()) {
            //     auto curHue = (int32_t) ((float) currentStartColor + pixelIdx * hueIncrement ) % 360;
                for(size_t colorIdx = 0; colorIdx < m_colors.size(); ++colorIdx) {
                    auto curColor = m_colors[(colorIdx + m_startIdx) % m_colors.size()];
                    led->setColor(curColor, pixelIdx + colorIdx);
                }
            }
            ++m_startIdx;
            led->show();

            // currentStartColor = (++currentStartColor) % 360u;

            // Serial << "step() - brightness: " << brightness << " colorIdx: " << currentStartColor << endl;
        }

    protected:
        size_t m_startIdx;
        std::vector<std::shared_ptr<RGBColor>> m_colors;
    };
}

#endif /* __LEDEFFECT_COLORCIRCLE__ */