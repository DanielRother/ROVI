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
#include <Components/Output/NeoPixelComponent.hpp>


const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

// std::shared_ptr<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 
// std::shared_ptr<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 
std::shared_ptr<Rovi::Devices::LedDeviceMQTT<Rovi::Components::NeoPixelComponent>> xmastree; 

void setup()
{
  std::cout << "--- setup() ---" << endl;
  sleep(5);
  iot.begin();
  // myRovi.setupRovi();

//   auto leds = std::make_shared<Rovi::Components::FastLedComponent<pin, nbPixel>>();
  auto leds = std::make_shared<Rovi::Components::NeoPixelComponent>(pin, nbPixel);

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
//   xmastree = std::make_shared<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);
  xmastree = std::make_shared<Rovi::Devices::LedDeviceMQTT<Rovi::Components::NeoPixelComponent>>(iot, leds, effects, name);


	// // Testing NeoPixel and anaolg RGB led strips
	// auto neoNbPixel = 10;
	// auto neoDataPin = 5;
	// auto neoEffects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
	// neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
	// neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
	// neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
	// auto neo = std::make_shared<Rovi::Components::NeoPixelComponent>(neoNbPixel, neoDataPin);
	// // auto neoDevice = Rovi::Devices::SimpleLedDevice<Rovi::Components::NeoPixelComponent>(neo, neoEffects, "neo");
	// // auto neoDevice = Rovi::Devices::LedDeviceMQTT<Rovi::Components::NeoPixelComponent>(iot, neo, neoEffects, "neo");



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