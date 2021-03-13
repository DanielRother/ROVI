#ifndef __ABSOLUT_HUE_H__
#define __ABSOLUT_HUE_H__

#include <memory>

#include "Common/LED/ColorTypes.h"
#include "Components/Input/RotaryEncoderWithButton.hpp"
#include "LedDevice.hpp"

namespace Rovi {
namespace Devices {
template <class ROTARY, class LED> class AbsolutHue : public LedDevice<LED> {
public:
  AbsolutHue(const std::shared_ptr<ROTARY> rotary,
             const std::shared_ptr<LED> led,
             const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects,
             const std::string &name = "AbsolutHue",
             std::chrono::minutes timePerEffect = std::chrono::minutes{0})
      : LedDevice<LED>{led, effects, name, timePerEffect}, m_rotary{rotary} {
    // Attention: Initialize the rotary encoder first. Otherwise the
    // led-set-methods wil fail due to unint rotary states
    std::cout << "AbsolutHue(): Init rotary" << std::endl;
    setupNormal();
    setupClick();
    setupDoubleClick();
    setupHold();

    std::cout << "AbsolutHue(): Init led" << std::endl;
    // Attention: Do NOT init the leds here again. Otherwise the application
    // hangs..!

    // Do not forget the restore the settings here as the base class restore
    // doesn't work for this devices
    this->restoreSettings();
  }

  virtual void update() {
    m_rotary->update();
    LedDevice<LED>::update();
  }

  virtual void setBrightness(uint8_t brightness) {
    std::cout << "AbsolutHue()::setBrightness(" << (int)brightness << ")"
              << std::endl;
    LedDevice<LED>::setBrightness(brightness);
    m_rotary->setValue(Components::ButtonStates::NORMAL, this->m_brightness);
  }

  virtual void setColor(const std::shared_ptr<Color> &color) {
    std::cout << "AbsolutHue()::setColor(" << color->toString() << ")"
              << std::endl;
    LedDevice<LED>::setColor(color);
    m_rotary->setValue(Components::ButtonStates::DOUBLE_CLICKED,
                       this->m_color->toHSV()->h);
  }

  virtual void setHue(uint32_t hue) {
    std::cout << "AbsolutHue()::setHue(" << hue << ")" << std::endl;
    LedDevice<LED>::setHue(hue);
    m_rotary->setValue(Components::ButtonStates::DOUBLE_CLICKED,
                       this->m_color->toHSV()->h);
  }

  virtual void setEffect(const std::shared_ptr<LEDEffect> &effect) {
    std::cout << "AbsolutHue()::setEffect(" << effect->name() << ")"
              << std::endl;
    LedDevice<LED>::setEffect(effect);
    for (auto effectIdx = 0; effectIdx < this->m_effects.size(); ++effectIdx) {
      if (this->m_effects[effectIdx]->name() == effect->name()) {
        m_rotary->setValue(Components::ButtonStates::HOLDED, effectIdx);
        break;
      }
    }
  }

  virtual void setEffect(int effect) {
    std::cout << "AbsolutHue()::setEffect(" << effect << ")" << std::endl;
    LedDevice<LED>::setEffect(effect);
    m_rotary->setValue(Components::ButtonStates::HOLDED, effect);
  }

protected:
  virtual void settingsToJson(JsonObject &settings,
                              DynamicJsonBuffer &buffer) override {
    Serial << "AbsolutHue::settingsToJson() " << endl;

    JsonObject &led = buffer.createObject();
    LedDevice<LED>::settingsToJson(led, buffer);
    settings["led"] = led;

    JsonObject &rotary = buffer.createObject();
    rotarySettingsToJson(rotary, buffer);
    settings["rotary"] = rotary;
  }

  virtual void jsonToSettings(JsonObject &settings) override {
    Serial << "AbsolutHue::jsonToSettings() " << endl;

    // Restore rotary first. It's better to evaluate a possible power at last
    // TODO: Rotaray
    std::cout << "Check rotary" << std::endl;
    auto rotary = settings["rotary"];
    if (rotary) {
      jsonToRotarySettings(rotary);
    } else {
      std::cout << "NO rotary received" << std::endl;
    }

    auto led = settings["led"];
    std::cout << "Check led" << std::endl;
    if (led) {
      LedDevice<LED>::jsonToSettings(led);
    } else {
      std::cout << "NO led received" << std::endl;
    }

    std::cout << "****** Restored setting ******" << std::endl;
    // std::cout << "p = " << this->m_on
    //           << ", b = " << (uint32_t)this->m_brightness
    //           << "; c = " << this->m_color->toString()
    //           << "; c2 = " << this->m_color2->toString()
    //           << "; e = " << this->m_effect->name()
    //           << "; t = " << this->m_timePerEffect.count() << std::endl;
    std::cout << "******************************" << std::endl;
  }

  virtual void getOptions(JsonObject &options,
                          DynamicJsonBuffer &buffer) override {
    JsonObject &ledOptions = buffer.createObject();
    LedDevice<LED>::getOptions(ledOptions, buffer);
    options["led"] = ledOptions;

    JsonObject &rotaryOptions = buffer.createObject();
    getRotaryOptions(rotaryOptions, buffer);
    options["rotary"] = rotaryOptions;
  }

  void rotarySettingsToJson(JsonObject &settings, DynamicJsonBuffer &buffer) {
    auto buttonState = m_rotary->getCurrentButtonState();
    settings["buttonState"] = String{
        // Use String here and above. Otherwise the char* points to nirvana...
        buttonState._to_string()};
    settings["lastButtonStateUpdate_ms"] =
        m_rotary->getLastButtonStateUpdateMs();

    JsonArray &values = buffer.createArray();
    auto stateValues = m_rotary->getStateValues();
    for (const auto &state : stateValues) {
      JsonObject &value = buffer.createObject();
      value["state"] = String{state.first._to_string()};
      value["value"] = state.second.value();
      values.add(value);
    }
    settings["values"] = values;
  }

  void jsonToRotarySettings(JsonObject &settings) {
    std::cout << "AbsolutHue::jsonToRotarySettings()" << std::endl;
    std::cout << "Check values" << std::endl;
    String tmp;
    settings.prettyPrintTo(tmp);
    std::cout << "settings: " << tmp.c_str() << std::endl;

    JsonArray &values = settings["values"];
    for (auto &v : values) {
      auto state = this->restoreString(v, "state");
      auto value = v["value"];

      if (state != "" && value) {
        std::cout << "state: " << state << ", value: " << (int)value
                  << std::endl;
        m_rotary->setValue(
            Components::ButtonStates::_from_string(state.c_str()), value);
        this->m_settingsChangedTimestamp = millis();
      }
    }
  }

  void getRotaryOptions(JsonObject &options, DynamicJsonBuffer &buffer) {
    JsonArray &settings = buffer.createArray();
    auto stateSettings = m_rotary->getStateSettings();
    for (const auto &state : stateSettings) {
      JsonObject &setting = buffer.createObject();
      setting["state"] = String{state.first._to_string()};
      setting["increment"] = state.second.increment;
      setting["minValue"] = state.second.minValue;
      setting["maxValue"] = state.second.maxValue;
      setting["preventOverflow"] = state.second.preventOverflow;
      settings.add(setting);
    }
    options["settings"] = settings;
  }

  void setupNormal() {
    std::cout << "AbsolutHue()::setupNormal()" << std::endl;
    auto activatedCallback = [this]() {
      std::cout
          << "NORMAL state activation callback - Brightness selection active"
          << std::endl;
      if (this->m_on) {
        std::cout << "--- blink()" << std::endl;
        blink();
      } else {
        std::cout << "--- power off --> do nothing" << std::endl;
      }
    };

    auto valueChangeCallback = [this](int brightness) {
      std::cout
          << "NORMAL value change callback - Set brightness to new value = "
          << brightness << std::endl;
      this->setBrightness(brightness);
      std::cout << "--- brightness set to " << (uint32_t)this->m_brightness
                << std::endl;
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 100;
    const auto increment = 10;
    const auto preventOverflow = true;
    m_rotary->setupState(Components::ButtonStates::NORMAL, minRotaryValue,
                         maxRotaryValue, increment, preventOverflow,
                         activatedCallback, valueChangeCallback);
    m_rotary->setValue(Components::ButtonStates::NORMAL, this->m_brightness);
  }

  void setupClick() {
    std::cout << "AbsolutHue()::setupClick()" << std::endl;
    auto activatedCallback = [this]() {
      std::cout << "CLICKED state activation callback - Toggle power"
                << std::endl;
      this->setOn(!this->m_on);
    };

    auto valueChangeCallback = [&](int value) {
      std::cout << "CLICKED value change callback - Do nothing" << std::endl;
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 10;
    const auto increment = 1;
    const auto preventOverflow = false;
    m_rotary->setupState(Components::ButtonStates::CLICKED, minRotaryValue,
                         maxRotaryValue, increment, preventOverflow,
                         activatedCallback, valueChangeCallback);
  }

  void setupDoubleClick() {
    std::cout << "AbsolutHue()::setupDoubleClick()" << std::endl;
    auto activatedCallback = [this]() {
      std::cout
          << "DOUBLE_CLICKED state activation callback - Hue selection active"
          << std::endl;
      std::cout << "--- Set effect to color_static and double blink"
                << std::endl;

      for (auto effectIdx = 0; effectIdx < this->m_effects.size();
           ++effectIdx) {
        if (this->m_effects[effectIdx]->name() == "color_static") {
          this->setEffect(effectIdx);
          break;
        }
      }
      doubleBlink();
    };

    auto valueChangeCallback = [&](int hue) {
      std::cout
          << "DOUBLE_CLICKED value change callback - Set hue to new value = "
          << hue << std::endl;
      auto color = std::make_shared<HSVColor>(hue, 1.0f, 1.0f);
      this->setColor(color);
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = 360;
    const auto increment = 10;
    const auto preventOverflow = false;
    m_rotary->setupState(Components::ButtonStates::DOUBLE_CLICKED,
                         minRotaryValue, maxRotaryValue, increment,
                         preventOverflow, activatedCallback,
                         valueChangeCallback);
  }

  void setupHold() {
    std::cout << "AbsolutHue()::setupHold()" << std::endl;
    auto activatedCallback = [this]() {
      std::cout << "HOLDED state activation callback - Effect selectio  active"
                << std::endl;
      std::cout << "--- Stop current effect and do a long blink" << std::endl;
      this->m_leds->stopEffect();
      blink(500);
    };

    auto valueChangeCallback = [&](int effectNumber) {
      std::cout << "HOLDED value change callback - Select effect number "
                << effectNumber << ", i.e. effect '"
                << this->m_effects[effectNumber]->name() << "'" << std::endl;
      this->setEffect(effectNumber);
    };

    const auto minRotaryValue = 0;
    const auto maxRotaryValue = this->m_effects.size() - 1;
    const auto increment = 1;
    const auto preventOverflow = false;
    m_rotary->setupState(Components::ButtonStates::HOLDED, minRotaryValue,
                         maxRotaryValue, increment, preventOverflow,
                         activatedCallback, valueChangeCallback);
  }

  void blink(const uint32_t delay_ms = 250) {
    this->setOn(false);
    delay(delay_ms);
    this->setOn(true);
  }

  void doubleBlink(const uint32_t delay_ms = 250) {
    blink(delay_ms);
    delay(delay_ms);
    blink(delay_ms);
  }

  std::shared_ptr<ROTARY> m_rotary;
};
} // namespace Devices
} // namespace Rovi

#endif /* __ABSOLUT_HUE_H__ */