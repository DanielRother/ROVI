#include "LEDEffectFactory.hpp"
#include "Effects/AllEffects.hpp"
#include <FileIOUtils.hpp>

namespace Rovi {
    std::shared_ptr<LEDEffect> LEDEffectFactory::getEffect(const std::string& selectedEffect, Components::LEDComponent* led) {    // TODO: Use an shared_ptr also for LEDComponent
        Serial << "   getEffect(" << selectedEffect << ")" << endl;
        
        // Convert string to lower
        auto effectLower = toLower(selectedEffect);
        auto delay_ms    = getDelay(effectLower);

        if(effectLower.find("white_static") != std::string::npos) {
            return std::make_shared<WhiteStatic>(led, delay_ms);
        } else if(effectLower.find("color_static") != std::string::npos) {
            return std::make_shared<ColorStatic>(led, delay_ms);
        } else if(effectLower.find("color_flow") != std::string::npos) {
            return std::make_shared<ColorFlow>(led, delay_ms);
        } else if(effectLower.find("color_random") != std::string::npos) {
            return std::make_shared<RandomColor>(led, delay_ms);
        } else if(effectLower.find("rainbow") != std::string::npos) {
            return std::make_shared<Rainbow>(led, delay_ms);
        }

        Serial << "Warning: No such defect defined (" << effectLower << "). Using default (does nothing)." << endl;
        return std::make_shared<WhiteStatic>(led, delay_ms);
    }

    std::shared_ptr<LEDEffect> LEDEffectFactory::getEffect(const int effectNumber, Components::LEDComponent* led) {    // TODO: Use an shared_ptr also for LEDComponent
        Serial << "   getEffect(" << effectNumber << ")" << endl;
        
        auto effectName = convertEffectNumberToName(effectNumber);
        return getEffect(effectName, led);
    }

    size_t LEDEffectFactory::getNumberOfEffects() {
        return effectNames.size();
    }

    std::string LEDEffectFactory::convertEffectNumberToName(const int effectNumber) {
        auto name = std::string{""};
        if(effectNumber >= 0 && effectNumber < getNumberOfEffects()) {
            name = effectNames[effectNumber];
        }

        return name;
    }

    int LEDEffectFactory::convertEffectNameToNumber(const std::string& effectName) {
        // TODO: Test me
        auto effectNumber = -1;
        auto effectIter = std::find(begin(effectNames), end(effectNames), effectName);
        if(effectIter != end(effectNames)) {
            effectNumber = std::distance(begin(effectNames), effectIter);
        }

        return effectNumber;
    }

    uint32_t LEDEffectFactory::getDelay(const std::string& selectedEffect) {
        uint32_t delay_ms = DEFAULT_DELAY_MS;
        if(selectedEffect.find("slow") != std::string::npos) {
            delay_ms *= 10;
        } else if(selectedEffect.find("fast") != std::string::npos) {
            delay_ms /= 10;
        }

        return delay_ms;
    }

    const std::vector<std::string> LEDEffectFactory::effectNames {
        "white_static",
        "color_static",
        "color_flow",
        "color_random",
        
        "color_flow_fast",
        "color_random_fast",

        "color_flow_slow",
        "color_random_slow",

        "rainbow"
    };

    const uint32_t LEDEffectFactory::DEFAULT_DELAY_MS = 100;
}