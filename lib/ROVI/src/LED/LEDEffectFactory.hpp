#ifndef __LED_EFFECT_FACTORY__
#define __LED_EFFECT_FACTORY__

#include <string>
#include <vector>
#include <memory>

#include "LEDComponent.hpp"
#include "LEDEffect.hpp"

class LEDEffectFactory {
public:
    static std::shared_ptr<LEDEffect> getEffect(const std::string& selectedEffect, LEDComponent* led);
    static std::shared_ptr<LEDEffect> getEffect(const uint32_t effectNumber, LEDComponent* led);

    static size_t getNumberOfEffects();

protected:
    static uint32_t getDelay(const std::string& selectedEffect);

    static const std::vector<std::string> effectNames; 
    static const uint32_t DEFAULT_DELAY_MS = 100;
};

#endif /* __LED_EFFECT_FACTORY__ */