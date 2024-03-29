#include "LEDComponent.hpp"

#include <Arduino.h>

#include <algorithm>
#include <string>
#include <thread>

#include <ArduinoIostream.hpp>
#include <ThreadUtil.hpp>

#include "Common/LED/ColorTypes.h"
#include "Common/LED/Effects/AllEffects.hpp"
#include "Common/LED/LEDEffectFactory.hpp"

#include <UtilFunctions.hpp>

namespace Rovi {
namespace Components {
LEDComponent::LEDComponent(const std::string &name)
    : m_on(true), m_brightness(128),
      m_color(std::make_shared<HSVColor>(0, 0, 0.2)),
      m_effect(std::make_shared<WhiteStatic>(this)) {
  std::cout << "LEDComponent ctor" << std::endl;
}

// LEDComponent::LEDComponent(const LEDComponent& other)
// : RoviComponent(other.name),
//   m_on(other.on) {}

LEDComponent::~LEDComponent() { std::cout << "LEDComponent dtor" << endl; }

void LEDComponent::update() { m_effect->update(); }

bool LEDComponent::isOn() const { return m_on; }

void LEDComponent::setOn(bool power) {
  Serial << "--- LEDComponent::setOn" << endl;

  if (power) {
    setColor(m_color);
    setColor1(m_color1);
    setColor2(m_color2);
    m_effect->start();
  } else {
    std::cout << "------ turn off" << std::endl;
    auto tmpLastColor = m_color;
    auto tmpLastColor1 = m_color1;
    auto tmpLastColor2 = m_color2;
    setColor(std::make_shared<RGBColor>(0, 0, 0));
    setColor1(std::make_shared<RGBColor>(0, 0, 0));
    setColor2(std::make_shared<RGBColor>(0, 0, 0));
    m_color = tmpLastColor;
    m_color1 = tmpLastColor1;
    m_color2 = tmpLastColor2;
    m_effect->stop();
  }

  m_on = power;
};

// void LEDComponent::setOn(const std::string& payload) {
//     std::cout << "  setPower() " << endl;

//     // Convert to lower cases
//     auto localPayload = payload;
//     std::transform(begin(localPayload), end(localPayload),
//     begin(localPayload),::tolower);

//     bool power = false;
//     if(payload == "on" || payload == "true" || payload == "1") {
//         power = true;
//     }

//     setOn(power);
// }

uint8_t LEDComponent::brightness() const { return m_brightness; }

void LEDComponent::setBrightness(uint8_t brightness) {
  Serial << "--- LEDComponent::setBrightness to " << (uint32_t)brightness
         << endl;
  setOn(true);
  auto limitedBrightness = Utils::clamp(
      brightness, (uint8_t)0, (uint8_t)100); // TBD: Move to Color class?
  Serial << "--- limitedBrightness " << (uint32_t)limitedBrightness << endl;
  setBrightnessImpl(limitedBrightness);
  m_brightness = limitedBrightness;
}

// void LEDComponent::setBrightness(const std::string& payload) {
//     Serial << "  setBrightness() " << endl;

//     // uint8_t brightness = 255;
//     // stoi is not supported by PlatformIO... :(
//     // try {
//     //     brightness = std::stoi(payload);
//     // } catch(const std::invalid_argument e& e) {
//     //     Serial << "  Payload (" << payload << ") could not be converted to
//     uint8_t" << endl << e.what() << endl;
//     // }

//     // TODO: Error handling
//     auto brightness = atoi(payload.c_str());
//     setBrightness(brightness);

//     // TBD: Still required?
//     // auto color = m_color->toHSV();
//     // color->v = (float) m_brightness / 255.0f;
//     // m_color = color->toRGB();
//     // setColor(color);
// }

std::shared_ptr<Color> LEDComponent::color() const {
  // std::cout << "--- LEDComponent::color(): " << m_color->toRGB()->toString()
  // << std::endl;
  return m_color;
}

std::shared_ptr<Color> LEDComponent::color1() const {
  // std::cout << "--- LEDComponent::color1(): " <<
  // m_color1->toRGB()->toString() << std::endl;
  return m_color1;
}

std::shared_ptr<Color> LEDComponent::color2() const {
  // std::cout << "--- LEDComponent::color2(): " <<
  // m_color2->toRGB()->toString() << std::endl;
  return m_color2;
}

void LEDComponent::setColor(const std::shared_ptr<Color> &color) {
  // std::cout << "--- LEDComponent::setColor() to " <<
  // color->toRGB()->toString() << std::endl;

  m_on = true;
  setColorImpl(color);
  m_color = color;
}

void LEDComponent::setColor1(const std::shared_ptr<Color> &color) {
  // std::cout << "--- LEDComponent::setColor1() to " <<
  // color->toRGB()->toString() << std::endl;

  m_on = true;
  // Not neccessary to set anything. Just store the value
  m_color1 = color;
}

void LEDComponent::setColor2(const std::shared_ptr<Color> &color) {
  // std::cout << "--- LEDComponent::setColor2() to " <<
  // color->toRGB()->toString() << std::endl;

  m_on = true;
  // Not neccessary to set anything. Just store the value
  m_color2 = color;
}

// void LEDComponent::setColor(const std::string& payload) {
//     Serial << "  setColor() " << endl;
//     auto color = Color::createColor(payload);
//     Serial << "    Color:      " << color->toString().c_str() << endl;
//     // TBD: Still required?
//     // m_brightness =
//     static_cast<uint8_t>(std::min(std::max(m_color->toHSV()->v * 255.0f,
//     0.0f), 255.0f));
//     // setBrightness(m_brightness);
//     // Serial << "    Brightness: " << m_brightness << endl;
//     setColor(color);
// }

uint32_t LEDComponent::hue() const { return m_color->toHSV()->h; }

void LEDComponent::setHue(uint32_t hue) {
  std::cout << "--- LEDComponent::setHue() to " << hue << std::endl;

  auto hsvColor = m_color->toHSV();
  hsvColor->h =
      Utils::clamp((float)hue, 0.0f, 360.0f); // TODO: Move to Color class
  std::cout << "--- Color should be set to " << hsvColor->toHSV()->toString()
            << ", i.e. " << hsvColor->toRGB()->toString() << std::endl;
  setColor(hsvColor);
  std::cout << "--- Color is now " << color()->toHSV()->toString() << std::endl;
}

// void LEDComponent::setHue(const std::string& payload) {
//     auto hue = atof(payload.c_str());
//     setHue(hue);
// }

std::shared_ptr<LEDEffect> LEDComponent::effect() const { return m_effect; }

void LEDComponent::setEffect(const std::shared_ptr<LEDEffect> &selectedEffect) {
  Serial << "  LEDComponent::setEffect() " << endl;
  setOn(true);
  stopEffect();
  m_effect = selectedEffect;
  startEffect();
}
void LEDComponent::setEffect(int effect) {
  Serial << "  LEDComponent::setEffect() " << endl;

  auto selectedEffect = LEDEffectFactory::getEffect(effect, this);
  setEffect(selectedEffect);
}

// void LEDComponent::setEffect(const std::string& payload) {
//     Serial << "  LEDComponent::setEffect() " << endl;

//     // TODO/TBD?: Use effect name?
//     auto effectNumber = atoi(payload.c_str());
//     setEffect(effectNumber);
// }

void LEDComponent::startEffect() {
  Serial << "  LEDComponent::startEffect() " << endl;
  m_effect->start();
}

void LEDComponent::stopEffect() {
  Serial << "  LEDComponent::stopEffect() " << endl;
  m_effect->stop();
}

bool LEDComponent::isAdressable() const { return false; }

size_t LEDComponent::nbPixel() const { return 1; }

void LEDComponent::setColor(const std::shared_ptr<Color> &color,
                            size_t pixelIdx) {
  if (!isAdressable()) {
    Serial << "Not adressable" << endl;
    return;
  }

  // Serial << "--- LEDComponent::setColor" << endl;
  m_on = true;
  setColorImpl(color, pixelIdx);
}

// void LEDComponent::setColor(const std::string& payload, size_t pixelIdx) {
//     Serial << "  setColor() " << endl;
//     auto color = Color::createColor(payload);
//     Serial << "    Color:      " << color->toString().c_str() << endl;
//     setColor(color, pixelIdx);
// }

std::shared_ptr<Color> LEDComponent::color(size_t pixelIdx) const {
  if (!isAdressable()) {
    return m_color;
  }

  return colorImpl(pixelIdx);
}

void LEDComponent::show() { showImpl(); }

AdressableLedComponent::AdressableLedComponent(size_t nbPixel,
                                               const std::string &name)
    : LEDComponent{name}, m_nbPixel{nbPixel}, m_colors{nbPixel},
      m_swapRGValues(nbPixel,
                     0) // TBD: Maybe make the Pixeltype dynamic as well
{
  std::cout << "AdressableLedComponent::ctor()" << std::endl;
  std::cout << "nbPixel = " << nbPixel << std::endl;
}

// AdressableLedComponent(const AdressableLedComponent& other) {};
AdressableLedComponent::~AdressableLedComponent() /*= default*/ {}

bool AdressableLedComponent::isAdressable() const { return true; }

size_t AdressableLedComponent::nbPixel() const { return m_nbPixel; }

void AdressableLedComponent::setColorImpl(const std::shared_ptr<Color> &color,
                                          size_t pixelIdx) {
  if (pixelIdx >= m_nbPixel) {
    Serial << "Invalid index" << endl;
    return;
  }

  // Serial << "--- AdressableLedComponent::setColor" << endl;
  m_on = true;
  m_colors[pixelIdx] = color;
}

std::shared_ptr<Color>
AdressableLedComponent::colorImpl(size_t pixelIdx) const {
  if (pixelIdx >= m_nbPixel) {
    Serial << "Invalid index" << endl;
    return nullptr;
  }

  return m_colors[pixelIdx];
}
} // namespace Components
} // namespace Rovi