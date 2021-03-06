#include "RotaryEncoderWithButton.hpp"

#include <list>
#include <map>
#include <string>

#include <ArduinoIostream.hpp>
#include <FileIOUtils.hpp>

#include <rom/gpio.h>

namespace Rovi {
namespace Components {
RotaryValue::RotaryValue() : RotaryValue(255, 0, false) {}

RotaryValue::RotaryValue(const int maxValue, const int minValue,
                         const bool preventOverflow)
    : m_value(0), m_maxValue(maxValue), m_minValue(minValue),
      m_preventOverflow(preventOverflow) {}

int RotaryValue::incrementByValue(const int increment) {
  auto tmpValue = m_value + increment;

  return setValue(tmpValue);
}

int RotaryValue::value() const { return m_value; }

int RotaryValue::setValue(const int value) {
  auto tmpValue = value;
  if (m_preventOverflow) {
    tmpValue =
        std::max(std::min(value, m_maxValue), m_minValue); // TODO: clamp()
  } else {
    // TODO: test negative overflow?
    auto range = m_minValue + m_maxValue;
    tmpValue += m_minValue;
    tmpValue = tmpValue % (range + 1);
    tmpValue -= m_minValue;

    if (tmpValue < 0) {
      tmpValue += range + 1;
    }
  }

  m_value = tmpValue;
  return m_value;
}

RotaryEncoder::RotaryEncoder(const uint8_t pinA, const uint8_t pinB)
    : pinA(pinA), pinB(pinB), old_AB(0), rotarySignalTransitions(0.0f),
      lastRotaryCounterUpdate_ms(millis()) {
  // Configure rotary encoder state pins
  pinMode(pinA, INPUT_PULLUP);
  digitalWrite(pinA, HIGH);
  pinMode(pinB, INPUT_PULLUP);
  digitalWrite(pinB, HIGH);
}

int8_t RotaryEncoder::getRotaryTick() {
  uint8_t tick = 0;

  auto transition = getEncoderTransition();
  rotarySignalTransitions += transition;

  auto now = millis();
  if (transition) {
    lastRotaryCounterUpdate_ms = now;
  }

  if (fabs(rotarySignalTransitions) >= SIGNAL_TRANSITIONS_PER_TICK ||
      (now - lastRotaryCounterUpdate_ms > ENCODER_TICK_UPDATE_TIMEOUT_MS)) {

    tick = round(rotarySignalTransitions / SIGNAL_TRANSITIONS_PER_TICK);

    rotarySignalTransitions = 0;
    lastRotaryCounterUpdate_ms = now;
  }

  return tick;
}

// This code is based on
// https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/ and
// https://www.hackster.io/tyeth/rotary-quadrature-encoder-let-s-make-a-digital-safe-769ca4
// Original comment:
// initially was going to hand pull the required bits from esp32 gpio via
// the 32bit functions gpio_input_get() and gpio_input_get_high() and test
// something like gpio_input_get() & (1<<11 | 1<< 21) == (1<<11 | 1<<21)
// but looking for the docs on the functions I found this line in gpio.h

//#define GPIO_INPUT_GET(gpio_no)     ((gpio_no < 32)?
//((gpio_input_get()>>gpio_no)&BIT0) : ((gpio_input_get_high()>>(gpio_no -
// 32))&BIT0))

// this will involve two calls to the function which is not okay really,
// as the pins may have changed value between calls potentially. One should
// suffice, maybe at the expense of a second variable or additional shifts.
int8_t RotaryEncoder::getEncoderTransition() {
  old_AB <<= 2; // remember previous state
  // bit shift old_AB two positions to the left and store.

  uint32_t curval =
      gpio_input_get(); // returns gpio pin status of pins - SEE DEFINE
  // note to self: these curval bits are probably backwards...
  uint32_t curtmpA = (curval & 1 << pinA) >> pinA;

  uint32_t curtmpB = (curval & 1 << pinB) >> (pinB - 1);
  old_AB |= ((curtmpA | curtmpB) & 0x03);
  // add current state and hopefully truncate to 8bit

  return (enc_states[(old_AB & 0x0f)]);
  // return the array item that matches the known possible encoder states

  // Thanks to kolban in the esp32 channel, who has a great book on everything
  // iot, for his initial help at my panic on the esp32 gpio access. long live
  // IRC :)
}

const int8_t RotaryEncoder::enc_states[] = {0,  -1, 1, 0, 1, 0, 0,  -1,
                                            -1, 0,  0, 1, 0, 1, -1, 0};
const float RotaryEncoder::SIGNAL_TRANSITIONS_PER_TICK = 4.0f;
const uint16_t RotaryEncoder::ENCODER_TICK_UPDATE_TIMEOUT_MS = 100;

RotaryEncoderWithButton::RotaryEncoderWithButton(const uint8_t pinA,
                                                 const uint8_t pinB,
                                                 const uint8_t pinButton,
                                                 const std::string &name)
    : rotary(pinA, pinB), button(pinButton, true),
      buttonState(ButtonStates::NORMAL), lastButtonStateUpdate_ms(millis()) {
  // Setup button, i.e. callbacks
  button.attachClick(std::bind(&RotaryEncoderWithButton::onClick, this));
  button.attachDoubleClick(
      std::bind(&RotaryEncoderWithButton::onDoubleClick, this));
  button.attachLongPressStart(
      std::bind(&RotaryEncoderWithButton::onHold, this));

  auto defaultButtonStateActivatedCallback = []() {
    Serial << "defaultButtonStateActivatedCallback" << endl;
  };
  auto defaultValueChangeCallback = [&](int value) {
    auto buttonStateString = buttonState._to_string();
    Serial << "defaultValueChangeCallback - New value = " << value
           << " for state " << buttonStateString << endl;
  };

  std::list<ButtonStates> states = {ButtonStates::NORMAL, ButtonStates::CLICKED,
                                    ButtonStates::DOUBLE_CLICKED,
                                    ButtonStates::HOLDED};
  for (auto &state : states) {
    stateSettings[state].activationCallback =
        defaultButtonStateActivatedCallback;
    stateSettings[state].valueChangedCallback = defaultValueChangeCallback;
    stateSettings[state].increment = 1;
    stateSettings[state].minValue = 0;
    stateSettings[state].maxValue = 255;
    stateSettings[state].preventOverflow = false;
  }
}

// RotaryEncoderWithButton::RotaryEncoderWithButton(const RotaryEncoder& other)
//   : RoviComponent(other.name), rotary
// {

// }

void RotaryEncoderWithButton::update() {
  button.tick();
  auto curButtonState = getCurrentButtonState();
  auto tick = rotary.getRotaryTick();
  auto newValue = tick * stateSettings[curButtonState].increment;

  if (tick != 0) {
    lastButtonStateUpdate_ms = millis();
    auto stateValue = incrementStateValueByValue(curButtonState, newValue);
    invokeRotaryValueChangeCallback(curButtonState, stateValue);
  }
}

void RotaryEncoderWithButton::setupState(
    const ButtonStates state, const int minValue, const int maxValue,
    const int increment, const bool preventOverflow,
    const std::function<void(void)> stateActivatedCallback,
    const std::function<void(int)> stateValueChangedCallback) {
  std::cout << "Setup rotary state " << state._to_string()
            << ": minValue = " << minValue << ", maxValue = " << maxValue
            << ", increment = " << increment
            << ", preventOverflow = " << preventOverflow << endl;
  stateSettings[state].activationCallback = stateActivatedCallback;
  stateSettings[state].valueChangedCallback = stateValueChangedCallback;
  stateSettings[state].increment = increment;
  stateSettings[state].minValue = minValue;
  stateSettings[state].maxValue = maxValue;
  stateSettings[state].preventOverflow = preventOverflow;
  stateValues[state] = RotaryValue(maxValue, minValue, preventOverflow);
}

void RotaryEncoderWithButton::setupState(
    const ButtonStates state, const int maxValue, const bool preventOverflow,
    const std::function<void(void)> stateActivatedCallback,
    const std::function<void(int)> stateValueChangedCallback) {
  auto minValue = 0;
  auto increment = 1;
  return setupState(state, minValue, maxValue, increment, preventOverflow,
                    stateActivatedCallback, stateValueChangedCallback);
}

int RotaryEncoderWithButton::value(const ButtonStates state) /* const */ {
  return stateValues[state].value();
}

int RotaryEncoderWithButton::setValue(const ButtonStates state,
                                      const int value) {
  return stateValues[state].setValue(value);
}

//*************************************************************************************************//
//*** Callback functions for the button state updates
//*************************************************************************************************//
void RotaryEncoderWithButton::onClick() {
  updateButtonState(ButtonStates::CLICKED);
}

void RotaryEncoderWithButton::onDoubleClick() {
  updateButtonState(ButtonStates::DOUBLE_CLICKED);
}

void RotaryEncoderWithButton::onHold() {
  updateButtonState(ButtonStates::HOLDED);
}

ButtonStates RotaryEncoderWithButton::getCurrentButtonState() {
  auto now = millis();

  // Don't know why but the following is not possible using better enums... :(
  // Therefore the strange workaround below
  //   if (now - lastButtonStateUpdate_ms > BUTTON_STATE_TIMEOUT_MS &&
  //       (buttonState != ButtonStates::NORMAL)) {
  //     updateButtonState(ButtonStates::NORMAL);
  //   }

  switch (buttonState) {
  case ButtonStates::NORMAL:
    return buttonState;
    break;
  }

  if (now - lastButtonStateUpdate_ms > BUTTON_STATE_TIMEOUT_MS) {
    updateButtonState(ButtonStates::NORMAL);
  }
  return buttonState;
}

unsigned long RotaryEncoderWithButton::getLastButtonStateUpdateMs() const {
  return lastButtonStateUpdate_ms;
}

std::map<ButtonStates, RotaryEncoderWithButton::ButtonStateSetting>
RotaryEncoderWithButton::getStateSettings() const {
  return stateSettings;
}
std::map<ButtonStates, RotaryValue>
RotaryEncoderWithButton::getStateValues() const {
  return stateValues;
}

void RotaryEncoderWithButton::updateButtonState(const ButtonStates state) {
  buttonState = state;
  lastButtonStateUpdate_ms = millis();

  invokeButtonStateActivatedCallback(state);
}

int RotaryEncoderWithButton::incrementStateValueByValue(
    const ButtonStates state, const int increment) {
  return stateValues[state].incrementByValue(increment);
}

void RotaryEncoderWithButton::invokeRotaryValueChangeCallback(
    const ButtonStates state, const int value) {
  stateSettings[state].valueChangedCallback(value);

  // Debugging output
  std::string buttonStateString = state._to_string();
  Serial << "Value changed to " << value
         << " for ButtonState = " << buttonStateString << endl;
}

void RotaryEncoderWithButton::invokeButtonStateActivatedCallback(
    const ButtonStates state) {
  stateSettings[state].activationCallback();

  // Debugging output
  std::string buttonStateString = state._to_string();
  Serial << "ButtonState = " << buttonStateString << " activated" << endl;
}

const uint16_t RotaryEncoderWithButton::BUTTON_STATE_TIMEOUT_MS = 5000;
} // namespace Components
} // namespace Rovi