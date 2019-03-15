#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <ArduinoOTA.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include <BaseClasses/RoviDevice.hpp>
#include <Components/Output/LEDComponent.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
//#include <Components/Output/RGBLEDComponent.hpp>
#include <Common/LED/LEDEffectFactory.hpp>
#include <Components/Input/RotaryEncoderWithButton.hpp>

Rovi::RoviDevice myRovi(iot);


// RotaryEncoder
uint8_t pinA = 12;
uint8_t pinB = 13;
uint8_t pinButton = 14;

// LED
uint16_t nbNeoPixelLEDs = 20;
uint8_t  neoPixelPin    = 15;

std::shared_ptr<Rovi::Components::RotaryEncoderWithButton> rotary;
std::shared_ptr<Rovi::Components::NeoPixelComponent> leds;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial << "--- setup() ---" << endl;

  sleep(5);
  iot.begin();

  myRovi.setupRovi();

  // Setup LED
  leds = std::make_shared<Rovi::Components::NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin);
  auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
  for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
    swapRGValues[pixelIdx] = 1;
  }
  leds->setSwapRGValues(swapRGValues);

  // Setup rotary
    rotary = std::make_shared<Rovi::Components::RotaryEncoderWithButton>(pinA, pinB, pinButton);
  // NORMAL
  {
    auto normalButtonStateActivatedCallback = []() {
      Serial << "NORMAL state activation callback" << endl;
      if(leds->getPowerStatus()) {
        leds->setPower(false);
        delay(250);
        leds->setPower(true);
      }
    };
    auto normalValueChangeCallback = [&](int value) {
      uint8_t brightness = value * 10;

      Serial << "NORMAL value change callback - New value = " << value << endl;
      Serial << "  setBrightness to " << (brightness) << endl;
      leds->setBrightness(brightness);

      auto lastColor = leds->getLastColor();
      auto color = lastColor->toHSV();
      color->v = (float) brightness / 255.0f;
      leds->setColor(color);     // Required, because otherwise the color is not restored when changen from brightness 0 to 1...
                                                // TODO: Check, if this is also required and/or working for the RGB_LEDs
    };
    int normalMaxRotaryValue = 25;
    bool normalPreventOverflow = true;
    rotary->setupState(Rovi::Components::RotaryEncoderWithButton::ButtonStates::NORMAL, normalMaxRotaryValue, normalPreventOverflow, normalButtonStateActivatedCallback, normalValueChangeCallback);
  }
  
  // CLICKED
  {
    auto clickedButtonStateActivatedCallback = []() {
      Serial << "CLICKED state activation callback" << endl;

      auto nextPowerStatus = !leds->getPowerStatus();
      if(nextPowerStatus) {
        leds->startEffect();
      } else {
        leds->stopEffect();
      }

      leds->setPower(nextPowerStatus);

      auto powerTopic = (myRovi.getBaseTopic() + "/power").c_str();
      auto powerPayload = std::string{"off"};
      if(nextPowerStatus) {
        powerPayload = std::string{"on"};
      }
      auto packetID = iot.mqtt.publish(powerTopic, to_underlying(Rovi::RoviDevice::MQTTQoSClasses::AT_MOST_ONE), false, powerPayload.c_str());

    };
    auto clickedValueChangeCallback = [&](int value) {
      Serial << "CLICKED value change callback - Do nothing" << endl;
    };
    int clickedMaxRotaryValue = 10;
    bool clickedPreventOverflow = false;
    rotary->setupState(Rovi::Components::RotaryEncoderWithButton::ButtonStates::CLICKED, clickedMaxRotaryValue, clickedPreventOverflow, clickedButtonStateActivatedCallback, clickedValueChangeCallback);
  }

  // DOUBLE_CLICKED
  {
    auto doubleClickedButtonStateActivatedCallback = []() {
      Serial << "DOUBLE_CLICKED state activation callback" << endl;
      leds->stopEffect();

      leds->setPower(false);
      delay(250);
      leds->setPower(true);
      delay(250);
      leds->setPower(false);
      delay(250);
      leds->setPower(true);
    };
    auto doubleClickedValueChangeCallback = [&](int value) {
      Serial << "DOUBLE_CLICKED value change callback - New value = " << value << endl;

      leds->setColor(std::make_shared<Rovi::HSVColor>(value*10, 1.0f, 1.0f));   // TODO: getCurrentBrightness()?
    };
    int doubleClickedMaxRotaryValue = 36;
    bool doubleClickedPreventOverflow = false;
    rotary->setupState(Rovi::Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, doubleClickedMaxRotaryValue, doubleClickedPreventOverflow, doubleClickedButtonStateActivatedCallback, doubleClickedValueChangeCallback);
  }

  // HOLDED
  {
    auto holdedButtonStateActivatedCallback = []() {
      Serial << "HOLDED state activation callback" << endl;
      leds->setPower(false);
      delay(500);
      leds->setPower(true);
    };
    auto holdedValueChangeCallback = [&](int value) {
      Serial << "HOLDED value change callback - New value = " << value << endl;
      
      auto selectedEffect = Rovi::LEDEffectFactory::getEffect(value, leds.get());
      leds->setEffect(selectedEffect);
    };
    int holdedMaxRotaryValue = Rovi::LEDEffectFactory::getNumberOfEffects() - 1;
    bool holdedPreventOverflow = false;
    rotary->setupState(Rovi::Components::RotaryEncoderWithButton::ButtonStates::HOLDED, holdedMaxRotaryValue, holdedPreventOverflow, holdedButtonStateActivatedCallback, holdedValueChangeCallback);
  }
  leds->setBrightness(0);



  myRovi.addComponent(leds);
  // myRovi.addComponent(std::make_shared<RGBLEDComponent>(pinR, pinG, pinB));
  myRovi.addComponent(rotary);

  Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

  // TODO: Is there an easier way to connect to methods???
  iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
  iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
    size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword");
  Serial << "*******************************************" << endl
         << "* OTA PASSWORD:" <<  otaPassword << endl
         << "*******************************************" << endl;
  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
bool showEnableMessage = false;

void loop() {
  ArduinoOTA.handle();

  rotary->update();
  leds->update();
}

