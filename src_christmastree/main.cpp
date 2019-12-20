#include <Arduino.h>

#include <ArduinoIostream.hpp>

#include <ArduinoOTA.h>

#include <chrono>

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
#include <Common/LED/Effects/AllEffects.hpp>
#include <Common/LED/Color/KnownColors.hpp>

Rovi::Deprecated::RoviDevice myRovi(iot);

const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

auto xmastree = std::make_shared<Rovi::Components::FastLedComponent<nbPixel, pin>>(name);

auto colorFlow = Rovi::LEDEffectFactory::getEffect("color_flow", xmastree.get());
auto rainbow = Rovi::LEDEffectFactory::getEffect("rainbow", xmastree.get());
auto colorCircleRGB = std::make_shared<Rovi::ColorCircle>(xmastree.get(), colorCircleDelay);
auto colorCircleRed = std::make_shared<Rovi::ColorCircle>(xmastree.get(), colorCircleDelay);
auto colorCircleBlue = std::make_shared<Rovi::ColorCircle>(xmastree.get(), colorCircleDelay);
auto colorCircleGreen = std::make_shared<Rovi::ColorCircle>(xmastree.get(), colorCircleDelay);
auto colorCirclePurple = std::make_shared<Rovi::ColorCircle>(xmastree.get(), colorCircleDelay);

std::vector<std::shared_ptr<Rovi::LEDEffect>> effects;
auto timePerEffect = std::chrono::minutes(15);
std::chrono::time_point<std::chrono::system_clock> nextEffectSelection;

void selectRandomEffect() {
  Serial << "Random effect" << endl;

  auto effectIndex = random(0, effects.size());
  xmastree->setEffect(effects[effectIndex]);

  Serial << "Effect " << (int) effectIndex << " selected" << endl;

  auto now = std::chrono::system_clock::now();
  nextEffectSelection = now + timePerEffect;
}

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

  Serial << "Init efects" << endl;
  effects.push_back(colorFlow);
  effects.push_back(rainbow);

  {
    auto colorsRGB = std::vector<std::shared_ptr<Rovi::RGBColor>>();
    colorsRGB.emplace_back(std::make_shared<Rovi::RGBColor>(128, 0, 0));
    colorsRGB.emplace_back(std::make_shared<Rovi::RGBColor>(0, 128, 0));
    colorsRGB.emplace_back(std::make_shared<Rovi::RGBColor>(0, 0, 128));
    colorCircleRGB->setColors(colorsRGB);
    effects.push_back(colorCircleRGB);
  }

  {
    auto colorsRed = std::vector<std::shared_ptr<Rovi::RGBColor>>();
    colorsRed.push_back(Rovi::tuYellow);
    colorsRed.push_back(Rovi::tuOrange);
    colorsRed.push_back(Rovi::tuRed);
    colorCircleRed->setColors(colorsRed);
    effects.push_back(colorCircleRed);
  }  

  {
    auto colorsBlue = std::vector<std::shared_ptr<Rovi::RGBColor>>();
    colorsBlue.push_back(Rovi::tuLightBlue);
    colorsBlue.push_back(Rovi::tuBlue);
    colorsBlue.push_back(Rovi::tuDarkBlue);
    colorCircleBlue->setColors(colorsBlue);
    effects.push_back(colorCircleBlue);
  }  

  {
    auto colorsGreen = std::vector<std::shared_ptr<Rovi::RGBColor>>();
    colorsGreen.push_back(Rovi::tuLightGreen);
    colorsGreen.push_back(Rovi::tuGreen);
    colorsGreen.push_back(Rovi::tuDarkGreen);
    colorCircleGreen->setColors(colorsGreen);
    effects.push_back(colorCircleGreen);
  }  

  {
    auto colorsPurple = std::vector<std::shared_ptr<Rovi::RGBColor>>();
    colorsPurple.push_back(Rovi::tuLightPurple);
    colorsPurple.push_back(Rovi::tuPurple);
    colorsPurple.push_back(Rovi::tuDarkPurple);
    colorCirclePurple->setColors(colorsPurple);
    effects.push_back(colorCirclePurple);
  }  

  selectRandomEffect();
}

void loop()
{
    ArduinoOTA.handle();

    xmastree->update();

    auto now = std::chrono::system_clock::now();
    if(now > nextEffectSelection) {
      selectRandomEffect();
    }

    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(128,0,0), 0);
    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(0,128,0), 1);
    // xmastree->setColor(std::make_shared<Rovi::RGBColor>(0,0,128), 2);
    // xmastree->show();

    // Serial << "." << endl;
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