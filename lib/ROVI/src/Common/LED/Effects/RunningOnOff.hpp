#ifndef __LEDEFFECT_RUNNING_ONOFF__
#define __LEDEFFECT_RUNNING_ONOFF__

#include "../LEDEffect.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

namespace Rovi {
    class RunningOnOff : public LEDEffect {
    public:
        enum class ColorModes {
            SINGLE,
            MULTI,
            RAINBOW,
            TWO_COLOR
        };

        enum class PositionModes {
            LINEAR,
            RANDOM,
            MIDDLE_TO_OUT,
            OUT_TO_MIDDLE
        };

        RunningOnOff(ColorModes colorMode, PositionModes positionMode,
            Components::LEDComponent* led, uint32_t delay_ms = 100)
            : LEDEffect(led, delay_ms)
            , colorMode(colorMode)
            , positionMode(positionMode)
            , currentPosition(0)
            , currentStartHue(0)
            , secondColorHue(180)
            , isTurningOn(true)
            , positions()
            {
                m_name = std::string{"runningFilling"};
                std::cout << "RunningOnOff - delay: " << delay_ms << std::endl;
            }

        void step() {

            if(!led->isAdressable()) {
                std::cout << "Effect not available for this kind of led" << std::endl;
                return;
            }

            auto nbPixel = led->nbPixel();
            if(positions.size() != nbPixel) {
                positions.resize(nbPixel);

                for(uint32_t pos = 0; pos <= nbPixel; ++pos) {
                    positions[pos] = pos;
                }

                if(positionMode == PositionModes::RANDOM) {
                      std::random_shuffle ( positions.begin(), positions.end() );
                }
            }

            uint32_t currentHue;  
            switch (colorMode) {
            case ColorModes::SINGLE: {
                std::cout << "colorMode: " << "SINGLE" << std::endl;
                currentHue = currentStartHue;
                break;
            }
            case ColorModes::MULTI: {
                std::cout << "colorMode: " << "MULTI" << std::endl;
                currentHue = random(360);
                break;
            }
            case ColorModes::RAINBOW: {
                std::cout << "colorMode: " << "RAINBOW" << std::endl;
                auto hueIncrement = 360.0f / (float) nbPixel;
                currentHue = (int32_t) ((float) currentStartHue + positions[currentPosition] * hueIncrement ) % 360;
                break;       
            }
            case ColorModes::TWO_COLOR: {
                std::cout << "colorMode: " << "TWO_COLOR" << std::endl;
                currentHue = currentStartHue;
                break;     
            }
            }

            std::cout << "pixel = " << currentPosition << "/" << nbPixel << ", position = " << positions[currentPosition] << ", isTurningOn = " << isTurningOn << ", hue = " << currentHue << std::endl;

            if(isTurningOn) {
                led->setColor(std::make_shared<HSVColor>(currentHue, 1.0f, 1.0f), positions[currentPosition]);               
            } else {
                if(colorMode != ColorModes::TWO_COLOR) {
                    led->setColor(std::make_shared<HSVColor>(currentHue, 1.0f, 0.0f), positions[currentPosition]);               
                } else {
                    std::cout << "Set second color instead of off" << std::endl;
                    led->setColor(std::make_shared<HSVColor>(secondColorHue, 1.0f, 1.0f), positions[currentPosition]);               
                }
            }
            led->show();

            currentPosition = (currentPosition + 1) % nbPixel;
            if(currentPosition == 0) {
                isTurningOn = !isTurningOn;
                currentStartHue = random(360);
                secondColorHue = random(360);

                if(positionMode == PositionModes::RANDOM) {
                      std::random_shuffle ( positions.begin(), positions.end() );
                }
            }
        }

    protected:
        ColorModes colorMode;
        PositionModes positionMode;

        uint32_t currentPosition;
        uint32_t currentStartHue;
        uint32_t secondColorHue;
        bool isTurningOn;

        std::vector<uint32_t> positions;
    };
}

#endif /* __LEDEFFECT_RUNNING_ONOFF__ */