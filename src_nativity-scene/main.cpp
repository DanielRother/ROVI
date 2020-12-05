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


const auto nbPixel = 27;
const auto pin = 13;
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
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_twocolor", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_rainbow", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_single_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_multi_random", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_twocolor_zufall", leds.get()));
  effects.push_back(Rovi::LEDEffectFactory::getEffect("running_onoff_rainbow_random", leds.get()));


#ifndef USE_MQTT
  xmastree = std::make_shared<Rovi::Devices::SimpleLedDevice<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);
#else
  xmastree = std::make_shared<Rovi::Devices::LedDeviceMQTT<Rovi::Components::FastLedComponent<pin, nbPixel>>>(iot, leds, effects, name);
#endif

  // Deactivae automatic effect change
  xmastree->setTimePerEffect(std::chrono::minutes{0});

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