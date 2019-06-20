#ifndef __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__
#define __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__

#include "BaseClasses/RoviComponent.hpp"
#include "Components/Input/RotaryEncoderWithButton.hpp"

namespace Rovi {
    namespace Components {
        namespace Deprecated {
            class RotaryEncoderWithButtonMQTT : public RotaryEncoderWithButton, public Rovi::Deprecated::RoviComponent {
                public:
                RotaryEncoderWithButtonMQTT(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton, 
                    const std::string& name = "RotaryEncoderMQTT")
                    : RotaryEncoderWithButton(pinA, pinB, pinButton)
                    , RoviComponent(name) {

                    }

            };

            
        }
    }
}

#endif /* __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__ */