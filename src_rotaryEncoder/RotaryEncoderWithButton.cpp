#include "RotaryEncoderWithButton.hpp"

#include <map>
#include <list>

#include <ArduinoIostream.hpp>

#include <rom/gpio.h>


RotaryValue::RotaryValue()
: value(0), maxValue(255), preventOverflow(false) {
}

RotaryValue::RotaryValue(const int maxValue, const bool preventOverflow)
: value(0), maxValue(maxValue), preventOverflow(preventOverflow) {
}

int RotaryValue::incrementByValue(const int increment) {
  int tmpValue = value + increment;

  if(preventOverflow) {
    tmpValue = std::max(std::min(tmpValue, maxValue), 0);
  } else {
    tmpValue = tmpValue % (maxValue + 1);
  }

  value = tmpValue;
  return value;
}

int RotaryValue::getValue() const {
  return value;
}

RotaryEncoder::RotaryEncoder(const uint8_t pinA, const uint8_t pinB)
  : pinA(pinA), pinB(pinB), 
  old_AB(0),
  rotarySignalTransitions(0.0f), lastRotaryCounterUpdate_ms(millis()) {
      // Configure rotary encoder state pins
      pinMode(pinA, INPUT_PULLUP);
      digitalWrite(pinA, HIGH);
      pinMode(pinB, INPUT_PULLUP);
      digitalWrite(pinB, HIGH);
}


uint8_t RotaryEncoder::getRotaryTick() {
    uint8_t tick = 0;

    auto transition = getEncoderTransition();
    rotarySignalTransitions += transition;

    auto now = millis();
    if(transition) {
        lastRotaryCounterUpdate_ms = now;
    }

    if(fabs(rotarySignalTransitions) >= SIGNAL_TRANSITIONS_PER_TICK ||
        (now - lastRotaryCounterUpdate_ms > ENCODER_TICK_UPDATE_TIMEOUT_MS && rotarySignalTransitions != 0.0f)) {
        // TBD: Is the last condition still required

        tick = round(rotarySignalTransitions / SIGNAL_TRANSITIONS_PER_TICK);

        rotarySignalTransitions = 0;
        lastRotaryCounterUpdate_ms = now;
    }

    return tick;
}

// This code is based on https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/ and 
// Original comment:
// initially was going to hand pull the required bits from esp32 gpio via
// the 32bit functions gpio_input_get() and gpio_input_get_high() and test
// something like gpio_input_get() & (1<<11 | 1<< 21) == (1<<11 | 1<<21)
// but looking for the docs on the functions I found this line in gpio.h

//#define GPIO_INPUT_GET(gpio_no)     ((gpio_no < 32)? ((gpio_input_get()>>gpio_no)&BIT0) : ((gpio_input_get_high()>>(gpio_no - 32))&BIT0))

// this will involve two calls to the function which is not okay really,
// as the pins may have changed value between calls potentially. One should
// suffice, maybe at the expense of a second variable or additional shifts.
int8_t RotaryEncoder::getEncoderTransition() {
    // TODO: Create members...
    // static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    // static uint8_t old_AB = 0;
    // static uint32_t curval = 0;
    // static uint32_t curtmpA = 0;
    // static uint32_t curtmpB = 0;
    /**/
    old_AB <<= 2;                   //remember previous state
    //bit shift old_AB two positions to the left and store.

    uint32_t curval = gpio_input_get();  // returns gpio pin status of pins - SEE DEFINE
    //note to self: these curval bits are probably backwards...
    uint32_t curtmpA = (curval & 1<< pinA ) >> pinA;

    uint32_t curtmpB=(curval & 1<< pinB ) >> (pinB - 1);
    old_AB |= ( ( curtmpA | curtmpB ) & 0x03 );
    //add current state and hopefully truncate to 8bit

    return ( enc_states[( old_AB & 0x0f )]);
    // return the array item that matches the known possible encoder states

    // Thanks to kolban in the esp32 channel, who has a great book on everything iot,
    // for his initial help at my panic on the esp32 gpio access. long live IRC :)
}

const int8_t   RotaryEncoder::enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
const float    RotaryEncoder::SIGNAL_TRANSITIONS_PER_TICK    = 4.0f;
const uint16_t RotaryEncoder::ENCODER_TICK_UPDATE_TIMEOUT_MS = 100;


RotaryEncoderWithButton::RotaryEncoderWithButton(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton)
  : rotary(pinA, pinB), button(pinButton, true),
    buttonState(ButtonStates::NORMAL), lastButtonStateUpdate_ms(millis()) {
      // Setup button, i.e. callbacks
      button.attachClick(std::bind(&RotaryEncoderWithButton::onClick, this));   // TBD: Maybe attachPress is better suited.... Test!
      button.attachDoubleClick(std::bind(&RotaryEncoderWithButton::onDoubleClick, this));
      button.attachLongPressStart(std::bind(&RotaryEncoderWithButton::onHold, this));

      auto defaultButtonStateActivatedCallback = []() {
        Serial << "defaultButtonStateActivatedCallback" << endl;
      };
      auto defaultValueChangeCallback = [&](int value) {
        auto buttonStateString = buttonStateToString(buttonState);
        Serial << "defaultValueChangeCallback - New value = " << value << " for state " << buttonStateString << endl;
      };

      std::list<ButtonStates> states = {ButtonStates::NORMAL, ButtonStates::CLICKED, ButtonStates::DOUBLE_CLICKED, ButtonStates::HOLDED};
      for(auto& state : states) {
        buttonStateActivatedCallbacks[state] = defaultButtonStateActivatedCallback;
        rotaryValueChangedCallbacks[state]   = defaultValueChangeCallback;
      }
}

void RotaryEncoderWithButton::update() {
    button.tick();
    auto curButtonState = getCurrentButtonState();
    auto tick = rotary.getRotaryTick();

    if(tick != 0) {
        auto stateValue = incrementStateValueByValue(curButtonState, tick);
        invokeRotaryValueChangeCallback(curButtonState, stateValue);
    }
}

void RotaryEncoderWithButton::setupState(const ButtonStates state, int maxValue, bool preventOverflow,
  const std::function<void(void)> stateActivatedCallback, const std::function<void(int)> stateValueChangedCallback) {
    rotaryValuePerState[state] = RotaryValue(maxValue, preventOverflow);
    buttonStateActivatedCallbacks[state] = stateActivatedCallback;
    rotaryValueChangedCallbacks[state] = stateValueChangedCallback;
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

RotaryEncoderWithButton::ButtonStates RotaryEncoderWithButton::getCurrentButtonState() {
    auto now = millis();
    if(now - lastButtonStateUpdate_ms > BUTTON_STATE_TIMEOUT_MS && buttonState != ButtonStates::NORMAL) {
        updateButtonState(ButtonStates::NORMAL);
    }

    return buttonState;
}

void RotaryEncoderWithButton::updateButtonState(const ButtonStates state) {
    buttonState = state;
    lastButtonStateUpdate_ms = millis();

    invokeButtonStateActivatedCallback(state);
}

int RotaryEncoderWithButton::incrementStateValueByValue(const ButtonStates state, const int increment) {
    return rotaryValuePerState[state].incrementByValue(increment);
}

void RotaryEncoderWithButton::invokeRotaryValueChangeCallback(const ButtonStates state, const int value) {
    // TODO: Add MQTT here
    auto buttonStateString = buttonStateToString(state);
    Serial << "Value changed to " << value << " for ButtonState = " << buttonStateString << endl;

    rotaryValueChangedCallbacks[state](value);
}

void RotaryEncoderWithButton::invokeButtonStateActivatedCallback(const ButtonStates state) {
    // TODO: Add MQTT here
    auto buttonStateString = buttonStateToString(state);
    Serial << "ButtonState = " << buttonStateString << " activated" << endl;

    buttonStateActivatedCallbacks[state]();
}

std::string RotaryEncoderWithButton::buttonStateToString(const ButtonStates state) const {
    switch (state) {
        case ButtonStates::NORMAL:
            return "NORMAL";
            break;
        case ButtonStates::CLICKED:
            return "CLICKED";
            break;
        case ButtonStates::DOUBLE_CLICKED:
            return "DOUBLE_CLICKED";
            break;
        case ButtonStates::HOLDED:
            return "HOLDED";
            break;
        default:
            break;
    }

    return "";
}

const uint16_t RotaryEncoderWithButton::BUTTON_STATE_TIMEOUT_MS        = 10000;