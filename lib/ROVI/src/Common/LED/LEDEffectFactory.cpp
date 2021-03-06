#include "LEDEffectFactory.hpp"
#include "Effects/AllEffects.hpp"
#include "Common/LED/Color/KnownColors.hpp"
#include <FileIOUtils.hpp>

namespace Rovi {
    std::shared_ptr<LEDEffect> LEDEffectFactory::getEffect(const std::string& selectedEffect, Components::LEDComponent* led) {    // TODO: Use an shared_ptr also for LEDComponent
        std::cout << "   getEffect(" << selectedEffect << ")" << std::endl;
        
        // Convert string to lower
        auto effectLower = toLower(selectedEffect);
        auto delay_ms    = getDelay(effectLower);

        // TODO: Use enum instead of strings
        // TODO: currently the speed postfix is not working for rainbow and running_onoff,
        //       because there the names are not unique
        if(effectLower.find("white_static") != std::string::npos) {
            auto effect = std::make_shared<WhiteStatic>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_static") != std::string::npos) {
            auto effect = std::make_shared<ColorStatic>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_flow") != std::string::npos) {
            auto effect = std::make_shared<ColorFlow>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_random") != std::string::npos) {
            auto effect = std::make_shared<RandomColor>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("rainbow") != std::string::npos && effectLower.size() == 7) {
            auto effect = std::make_shared<Rainbow>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_circle_rgb") != std::string::npos) {
            auto effect = createColorCircle(effectLower, {
                        std::make_shared<Rovi::RGBColor>(128, 0, 0),
                        std::make_shared<Rovi::RGBColor>(0, 128, 0),
                        std::make_shared<Rovi::RGBColor>(0, 0, 128)
                        }, led, delay_ms * 5);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_circle_red") != std::string::npos) {
            auto effect = createColorCircle(effectLower, {
                        std::make_shared<Rovi::RGBColor>(Rovi::tuYellow),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuOrange),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuRed)
                        }, led, delay_ms * 5);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_circle_blue") != std::string::npos) {
            auto effect = createColorCircle(effectLower, {
                        std::make_shared<Rovi::RGBColor>(Rovi::tuLightBlue),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuBlue),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuDarkBlue)
                        }, led, delay_ms * 5);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_circle_green") != std::string::npos) {
            auto effect = createColorCircle(effectLower, {
                        std::make_shared<Rovi::RGBColor>(Rovi::tuLightGreen),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuGreen),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuDarkGreen)
                        }, led, delay_ms * 5);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_circle_purple") != std::string::npos) {
            auto effect = createColorCircle(effectLower, {
                        std::make_shared<Rovi::RGBColor>(Rovi::tuLightPurple),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuPurple),
                        std::make_shared<Rovi::RGBColor>(Rovi::tuDarkPurple)
                        }, led, delay_ms * 5);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_single") != std::string::npos && effectLower.size() == std::string{"running_onoff_single"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::SINGLE, RunningOnOff::PositionModes::LINEAR, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_multi") != std::string::npos && effectLower.size() == std::string{"running_onoff_multi"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::MULTI, RunningOnOff::PositionModes::LINEAR, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_twocolor") != std::string::npos && effectLower.size() == std::string{"running_onoff_twocolor"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::TWO_COLOR, RunningOnOff::PositionModes::LINEAR, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_rainbow") != std::string::npos && effectLower.size() == std::string{"running_onoff_rainbow"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::RAINBOW, RunningOnOff::PositionModes::LINEAR, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_single_random") != std::string::npos && effectLower.size() == std::string{"running_onoff_single_random"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::SINGLE, RunningOnOff::PositionModes::RANDOM, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_multi_random") != std::string::npos && effectLower.size() == std::string{"running_onoff_multi_random"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::MULTI, RunningOnOff::PositionModes::RANDOM, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower == "running_onoff_twocolor_zufall") {     // WTF: As soon as I change this to "*_random" it's doesn't found anymore...
            std::cout << "************ WTF: Why is this not found all the time ***************" << std::endl;
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::TWO_COLOR, RunningOnOff::PositionModes::RANDOM, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("running_onoff_rainbow_random") != std::string::npos && effectLower.size() == std::string{"running_onoff_rainbow_random"}.size()) {
            auto effect = std::make_shared<RunningOnOff>(RunningOnOff::ColorModes::RAINBOW, RunningOnOff::PositionModes::RANDOM, led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        } else if(effectLower.find("color_gradient") != std::string::npos && effectLower.size() == std::string{"color_gradient"}.size()) {
            auto effect = std::make_shared<ColorGradient>(led, delay_ms);
            effect->setName(effectLower);
            std::cout << "found effect: "  << effect->name() << std::endl;
            return effect;
        }

        Serial << "Warning: No such defect defined (" << effectLower << "). Using default (does nothing)." << endl;
        return std::make_shared<WhiteStatic>(led, delay_ms);
    }

    std::shared_ptr<Rovi::ColorCircle> LEDEffectFactory::createColorCircle(const std::string& name, const std::vector<std::shared_ptr<Rovi::RGBColor>> colors, Components::LEDComponent* led, const int delay) {
        std::cout << "createColorCircle() - colors: " << std::endl;
        for(auto color : colors) {
            std::cout << color->toString() << std::endl;
        }
        auto colorCircle = std::make_shared<Rovi::ColorCircle>(led, delay);
        colorCircle->setColors(colors);
        colorCircle->setName(name);
        return colorCircle;
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

        "rainbow",
        "color_circle"
    };

    const uint32_t LEDEffectFactory::DEFAULT_DELAY_MS = 100;
}