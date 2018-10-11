#include <Arduino.h>

// #include <string>
// #include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

// #include <ArduinoOTA.h>

// //***************************************************************************//
// // Basecamp and Rovi initialize 
// //***************************************************************************//
// #include <Basecamp.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };

// #include "BaseClasses/RoviDevice.hpp"
// #include "LED/LEDComponent.hpp"
// #include "LED/NeoPixelComponent.hpp"
// #include "LED/RGBLEDComponent.hpp"

// RoviDevice myRovi(iot);





// //***************************************************************************//
// // Arduino setup()
// //***************************************************************************//
// void setup() {
//   Serial << "--- setup() ---" << endl;

//   sleep(5);
//   iot.begin();

//   // myRovi.addComponent(std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin));
//   // myRovi.addComponent(std::make_shared<RGBLEDComponent>(pinR, pinG, pinB));
//   myRovi.setupRovi();

//   // TODO: Is there an easier way to connect to methods???
//   iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
//   iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
//   iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
//     size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
//   iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


//   // Activate Over-the-Air updates
//   String otaPassword = iot.configuration.get("OTAPassword");
//   Serial << "*******************************************" << endl
//          << "* OTA PASSWORD:" <<  otaPassword << endl
//          << "*******************************************" << endl;
//   ArduinoOTA.setPassword(otaPassword.c_str());
//   ArduinoOTA.begin();


//   // Noch aus dem Beispiel zum hinzufügen zum Webinterface
//   // iot.web.setInterfaceElementAttribute(      // <- Das setzt das Element noch auf Readonly
//   //   "tempDisplay",
//   //   "readonly",
//   //   "true"
//   // );

//   iot.web.server.on(
//     "/temperature",
//     HTTP_GET,
//     [](AsyncWebServerRequest * request)
//   {
//     AsyncWebServerResponse *response =
//       request->beginResponse(
//         200,
//         "text/plain",
//         getTemp()
//       );
//     request->send(response);
//   });
// }


// //***************************************************************************//
// // Arduino loop()
// //***************************************************************************//
// bool showEnableMessage = false;

// void loop() {
//   ArduinoOTA.handle();

//   // sleep(5);
//   // // Serial << "--- loop() ---" << endl;
//   // // String temp = getTemp();
//   // // Serial << "  Temperatur: " << temp << endl;
//   // // iot.mqtt.publish("example/temp/", 1, true, temp.c_str());

//   // // iot.mqtt.publish("example/foo/", 1, true, "OTA hat geklappt :)");
//   // iot.mqtt.publish(myRovi.statusTopic.c_str(), 1, false, "online");

//   // Serial << "  myRovi " << myRovi.name  << " is in room " << myRovi.room << endl;
// }


#include <algorithm>
#include <math.h>
#include <map>

#include <OneButton.h>

#define ENC_A 12
#define ENC_B 13

// initially was going to hand pull the required bits from esp32 gpio via
// the 32bit functions gpio_input_get() and gpio_input_get_high() and test
// something like gpio_input_get() & (1<<11 | 1<< 21) == (1<<11 | 1<<21)
// but looking for the docs on the functions I found this line in gpio.h

//#define GPIO_INPUT_GET(gpio_no)     ((gpio_no < 32)? ((gpio_input_get()>>gpio_no)&BIT0) : ((gpio_input_get_high()>>(gpio_no - 32))&BIT0))

// this will involve two calls to the function which is not okay really,
// as the pins may have changed value between calls potentially. One should
// suffice, maybe at the expense of a second variable or additional shifts.
#include <rom/gpio.h>

class RotaryValue {
public:
RotaryValue()
: value(0), maxValue(255), preventOverflow(false) {

}

RotaryValue(const int maxValue, const bool preventOverflow = false)
: value(0), maxValue(maxValue), preventOverflow(preventOverflow) {

}

int incrementByValue(const int increment) {
  int tmpValue = value + increment;

  if(preventOverflow) {
    tmpValue = std::max(std::min(tmpValue, maxValue), 0);
  } else {
    tmpValue = tmpValue % (maxValue + 1);
  }

  value = tmpValue;
  return value;
}

int getValue() const {
  return value;
}

protected:
  int value;
  int maxValue;
  bool preventOverflow;
};

class RotaryEncoder {
public:
  enum class ButtonStates {
    NORMAL,
    CLICKED,
    DOUBLE_CLICKED,
    HOLDED
  };

  RotaryEncoder(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton)
  : pinA(pinA), pinB(pinB), pinButton(pinButton),
    rotarySignalTransitions(0.0f), lastRotaryCounterUpdate_ms(millis()),
    counter(0),
    button(pinButton, true),
    buttonState(ButtonStates::NORMAL), lastButtonStateUpdate_ms(millis()) {
      // Configure rotary encoder state pins 
      pinMode(pinA, INPUT_PULLUP);
      digitalWrite(pinA, HIGH);
      pinMode(pinB, INPUT_PULLUP);
      digitalWrite(pinB, HIGH);

      // Setup button
      button.attachClick(std::bind(&RotaryEncoder::onClick, this));   // TBD: Maybe attachPress is better suited.... Test!
      button.attachDoubleClick(std::bind(&RotaryEncoder::onDoubleClick, this));
      button.attachLongPressStart(std::bind(&RotaryEncoder::onHold, this));

      // TBD: Is it required to add default values to the counterMap?
      // I don't think so because there is a default ctor of RotaryValues
      // TODO: Test!

      auto defaultValueChangeCallback = [&](int value) {
        auto buttonStateString = buttonStateToString(buttonState);
        Serial << "defaultValueChangeCallback - New value = " << value << " for state " << buttonStateString << endl;
      };
      rotaryValueChangedCallbacks[ButtonStates::NORMAL]           = defaultValueChangeCallback;
      rotaryValueChangedCallbacks[ButtonStates::CLICKED]          = defaultValueChangeCallback;
      rotaryValueChangedCallbacks[ButtonStates::DOUBLE_CLICKED]   = defaultValueChangeCallback;
      rotaryValueChangedCallbacks[ButtonStates::HOLDED]           = defaultValueChangeCallback;

      auto defaultButtonStateActivatedCallback = []() {
        Serial << "defaultButtonStateActivatedCallback" << endl;
      };
      buttonStateActivatedCallbacks[ButtonStates::NORMAL]         = defaultButtonStateActivatedCallback;
      buttonStateActivatedCallbacks[ButtonStates::CLICKED]        = defaultButtonStateActivatedCallback;
      buttonStateActivatedCallbacks[ButtonStates::DOUBLE_CLICKED] = defaultButtonStateActivatedCallback;
      buttonStateActivatedCallbacks[ButtonStates::HOLDED]         = defaultButtonStateActivatedCallback;      
  }

  void update() {
    button.tick();
    auto curButtonState = getCurrentButtonState();

    auto tick = getRotaryTick(); 
    counter += tick;
    // TODO: Remove old counter

    auto stateValue = incrementStateValueByValue(curButtonState, tick);
    invokeRotaryValueChangeCallback(curButtonState, stateValue);

    if(tick != 0) {
      Serial << "Counter value (in update()) = " << counter << endl;
      Serial << "Counter value perState      = " << stateValue << endl;
    }
  }

  void setupState(const ButtonStates state, int maxValue, bool preventOverflow,
    const std::function<void(void)> stateActivatedCallback, const std::function<void(int)> stateValueChangedCallback) {
      rotaryValuePerState[state] = RotaryValue(maxValue, preventOverflow);
      buttonStateActivatedCallbacks[state] = stateActivatedCallback;
      rotaryValueChangedCallbacks[state] = stateValueChangedCallback;
  }

protected:
  //*************************************************************************************************//
  //*** Calculate rotary state update
  //*************************************************************************************************//
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
  uint8_t getRotaryTick() {
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

  /*!
    \return change in encoder state (-1,0,1) 
  */
  int8_t getEncoderTransition() { 
    // TODO: Create members... 
    static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    static uint8_t old_AB = 0;
    static uint32_t curval = 0;
    static uint32_t curtmpA = 0;
    static uint32_t curtmpB = 0;
    /**/
    old_AB <<= 2;                   //remember previous state
    //bit shift old_AB two positions to the left and store.

    curval = gpio_input_get();  // returns gpio pin status of pins - SEE DEFINE 
    //Serial.println("curval");
    //Serial.println(curval);
    //note to self: these curval bits are probably backwards...
    curtmpA = (curval & 1<< ENC_A ) >> ENC_A;
    //Serial.println("curtmp A");
    //Serial.println(curtmpA);

    curtmpB=(curval & 1<< ENC_B ) >> (ENC_B - 1);
    //Serial.println(curtmpB);
    old_AB |= ( ( curtmpA | curtmpB ) & 0x03 ); 
    //add current state and hopefully truncate to 8bit 
  
    return ( enc_states[( old_AB & 0x0f )]);
    // return the array item that matches the known possible encoder states

    // Thanks to kolban in the esp32 channel, who has a great book on everything iot,
    // for his initial help at my panic on the esp32 gpio access. long live IRC :)
  }

  //*************************************************************************************************//
  //*** Callback functions for the button state updates
  //*************************************************************************************************//
  void onClick() {
    updateButtonState(ButtonStates::CLICKED);
  }

  void onDoubleClick() {
    updateButtonState(ButtonStates::DOUBLE_CLICKED);
  }

  void onHold() {
    updateButtonState(ButtonStates::HOLDED);
  }

  ButtonStates getCurrentButtonState() {
    auto now = millis();
    if(now - lastButtonStateUpdate_ms > BUTTON_STATE_TIMEOUT_MS) {
      updateButtonState(ButtonStates::NORMAL);
    }

    return buttonState;
  }

  void updateButtonState(const ButtonStates state) {
    buttonState = state;
    lastButtonStateUpdate_ms = millis();

    invokeButtonStateActivatedCallback(state);
  }

  std::string buttonStateToString(const ButtonStates state) const {
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

  int incrementStateValueByValue(const ButtonStates state, const int increment) {
    return rotaryValuePerState[state].incrementByValue(increment);
  }

  void invokeRotaryValueChangeCallback(const ButtonStates state, const int value) {
    // TODO: Add MQTT here
    auto buttonStateString = buttonStateToString(state);
    Serial << "Value changed to " << value << " for ButtonState = " << buttonStateString << endl;

    rotaryValueChangedCallbacks[state](value);
  }

  void invokeButtonStateActivatedCallback(const ButtonStates state) {
    // TODO: Add MQTT here
    auto buttonStateString = buttonStateToString(state);
    Serial << "ButtonState = " << buttonStateString << " activated" << endl;

    buttonStateActivatedCallbacks[state]();
  }

protected:
  //*************************************************************************************************//
  //*** Member variables
  //*************************************************************************************************//
  uint8_t pinA;
  uint8_t pinB;
  uint8_t pinButton; 

  float rotarySignalTransitions;
  unsigned long lastRotaryCounterUpdate_ms;

  uint8_t counter; // <- Only for testing. Will be removed/replace by a button state dependened map
  std::map<ButtonStates, RotaryValue> rotaryValuePerState;
  std::map<ButtonStates, std::function<void(int)>> rotaryValueChangedCallbacks;
  std::map<ButtonStates, std::function<void(void)>> buttonStateActivatedCallbacks;

  static const float    SIGNAL_TRANSITIONS_PER_TICK;
  static const uint16_t ENCODER_TICK_UPDATE_TIMEOUT_MS;


  OneButton button;
  ButtonStates buttonState;
  unsigned long lastButtonStateUpdate_ms;

  static const uint16_t BUTTON_STATE_TIMEOUT_MS;
};

const float    RotaryEncoder::SIGNAL_TRANSITIONS_PER_TICK    = 4.0f;
const uint16_t RotaryEncoder::ENCODER_TICK_UPDATE_TIMEOUT_MS = 100;

const uint16_t RotaryEncoder::BUTTON_STATE_TIMEOUT_MS        = 10000;


RotaryEncoder rotary(ENC_A, ENC_B, -1);
 
void setup()
{
  
  Serial.begin (115200);
      delay(10);
    delay(10);

  Serial.println("Start");
}
 
void loop()
{
  rotary.update();

  delay(10);
} 

