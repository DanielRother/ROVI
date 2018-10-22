#ifndef __LEDEFFECT_RANDOMCOLOR__
#define __LEDEFFECT_RANDOMCOLOR__

#include "../LEDEffect.hpp"

#include <chrono>
#include <random>

class RandomColor : public LEDEffect {
public:
    enum class EffectState {
        INIT,
        FADE_IN_1,
        FADE_IN_2,
        FADE_OUT_1,
        FADE_OUT_2
    };

    RandomColor(LEDComponent* led, uint32_t delay_ms = 100)
        : LEDEffect(led, delay_ms), 
        generator(std::chrono::system_clock::now().time_since_epoch().count()), distribution(0,360),
        currentColor(0), state(EffectState::INIT), currentSaturation(0.0f)
        {}

    void step() {
        // Get current brightness
        auto currentBrightness = getCurrentBrightness();
        led->setColor(std::make_shared<HSVColor>(currentColor, currentSaturation, currentBrightness));
    
        switch (state) {
            case EffectState::INIT:
                currentColor = distribution(generator);
                state = EffectState::FADE_IN_1;

                Serial << "RandomColor" << endl;
                Serial << "Color: " << currentColor << endl;
                Serial << "brightness: " << currentBrightness << endl;
                break;
            case EffectState::FADE_IN_1:
                currentSaturation += 0.01;
                if(currentSaturation >= 0.5f) {
                    state = EffectState::FADE_IN_2;
                }
                break;
            case EffectState::FADE_IN_2:
                currentSaturation += 0.005;
                if(currentSaturation >= 1.0f) {
                    state = EffectState::FADE_OUT_1;
                }
                break;
            case EffectState::FADE_OUT_1:
                currentSaturation -= 0.005;
                if(currentSaturation <= 0.5f) {
                    state = EffectState::FADE_OUT_2;
                }
                break;
            case EffectState::FADE_OUT_2:
                currentSaturation -= 0.01;
                if(currentSaturation <= 0.0f) {
                    state = EffectState::INIT;
                }
                break;
            default:
                break;
        }     
    }

protected:
    std::default_random_engine          generator;
    std::uniform_int_distribution<int>  distribution;

    int currentColor;
    EffectState state;
    float currentSaturation;
};


#endif /* __LEDEFFECT_RANDOMCOLOR__ */