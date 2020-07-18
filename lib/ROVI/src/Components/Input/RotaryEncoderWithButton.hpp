#ifndef __ROTARY_ENCODER_WITH_BUTTON_H__
#define __ROTARY_ENCODER_WITH_BUTTON_H__

#include <Arduino.h>

#include <map>

#include "OneButtonFork.h"

// TODO: Cleanup nomenclature

namespace Rovi {
    namespace Components {
        class RotaryValue {
        public:
          RotaryValue();
          RotaryValue(const int maxValue, const int minValue = 0, const bool preventOverflow = false);

        int incrementByValue(const int increment);

        int value() const;
        int setValue(const int value);

        protected:
          int m_value;
          int m_maxValue;
          int m_minValue;
          bool m_preventOverflow;
        };


        class RotaryEncoder {
        public:
          RotaryEncoder(const uint8_t pinA, const uint8_t pinB);
          RotaryEncoder(const RotaryEncoder& other) = default;
          virtual ~RotaryEncoder() = default;

          /*!
          * \brief Converts the transitions of the rotary encoder into a tick of the counter
          * 
          * As there are n (typically n=4) transitions of the encoder signal during one pysical rotation tick
          * these have to be converted before updating the counter value. I.e. there have to be n transitions
          * for one update. Additionally, and to keep bouncing effects of the hardware to a minimum, there is
          * a timeout which resets the transistion counter after a while (e.g. one transition was lost but the 
          * counter is still updated after m ms as n-1 transitions are enough to savely detect the tick). 
          * 
          * \return Change of the encoder tick value (-1,0,1) 
          */
          int8_t getRotaryTick();

          /*!
          * \return change in encoder transition value (-1,0,1) 
          */
          int8_t getEncoderTransition();

        protected:
          uint8_t pinA;
          uint8_t pinB;

          static const int8_t enc_states[];
          uint8_t old_AB;

          float rotarySignalTransitions;
          unsigned long lastRotaryCounterUpdate_ms;

          static const float    SIGNAL_TRANSITIONS_PER_TICK;
          static const uint16_t ENCODER_TICK_UPDATE_TIMEOUT_MS;
        };


        class RotaryEncoderWithButton {
        public:
          enum class ButtonStates {
            NORMAL,
            CLICKED,
            DOUBLE_CLICKED,
            HOLDED
          };

          static std::string buttonStateToString(const ButtonStates state);

          struct ButtonStateSetting {
            int increment;
            std::function<void(void)> activationCallback;
            std::function<void(int)> valueChangedCallback;
          };

          RotaryEncoderWithButton(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton, const std::string& name = "RotaryEncoder");
          // RotaryEncoderWithButton(const RotaryEncoder& other);
          virtual ~RotaryEncoderWithButton() = default;

          void update();
          void setupState(const ButtonStates state, const int minValue, const int maxValue, const int increment, const bool preventOverflow,
            const std::function<void(void)> stateActivatedCallback, const std::function<void(int)> stateValueChangedCallback);
          void setupState(const ButtonStates state, const int maxValue, const bool preventOverflow,
            const std::function<void(void)> stateActivatedCallback, const std::function<void(int)> stateValueChangedCallback);

          int value(const ButtonStates state) /* const */;    // TODO: Re-add const
          int setValue(const ButtonStates state, const int value);

        protected:
          //*************************************************************************************************//
          //*** Callback functions for the button state updates
          //*************************************************************************************************//
          void onClick();
          void onDoubleClick();
          void onHold();

          ButtonStates getCurrentButtonState();
          void updateButtonState(const ButtonStates state);

          int incrementStateValueByValue(const ButtonStates state, const int increment);
          void invokeRotaryValueChangeCallback(const ButtonStates state, const int value);
          void invokeButtonStateActivatedCallback(const ButtonStates state);

          // std::string prepareMQTTTopicString() const;

        protected:
          //*************************************************************************************************//
          //*** Member variables
          //*************************************************************************************************//
          RotaryEncoder rotary;
          OneButton button;

          ButtonStates buttonState;
          unsigned long lastButtonStateUpdate_ms;

          std::map<ButtonStates, ButtonStateSetting> stateSettings;
          std::map<ButtonStates, RotaryValue> stateValues;

          // std::map<ButtonStates, RotaryValue> rotaryValuePerState;
          // std::map<ButtonStates, std::function<void(void)>> buttonStateActivatedCallbacks;
          // std::map<ButtonStates, std::function<void(int)>> rotaryValueChangedCallbacks;
          // int m_increment;

          static const uint16_t BUTTON_STATE_TIMEOUT_MS;
        };
    }
}

#endif /* __ROTARY_ENCODER_WITH_BUTTON_H__ */