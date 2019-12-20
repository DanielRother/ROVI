#include <Arduino.h>

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
#include <Components/Output/NeoPixelComponent.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Common/LED/LEDEffectFactory.hpp>

Rovi::Deprecated::RoviDevice myRovi(iot);


const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
// auto xmastree = std::make_shared<Rovi::Components::NeoPixelComponent>(nbPixel, pin, name);
auto xmastree = std::make_shared<Rovi::Components::FastLedComponent<nbPixel, pin>>(name);
auto colorFlow = Rovi::LEDEffectFactory::getEffect("color_flow", xmastree.get());
auto rainbow = Rovi::LEDEffectFactory::getEffect("rainbow", xmastree.get());

void setup()
{
  std::cout << "--- setup() ---" << endl;
  sleep(5);
  iot.begin();
  myRovi.setupRovi();

  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword"); // The prefedined variable OTAPassword is always reset to '1'; TODO: Check why/fix?
  std::cout << "*******************************************" << endl
         << "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
         << "*******************************************" << endl;

  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();


  // TODO: Init Neopixel
  // TODO: Move to dedicated component
  std::cout << "Init neopixel" << std::endl;
  xmastree->init();
  xmastree->setBrightness(255);
  xmastree->setColor(std::make_shared<Rovi::HSVColor>(128,0.0f,0.5f));
  Serial << "Start color flow" << endl;
  // xmastree->setEffect(colorFlow);
  xmastree->setEffect(rainbow);
}

uint8_t colorIdx = 0;

void loop()
{
    ArduinoOTA.handle();

    xmastree->update();

    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(128,0,0), 0);
    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(0,128,0), 1);
    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(0,0,128), 2);
    // xmastree->show();

    Serial << "." << endl;
    // Serial << "RED" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(128,0,0));
    // sleep(5);
    // Serial << "GREEN" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(0,128,0));
    // sleep(1);
    // Serial << "BLUE" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(0,0,128));
    // sleep(1);
    // Serial << "Colorflow" << endl;
    // auto colorFlow = LEDEffectFactory::getEffect("color_flow_fast", &nanoleaf);
    // nanoleaf.setEffect(colorFlow);
    // sleep(5);
}