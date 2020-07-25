#include <Arduino.h>
#include <ArduinoOTA.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include <Devices/LedDeviceMQTT.hpp>
#include <Components/Output/FastLedComponent.hpp>

// #include <BaseClasses/RoviDevice.hpp>
// Rovi::Deprecated::RoviDevice myRovi(iot);

const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

// std::shared_ptr<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 
std::shared_ptr<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 

void setup()
{
  std::cout << "--- setup() ---" << endl;
  sleep(5);
  iot.begin();
  // myRovi.setupRovi();

  auto leds = std::make_shared<Rovi::Components::FastLedComponent<pin, nbPixel>>();
  // xmastree = std::make_shared<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>>(leds, name);

  auto effects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
  effects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("rainbow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_rgb", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_red", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_blue", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_green", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_purple", leds.get()));
  // xmastree->setEffects(effects);
    xmastree = std::make_shared<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);

  // sleep(60);

  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword"); // The prefedined variable OTAPassword is always reset to '1'; TODO: Check why/fix?
  std::cout << "*******************************************" << endl
         << "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
         << "*******************************************" << endl;

  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();
}

void loop()
{
    ArduinoOTA.handle();
    xmastree->update();
}