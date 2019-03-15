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
            LEDComponent(const LEDComponent& other);
            virtual ~LEDComponent() /*= default*/;

            // The convertion between string and concrete data type can be done in this abstract class
            // But the actual action have to be implemented in an derived class
            // Names due to overloaded function and bind make problems and would led to long typecasts...
            virtual void setPowerMQTT(const std::string& payload);
            virtual void setColorMQTT(const std::string& payload);
            virtual void setBrightnessMQTT(const std::string& payload);
            virtual void setEffectMQTT(const std::string& payload);


            // Interfac for the derived class
            virtual void setColor(const std::shared_ptr<Color>& color) = 0;
            virtual void setPower(bool power) = 0;
            virtual void setBrightness(uint8_t brightness) = 0;

            // Other member methods
            void update();

            void setEffect(std::shared_ptr<LEDEffect> selectedEffect);

            std::shared_ptr<Color> getLastColor() const ;
            bool getPowerStatus() const;
            void startEffect();
            void stopEffect();

        // protected:
            std::shared_ptr<Color> lastColor;
            bool powerStatus;

            std::shared_ptr<LEDEffect> effect;
        };
    }
}

#endif /* __LEDCOMPONENT_H__ */