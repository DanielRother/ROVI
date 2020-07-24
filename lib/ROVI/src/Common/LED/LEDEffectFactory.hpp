#ifndef __LED_EFFECT_FACTORY__
#define __LED_EFFECT_FACTORY__

#include <string>
#include <vector>
#include <memory>

#include "Components/Output/LEDComponent.hpp"       // TBD: Required?
#include "LEDEffect.hpp"
#include "Effects/AllEffects.hpp"

namespace Rovi {
    class LEDEffectFactory {
    public:
        static std::shared_ptr<LEDEffect> getEffect(const std::string& selectedEffect, Components::LEDComponent* led);
        static std::shared_ptr<LEDEffect> getEffect(const int effectNumber, Components::LEDComponent* led);

        static std::shared_ptr<Rovi::ColorCircle> createColorCircle(const std::string& name, const std::vector<std::shared_ptr<Rovi::RGBColor>> colors, Components::LEDComponent* led, const int delay);

        static std::string convertEffectNumberToName(const int effectNumber);
        static int convertEffectNameToNumber(const std::string& effectName);

        static size_t getNumberOfEffects();

    protected:
        static uint32_t getDelay(const std::string& selectedEffect);

        static const std::vector<std::string> effectNames; 
        static const uint32_t DEFAULT_DELAY_MS;
    };
}

#endif /* __LED_EFFECT_FACTORY__ */