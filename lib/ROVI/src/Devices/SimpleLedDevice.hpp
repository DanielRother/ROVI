#ifndef __SIMPLE_LED_DEVICE_H__
#define __SIMPLE_LED_DEVICE_H__

#include <chrono>
#include <iostream>
#include <memory>

#include "BasicDevice.hpp"
#include <Common/LED/Color/KnownColors.hpp>
#include <Common/LED/Effects/AllEffects.hpp>
#include <Common/LED/LEDEffectFactory.hpp>

namespace Rovi {
namespace Devices {
template <class LED> class SimpleLedDevice : virtual BasicDevice {
public:
  SimpleLedDevice(const std::shared_ptr<LED> led,
                  const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects,
                  const std::string &name = "led",
                  std::chrono::minutes timePerEffect = std::chrono::minutes{15})
      : BasicDevice{}, m_name{name}, m_leds{led}, m_on{true}, m_brightness{128},
        m_color{std::make_shared<HSVColor>(0.0f, 1.0f, 0.5f)},
        m_color2{std::make_shared<HSVColor>(180.0f, 1.0f, 0.5f)},
        m_effect{LEDEffectFactory::getEffect("color_static", m_leds.get())},
        m_tmpEffectName{}, m_effects{effects}, m_timePerEffect{timePerEffect},
        m_nextEffectSelection{std::chrono::system_clock::now()} {
    std::cout << "Init led" << std::endl;
    m_leds->init();
    restoreSettings();
  }

  virtual void update() {
    m_leds->update();

    auto now = std::chrono::system_clock::now();
    if (now > m_nextEffectSelection &&
        m_timePerEffect != std::chrono::minutes(0) && m_on) {
      selectRandomEffect();
    }

    BasicDevice::update();
  }

  virtual bool isOn() const { return m_on; }

  virtual void setOn(bool on) {
    std::cout << "SimpleLedDevice::setOn(" << on << ")" << std::endl;
    m_settingsChanged = true;
    if (!on) {
      m_effect->stop();
    }
    m_leds->setOn(on);
    delay(10);
    m_leds->setOn(on);
    delay(10);
    m_leds->setOn(on);
    m_on = m_leds->isOn();
  }

  virtual uint8_t brightness() const { return m_brightness; }

  virtual void setBrightness(uint8_t brightness) {
    std::cout << "SimpleLedDevice::setBrightness(" << (int)brightness << ")"
              << std::endl;
    m_settingsChanged = true;
    setOn(true);
    m_leds->setBrightness(brightness);
    // Not sure why this is required but otherwise the brightness is not set at
    // all when changing from 0 to a small percentage
    if (m_brightness >= 0) {
      m_leds->setBrightness(brightness);
    }
    m_brightness = m_leds->brightness();
  }

  virtual std::shared_ptr<Color> color() {
    // m_color = m_leds->color();
    return m_color;
  }

  virtual std::shared_ptr<Color> color2() { return m_color2; }

  virtual void setColor(const std::shared_ptr<Color> &color) {
    std::cout << "SimpleLedDevice::setColor(" << color->toString() << ")"
              << std::endl;
    m_settingsChanged = true;

    // m_effect->stop();

    // auto selectedEffect = std::string{"color_static"};
    // bool effectFound = false;
    // for(auto effect : this->m_effects) {
    //     if(effect->name() == selectedEffect) {
    //         effectFound = true;
    //         setEffect(effect);
    //         break;
    //     }
    // }
    // if(!effectFound) {
    //     auto effect = LEDEffectFactory::getEffect(selectedEffect,
    //     m_leds.get()); setEffect(effect);
    //     // Delay processing, otherwise the effect will update to really set
    //     color to the effect's default one... sleep(1); effect->update();
    // }

    std::cout << "    finally realy set color" << std::endl;
    m_leds->setColor1(color);
    m_color = color;
  }

  virtual void setColor2(const std::shared_ptr<Color> &color) {
    std::cout << "SimpleLedDevice::setColor2(" << color->toString() << ")"
              << std::endl;
    m_settingsChanged = true;

    std::cout << "    finally realy set color" << std::endl;
    m_leds->setColor2(color);
    m_color2 = color;
    // setOn(true);
  }

  virtual uint32_t hue() const { return m_leds->hue(); }

  virtual void setHue(uint32_t hue) {
    std::cout << "SimpleLedDevice::setHue(" << hue << ")" << std::endl;
    m_settingsChanged = true;
    setOn(true);
    m_leds->setHue(hue);
    // m_color = m_leds->color();
  }

  virtual std::shared_ptr<LEDEffect> effect() const { return m_effect; }

  // TBD: Adapt?
  virtual void setEffect(const std::shared_ptr<LEDEffect> &effect) {
    m_settingsChanged = true;
    setOn(true);
    m_leds->setEffect(effect);
    m_effect = m_leds->effect();
  }

  virtual void setEffect(int effect) {
    m_settingsChanged = true;
    setOn(true);
    m_leds->setEffect(effect);
    m_effect = m_leds->effect();
  }

  virtual void
  setEffects(const std::vector<std::shared_ptr<LEDEffect>> &effects) {
    m_settingsChanged = true;
    setOn(true);
    m_effects = effects;
    selectRandomEffect();
  }

  virtual std::vector<std::shared_ptr<LEDEffect>> getEffects() const {
    return m_effects;
  }

  virtual std::chrono::minutes timePerEffect() const { return m_timePerEffect; }

  virtual void setTimePerEffect(const std::chrono::minutes &timePerEffect) {
    m_settingsChanged = true;
    m_timePerEffect = timePerEffect;
    m_nextEffectSelection = std::chrono::system_clock::now() + m_timePerEffect;
  }

protected:
  virtual void selectRandomEffect() {
    std::cout << "SimpleLedDevice::selectRandomEffect()" << std::endl;

    setOn(true);
    auto effectIndex = random(0, m_effects.size());
    setEffect(m_effects[effectIndex]);

    std::cout << "Effect " << (int)effectIndex << " selected" << std::endl;

    auto now = std::chrono::system_clock::now();
    m_nextEffectSelection = now + m_timePerEffect;
  }

  virtual void saveSettingsImpl(JsonObject &settings,
                                DynamicJsonBuffer &buffer) override {
    Serial << "SimpleLedDevice::saveSettings() " << endl;

    settings["power"] = this->m_on;
    settings["brightness"] = this->m_brightness;
    JsonObject &color = buffer.createObject();
    auto curRgbColor = this->color()->toRGB();
    color["r"] = curRgbColor->r;
    color["g"] = curRgbColor->g;
    color["b"] = curRgbColor->b;
    settings["color"] = color;
    JsonObject &color2 = buffer.createObject();
    auto curRgbColor2 = this->color2()->toRGB();
    color2["r"] = curRgbColor2->r;
    color2["g"] = curRgbColor2->g;
    color2["b"] = curRgbColor2->b;
    settings["color2"] = color2;
    m_tmpEffectName =
        this->m_effect->name(); // Must be a not temporary local copy. Otherwise
                                // the char* points to nirvana...
    settings["effect"] = m_tmpEffectName.c_str();
    settings["timePerEffect_m"] = (int)this->m_timePerEffect.count();
  }

  virtual void restoreSettingsImpl(JsonObject &settings) override {
    Serial << "LedDeviceMQTT::restoreSettings() " << endl;

    uint8_t brightness = settings["brightness"];
    setBrightness(brightness);
    auto colorSettings = settings["color"];
    uint8_t r = colorSettings["r"];
    uint8_t g = colorSettings["g"];
    uint8_t b = colorSettings["b"];
    auto color = std::make_shared<RGBColor>(r, g, b);
    setColor(color);
    auto colorSettings2 = settings["color2"];
    uint8_t r2 = colorSettings2["r"];
    uint8_t g2 = colorSettings2["g"];
    uint8_t b2 = colorSettings2["b"];
    auto color2 = std::make_shared<RGBColor>(r2, g2, b2);
    setColor2(color2);
    auto selectedEffect = restoreString(settings, "effect");
    for (auto effect : this->m_effects) {
      if (effect->name() == selectedEffect) {
        setEffect(effect);
        break;
      }
    }
    int timePerEffect = settings["timePerEffect_m"];
    setTimePerEffect(std::chrono::minutes{timePerEffect});
    // Check power last as power == false should always turn the bulb off
    bool power = settings["power"];
    setOn(power);

    std::cout << "****** Restored setting ******" << std::endl;
    std::cout << "p = " << this->m_on
              << ", b = " << (uint32_t)this->m_brightness
              << "; c = " << this->m_color->toString()
              << "; c2 = " << this->m_color2->toString()
              << "; e = " << this->m_effect->name()
              << "; t = " << this->m_timePerEffect.count() << std::endl;
    std::cout << "******************************" << std::endl;
  }

  std::string m_name;
  std::shared_ptr<LED> m_leds;

  bool m_on;
  uint8_t m_brightness;
  std::shared_ptr<Color> m_color;
  std::shared_ptr<Color> m_color2;
  std::shared_ptr<LEDEffect> m_effect;
  std::string m_tmpEffectName;

  std::vector<std::shared_ptr<Rovi::LEDEffect>> m_effects;
  std::chrono::minutes m_timePerEffect;
  std::chrono::time_point<std::chrono::system_clock> m_nextEffectSelection;
};
}; // namespace Devices
}; // namespace Rovi

#endif /* __SIMPLE_LED_DEVICE_H__ */