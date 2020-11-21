#define USE_MQTT

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
#include <Components/Output/RGBLEDComponent.hpp>


const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

#ifndef USE_MQTT
	std::shared_ptr<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 
#else
	std::shared_ptr<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>> xmastree; 
#endif

void setup()
{
  std::cout << "--- setup() ---" << endl;
  sleep(5);
  iot.begin();
  std::cout << "*********************************" << std::endl;
  std::cout << "WiFi SSID" << iot.configuration.get("WifiEssid").c_str() << "; PW: " << iot.configuration.get("WifiPassword").c_str() << std::endl;
  std::cout << "*********************************" << std::endl;
	iot.preferences.putUInt("bootcounter", 0);
	
  auto leds = std::make_shared<Rovi::Components::FastLedComponent<pin, nbPixel>>();
//   auto leds = std::make_shared<Rovi::Components::NeoPixelComponent>(pin, nbPixel);

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
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_single", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_multi", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_rainbow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_single_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_multi_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_rainbow_random", leds.get()));


#ifndef USE_MQTT
  xmastree = std::make_shared<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);
#else
  xmastree = std::make_shared<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);
#endif

	// // Testing NeoPixel and anaolg RGB led strips
	// {
	// 	// NeoPixel
	// 	// Status: Compiling and working
	// 	auto neoNbPixel = 10;
	// 	auto neoDataPin = 5;
	// 	auto neoEffects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
	// 	neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
	// 	neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
	// 	neoEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
	// 	auto neo = std::make_shared<Rovi::Components::NeoPixelComponent>(neoNbPixel, neoDataPin);
	// 	// // auto neoDevice = Rovi::Devices::SimpleLedDevice<Rovi::Components::NeoPixelComponent>(neo, neoEffects, "neo");
	// 	auto neoDevice = Rovi::Devices::LedDeviceMQTT<Rovi::Components::NeoPixelComponent>(iot, neo, neoEffects, "neo");
	// }
	// {
	// 	// Analog RGB led strip
	// 	// Status: Compiling and not tested on real hardware yet
	// 	auto pinR = 1;
	// 	auto pinG = 2;
	// 	auto pinB = 3;
	// 	auto rgbEffects = std::vector<std::shared_ptr<Rovi::LEDEffect>>();
	// 	rgbEffects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", leds.get()));
	// 	rgbEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", leds.get()));
	// 	rgbEffects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", leds.get()));
	// 	auto rgbLed = std::make_shared<Rovi::Components::RGBLEDComponent>(pinR, pinG, pinB, false, "RGB LED");
	// 	// auto rgbDevice = Rovi::Devices::SimpleLedDevice<Rovi::Components::RGBLEDComponent>(rgbLed, rgbEffects, "RGB LED Device");
	// 	auto rgbDevice = Rovi::Devices::LedDeviceMQTT<Rovi::Components::RGBLEDComponent>(iot, rgbLed, rgbEffects, "RGB LED Device");
	// }


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