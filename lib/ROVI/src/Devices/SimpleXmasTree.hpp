#ifndef __SIMPLE_XMAS_TREE_H__
#define __SIMPLE_XMAS_TREE_H__

#include <memory>
#include <iostream>

#include <Common/LED/LEDEffectFactory.hpp>
#include <Common/LED/Effects/AllEffects.hpp>

#include "SimpleLedDevice.hpp"

namespace Rovi {
    namespace Devices {
        template<class LED>
        class SimpleXmasTree : public SimpleLedDevice<LED> {
            public:
                SimpleXmasTree(const std::shared_ptr<LED> led, const std::string& name = "xmastree", const int colorCircleDelay = 500)
                : SimpleLedDevice<LED>(led, name)
                , m_colorCircleDelay(colorCircleDelay)
                {
                    std::cout << "Warning: colorCircleDelay not used yet" << std::endl;
                }

                virtual void setupEffects() override {
                    Serial << "Init efects" << endl;
                    this->m_effects.clear();
                    // this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", this->m_leds.get()));
                    // this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", this->m_leds.get()));
                    // this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", this->m_leds.get()));
                    this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("rainbow", this->m_leds.get()));
                    this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_red", this->m_leds.get()));
                    this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_blue", this->m_leds.get()));
                    this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_green", this->m_leds.get()));
                    this->m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_purple", this->m_leds.get()));
                    this->selectRandomEffect();
                }

            protected:
                int m_colorCircleDelay;
                
        };
    };
};

#endif /* __SIMPLE_XMAS_TREE_H__ */