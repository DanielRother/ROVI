#ifndef __LED_DEVICE_H__
#define __LED_DEVICE_H__

#include <chrono>
#include <iostream>
#include <memory>

#include "BasicDevice.hpp"
#include <Common/LED/Color/KnownColors.hpp>
#include <Common/LED/Effects/AllEffects.hpp>
#include <Common/LED/LEDEffectFactory.hpp>

namespace Rovi {
namespace Devices {
template <class LED> class LedDevice : public BasicDevice {
public:
  LedDevice(const std::shared_ptr<LED> led,
            const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects,
            const std::string &name = "led",
            std::chrono::minutes timePerEffect = std::chrono::minutes{15})
      : BasicDevice{name}, m_leds{led}, m_on{true}, m_brightness{128},
        m_color{std::make_shared<HSVColor>(0.0f, 1.0f, 0.5f)},
        m_color2{std::make_shared<HSVColor>(180.0f, 1.0f, 0.5f)},
        m_effect{LEDEffectFactory::getEffect("color_static", m_leds.get())},
        m_tmpEffectName{}, m_effects{effects}, m_timePerEffect{timePerEffect},
        m_nextEffectSelection{std::chrono::system_clock::now()} {
    std::cout << "Init LedDevice" << std::endl;
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

    // Don't forget to call the mother update here!
    BasicDevice::update();
  }

  virtual bool isOn() const { return m_on; }

  virtual void setOn(bool on) {
    std::cout << "LedDevice::setOn(" << on << ")" << std::endl;
    if (!on) {
      m_effect->stop();
    }
    m_leds->setOn(on);
    delay(10);
    m_leds->setOn(on);
    delay(10);
    m_leds->setOn(on);
    m_on = m_leds->isOn();
    // Always set this a last. Otherwise new values may be ignored (if
    // distributeSettings happens sometime in between)
    m_settingsChangedTimestamp = millis();
  }

  virtual uint8_t brightness() const { return m_brightness; }

  virtual void setBrightness(uint8_t brightness) {
    std::cout << "LedDevice::setBrightness(" << (int)brightness << ")"
              << std::endl;
    setOn(true);
    m_leds->setBrightness(brightness);
    // Not sure why this is required but otherwise the brightness is not set at
    // all when changing from 0 to a small percentage
    if (m_brightness >= 0) {
      m_leds->setBrightness(brightness);
    }
    m_brightness = m_leds->brightness();
    m_settingsChangedTimestamp = millis();
  }

  virtual std::shared_ptr<Color> color() {
    // m_color = m_leds->color();
    return m_color;
  }

  virtual std::shared_ptr<Color> color2() { return m_color2; }

  virtual void setColor(const std::shared_ptr<Color> &color) {
    std::cout << "LedDevice::setColor(" << color->toString() << ")"
              << std::endl;
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
    m_settingsChangedTimestamp = millis();
  }

  virtual void setColor2(const std::shared_ptr<Color> &color) {
    std::cout << "LedDevice::setColor2(" << color->toString() << ")"
              << std::endl;
    std::cout << "    finally realy set color" << std::endl;
    m_leds->setColor2(color);
    m_color2 = color;
    // setOn(true);
    m_settingsChangedTimestamp = millis();
  }

  virtual uint32_t hue() const { return m_leds->hue(); }

  virtual void setHue(uint32_t hue) {
    std::cout << "LedDevice::setHue(" << hue << ")" << std::endl;
    setOn(true);
    m_leds->setHue(hue);
    // m_color = m_leds->color();
    m_settingsChangedTimestamp = millis();
  }

  virtual std::shared_ptr<LEDEffect> effect() const { return m_effect; }

  // TBD: Adapt?
  virtual void setEffect(const std::shared_ptr<LEDEffect> &effect) {
    setOn(true);
    m_leds->setEffect(effect);
    m_effect = m_leds->effect();
    m_settingsChangedTimestamp = millis();
  }

  virtual void setEffect(int effect) {
    setOn(true);
    m_leds->setEffect(effect);
    m_effect = m_leds->effect();
    m_settingsChangedTimestamp = millis();
  }

  virtual void
  setEffects(const std::vector<std::shared_ptr<LEDEffect>> &effects) {
    setOn(true);
    m_effects = effects;
    selectRandomEffect();
    m_settingsChangedTimestamp = millis();
  }

  virtual std::vector<std::shared_ptr<LEDEffect>> getEffects() const {
    return m_effects;
  }

  virtual std::chrono::minutes timePerEffect() const { return m_timePerEffect; }

  virtual void setTimePerEffect(const std::chrono::minutes &timePerEffect) {
    m_timePerEffect = timePerEffect;
    m_nextEffectSelection = std::chrono::system_clock::now() + m_timePerEffect;
    m_settingsChangedTimestamp = millis();
  }

protected:
  virtual void settingsToJson(JsonObject &settings,
                              DynamicJsonBuffer &buffer) override {
    Serial << "LedDevice::settingsToJson() " << endl;

    settings["power"] = this->m_on;
    settings["brightness"] = this->m_brightness;
    settings["colorType"] = "rgb";
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

  virtual void jsonToSettings(JsonObject &settings) override {
    Serial << "LedDeviceMQTT::jsonToSettings() " << endl;

    std::cout << "Check brightness" << std::endl;
    uint8_t brightness = settings["brightness"];
    if (brightness) {
      std::cout << "New brightness value received" << std::endl;
      setBrightness(brightness);
    } else {
      std::cout << "NO brightness received" << std::endl;
    }

    std::cout << "Check colorType" << std::endl;
    auto colorType = restoreString(settings, "colorType");
    if (colorType != "") {
      std::cout << "New colorType value received" << std::endl;
      std::cout << "Check color" << std::endl;
      auto colorSettings = settings["color"];
      if (colorSettings && colorType == "rgb") {
        std::cout << "color and colorType == rgb" << std::endl;
        std::cout << "Check rgb" << std::endl;
        auto r = colorSettings["r"];
        auto g = colorSettings["g"];
        auto b = colorSettings["b"];
        if ((const char *)r && (const char *)g && (const char *)b) {
          std::cout << "all components included in message" << std::endl;
          auto color =
              std::make_shared<RGBColor>((uint8_t)r, (uint8_t)g, (uint8_t)b);
          setColor(color);
        }
      } else if (colorSettings && colorType == "hsv") {
        std::cout << "color and colorType == hsv" << std::endl;
        std::cout << "Check hsv" << std::endl;
        auto h = colorSettings["h"];
        auto s = colorSettings["s"];
        auto v = colorSettings["v"];
        if ((const char *)h && (const char *)s && (const char *)v) {
          std::cout << "all components included in message" << std::endl;
          auto color = std::make_shared<HSVColor>((float)h, (float)s, (float)v);
          setColor(color);
        }
      }

      std::cout << "Check color2" << std::endl;
      auto colorSettings2 = settings["color2"];
      if (colorSettings2 && colorType == "rgb") {
        std::cout << "color2 and colorType == rgb" << std::endl;
        std::cout << "Check rgb" << std::endl;
        auto r = colorSettings2["r"];
        auto g = colorSettings2["g"];
        auto b = colorSettings2["b"];
        if ((const char *)r && (const char *)g && (const char *)b) {
          std::cout << "all components included in message" << std::endl;
          auto color =
              std::make_shared<RGBColor>((uint8_t)r, (uint8_t)g, (uint8_t)b);
          setColor2(color);
        }
      } else if (colorSettings2 && colorType == "hsv") {
        std::cout << "color2 and colorType == hsv" << std::endl;
        std::cout << "Check hsv" << std::endl;
        auto h = colorSettings2["h"];
        auto s = colorSettings2["s"];
        auto v = colorSettings2["v"];
        if ((const char *)h && (const char *)s && (const char *)v) {
          std::cout << "all components included in message" << std::endl;
          auto color = std::make_shared<HSVColor>((float)h, (float)s, (float)v);
          setColor2(color);
        }
      }
    } else {
      std::cout << "NO colorType received" << std::endl;
    }

    std::cout << "Check effect" << std::endl;
    auto selectedEffect = restoreString(settings, "effect");
    for (auto effect : this->m_effects) {
      if (effect->name() == selectedEffect) {
        setEffect(effect);
        break;
      }
    }

    std::cout << "Check timePerEffect" << std::endl;
    int timePerEffect = settings["timePerEffect_m"];
    if (timePerEffect) {
      std::cout << "New timePerEffect value received" << std::endl;
      setTimePerEffect(std::chrono::minutes{timePerEffect});
    } else {
      std::cout << "NO timePerEffect received" << std::endl;
    }

    // Check power last as power == false should always turn the bulb off
    std::cout << "Check power" << std::endl;
    auto power = settings["power"];
    if ((const char *)power) {
      std::cout << "New power value received" << std::endl;
      setOn((bool)power);
    } else {
      std::cout << "NO power received" << std::endl;
    }

    std::cout << "****** Restored setting ******" << std::endl;
    std::cout << "p = " << this->m_on
              << ", b = " << (uint32_t)this->m_brightness
              << "; c = " << this->m_color->toString()
              << "; c2 = " << this->m_color2->toString()
              << "; e = " << this->m_effect->name()
              << "; t = " << this->m_timePerEffect.count() << std::endl;
    std::cout << "******************************" << std::endl;
  }

  virtual void getOptions(JsonObject &options,
                          DynamicJsonBuffer &jsonBuffer) override {
    JsonArray &possibleEffects = jsonBuffer.createArray();
    for (auto e : this->m_effects) {
      String effectS = e->name().c_str();
      possibleEffects.add(effectS);
    }
    options["possibleEffects"] = possibleEffects;
  }

  virtual void selectRandomEffect() {
    std::cout << "LedDevice::selectRandomEffect()" << std::endl;

    setOn(true);
    auto effectIndex = random(0, m_effects.size());
    setEffect(m_effects[effectIndex]);

    std::cout << "Effect " << (int)effectIndex << " selected" << std::endl;

    auto now = std::chrono::system_clock::now();
    m_nextEffectSelection = now + m_timePerEffect;
  }

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

#endif /* __LED_DEVICE_H__ */