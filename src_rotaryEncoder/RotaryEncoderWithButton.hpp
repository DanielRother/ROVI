#ifndef __ROTARY_ENCODER_WITH_BUTTON_H__
#define __ROTARY_ENCODER_WITH_BUTTON_H__

#include <Arduino.h>

#include <map>

#include <OneButton.h>

#define ENC_A 12
#define ENC_B 13

class RotaryValue {
public:
  RotaryValue();
  RotaryValue(const int maxValue, const bool preventOverflow = false);

int incrementByValue(const int increment);

int getValue() const;

protected:
  int value;
  int maxValue;
  bool preventOverflow;
};

class RotaryEncoder {
public:
  RotaryEncoder(const uint8_t pinA, const uint8_t pinB);

  /*!
   * \brief Converts the transitions of the rotary encoder into a tick of the counter
   * 
   * As there are n (typically n=4) transitions of the encoder signal during one pysical rotation tick
   * these have to be converted before updating the counter value. I.e. there have to be n transitions
   * for one update. Additionally, and to keep bouncing effects of the hardware to a minimum, there is
   * a timeout which resets the transistion counter after a while (e.g. one transition was lost but the 
   * counter is still updated after m ms as n-1 ticks are enough to savely detect the tick). 
   * 
   * \return Change of the counter value (-1,0,1) 
   */
  uint8_t getRotaryTick();

  /*!
   * \return change in encoder state (-1,0,1) 
   */
  int8_t getEncoderTransition();

protected:
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

  RotaryEncoderWithButton(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton);

  void update();
  void setupState(const ButtonStates state, int maxValue, bool preventOverflow,
    const std::function<void(void)> stateActivatedCallback, const std::function<void(int)> stateValueChangedCallback);

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

  std::string buttonStateToString(const ButtonStates state) const;

protected:
  //*************************************************************************************************//
  //*** Member variables
  //*************************************************************************************************//
  RotaryEncoder rotary;
  OneButton button;

  ButtonStates buttonState;
  unsigned long lastButtonStateUpdate_ms;

  uint8_t counter; // <- Only for testing. Will be removed/replace by a button state dependened map
  std::map<ButtonStates, RotaryValue> rotaryValuePerState;
  std::map<ButtonStates, std::function<void(void)>> buttonStateActivatedCallbacks;
  std::map<ButtonStates, std::function<void(int)>> rotaryValueChangedCallbacks;

  static const uint16_t BUTTON_STATE_TIMEOUT_MS;
};

#endif /* __ROTARY_ENCODER_WITH_BUTTON_H__ */