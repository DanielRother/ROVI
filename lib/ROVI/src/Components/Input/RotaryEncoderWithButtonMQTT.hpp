#ifndef __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__
#define __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__

#include <Arduino.h>

#include "RotaryEncoderWithButton.hpp"
#include "../../Common/MQTTComponent.hpp"

namespace Rovi {
    namespace Components {
        class RotaryEncoderWithButtonMQTT : public RotaryEncoderWithButton, public MqttComponent<RotaryEncoderWithButtonMQTT> {
        public:
			RotaryEncoderWithButtonMQTT(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton, const std::string& name= "RotaryEncoderMQTT")
				: RotaryEncoderWithButton(pinA, pinB, pinButton, name)
				{}

            virtual void status(char* json, size_t maxSize) const override {
				StaticJsonBuffer<MAX_SIZE>jb;
				JsonObject& obj = jb.createObject();

				obj["buttonState"] = String{buttonStateToString(buttonState).c_str()};
				obj["lastButtonStateUpdate_ms"] = lastButtonStateUpdate_ms;

				JsonArray& values =jb.createArray();
				for(const auto& state : stateValues) {
					JsonObject& value = jb.createObject();
					value["state"] = String{buttonStateToString(state.first).c_str()};
					value["value"] = state.second.value();
					values.add(value);
				}
				obj["values"] = values;

				JsonArray& settings =jb.createArray();
				for(const auto& state : stateSettings) {
					JsonObject& setting = jb.createObject();
					setting["state"] = String{buttonStateToString(state.first).c_str()};
					setting["increment"] = state.second.increment;
					setting["minValue"] = state.second.minValue;
					setting["maxValue"] = state.second.maxValue;
					setting["preventOverflow"] = state.second.preventOverflow;
					settings.add(setting);
				}
				obj["settings"] = settings;

    			obj.printTo(json, maxSize);
			}

			static const size_t MAX_SIZE = 2500;//1146 = 450 + 2*JSON_ARRAY_SIZE(4) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 4*JSON_OBJECT_SIZE(5);
        };

    }
}

#endif /* __ROTARY_ENCODER_WITH_BUTTON_MQTT_H__ */