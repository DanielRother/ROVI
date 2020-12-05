#ifndef __LEDCOMPONENT_H__
#define __LEDCOMPONENT_H__

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>
#include <ThreadUtil.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "Common/LED/ColorTypes.h"

namespace Components {
    class LEDComponent;
}
#include "Common/LED/LEDEffect.hpp"

namespace Rovi {
    namespace Components {
        // TODO: Use Homie payload data types
        class LEDComponent {
        public:
            LEDComponent(const std::string& name = "LED");
            // LEDComponent(const LEDComponent& other);
            virtual ~LEDComponent() /*= default*/;

            // Member methods
            virtual void init() {};
            void update();
            bool isOn() const;
            void setOn(bool on);
            uint8_t brightness() const;
            void setBrightness(uint8_t brightness);
            std::shared_ptr<Color> color() const ;
            std::shared_ptr<Color> color1() const ;
            std::shared_ptr<Color> color2() const ;
            void setColor(const std::shared_ptr<Color>& color);
            void setColor1(const std::shared_ptr<Color>& color);
            void setColor2(const std::shared_ptr<Color>& color);
            std::shared_ptr<LEDEffect> effect() const;
            uint32_t hue() const;
            void setHue(uint32_t hue);
            void setEffect(const std::shared_ptr<LEDEffect>& effect);
            void setEffect(int effect);
            void startEffect();
            void stopEffect();

            virtual bool isAdressable() const;
            virtual size_t nbPixel() const;
            void setColor(const std::shared_ptr<Color>& color, size_t pixelIdx);
            std::shared_ptr<Color> color(size_t pixelIdx) const;
            virtual void show();

        protected:
            // Interface for the derived class
            virtual void setBrightnessImpl(uint8_t brightness) = 0;
            virtual void setColorImpl(const std::shared_ptr<Color>& color) = 0;

            virtual void setColorImpl(const std::shared_ptr<Color>& color, size_t pixelIdx) = 0;
            virtual std::shared_ptr<Color> colorImpl(size_t pixelIdx) const = 0;
            virtual void showImpl() = 0;

            // TODO: Use Homie/PalyoadDataTypes
            bool m_on;
            uint8_t m_brightness;
            std::shared_ptr<Color> m_color;
            std::shared_ptr<Color> m_color1;        // Aus irgendeinem verfickten Grund wird m_color nicht immer ordentlich gesetzt...
            std::shared_ptr<Color> m_color2;
            std::shared_ptr<LEDEffect> m_effect;
        };

        // class LEDComponentDepracedRovi : public Deprecated::RoviComponent, public LEDComponent /*, public std::enable_shared_from_this<LEDComponent>*/ {
        // public:
        //     // The convertion between string and concrete data type can be done in this abstract class
        //     // But the actual action have to be implemented in an derived class
        //     // Names due to overloaded function and bind make problems and would led to long typecasts...
        //     virtual void setPowerMQTT(const std::string& payload);
        //     virtual void setColorMQTT(const std::string& payload);
        //     virtual void setBrightnessMQTT(const std::string& payload);
        //     virtual void setEffectMQTT(const std::string& payload);
        // };

        class AdressableLedComponent : public LEDComponent {
        public:
            AdressableLedComponent(size_t nbPixel, const std::string& name = "AdressableLedComponent");
            // AdressableLedComponent(const AdressableLedComponent& other);
            virtual ~AdressableLedComponent() /*= default*/;      

            virtual bool isAdressable() const override;
            virtual size_t nbPixel() const override;

            void setSwapRGValues(const std::vector<uint32_t> newSwapRGValues) {
                m_swapRGValues = newSwapRGValues;
            }

        protected:
            virtual void setColorImpl(const std::shared_ptr<Color>& color, size_t pixelIdx);
            virtual std::shared_ptr<Color> colorImpl(size_t pixelIdx) const;

            size_t m_nbPixel;
            std::vector<std::shared_ptr<Color>> m_colors;
            std::vector<uint32_t> m_swapRGValues;
        };
    }
}

#endif /* __LEDCOMPONENT_H__ */