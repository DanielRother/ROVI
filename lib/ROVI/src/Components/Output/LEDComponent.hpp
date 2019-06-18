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
        class LEDComponent : public Deprecated::RoviComponent /*, public std::enable_shared_from_this<LEDComponent>*/ {
        public:
            LEDComponent(const std::string& name = "LED");
            // LEDComponent(const LEDComponent& other);
            virtual ~LEDComponent() /*= default*/;

            // Member methods
            void update();
            bool isOn() const;
            void setOn(bool on);
            std::shared_ptr<Color> color() const ;
            void setColor(const std::shared_ptr<Color>& color);
            std::shared_ptr<LEDEffect> effect() const;
            void setEffect(std::shared_ptr<LEDEffect> selectedEffect);
            void startEffect();
            void stopEffect();
            uint8_t brightness() const;
            void setBrightness(uint8_t brightness);


            // TODO: Move to subclass
            // The convertion between string and concrete data type can be done in this abstract class
            // But the actual action have to be implemented in an derived class
            // Names due to overloaded function and bind make problems and would led to long typecasts...
            virtual void setPowerMQTT(const std::string& payload);
            virtual void setColorMQTT(const std::string& payload);
            virtual void setBrightnessMQTT(const std::string& payload);
            virtual void setEffectMQTT(const std::string& payload);

        protected:
            // Interface for the derived class
            virtual void setBrightnessImpl(uint8_t brightness) = 0;
            virtual void setColorImpl(const std::shared_ptr<Color>& color) = 0;

            bool m_on;
            uint8_t m_brightness;
            std::shared_ptr<Color> m_color;
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
    }
}

#endif /* __LEDCOMPONENT_H__ */