#include "LEDComponent.hpp"

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>
#include <ThreadUtil.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "Common/LED/ColorTypes.h"
#include "Common/LED/LEDEffectFactory.hpp"
#include "Common/LED/Effects/AllEffects.hpp"

namespace Rovi {
    namespace Components {
        LEDComponent::LEDComponent(const std::string& name) 
        : m_on(true), m_brightness(128)
        , m_color(std::make_shared<HSVColor>(0,0,0.2))
        , m_effect(std::make_shared<WhiteStatic>(this)) {
        }

        // LEDComponent::LEDComponent(const LEDComponent& other)
        // : RoviComponent(other.name),
        //   m_on(other.on) {}

        LEDComponent::~LEDComponent() {
            std::cout << "LEDComponent dtor" << endl;
        }

        void LEDComponent::update() {
            m_effect->update();
        }

        bool LEDComponent::isOn() const {
            return m_on;
        }

        void LEDComponent::setOn(bool power) {
            Serial << "--- LEDComponent::setOn" << endl;

            if(power) {
                setColor(m_color);
                m_effect->start();
            } else {
                auto tmpLastColor = m_color;
                setColor(std::make_shared<RGBColor>(0,0,0));
                m_color = tmpLastColor;
                m_effect->stop();
            }

            m_on = power;
        };

        void LEDComponent::setOn(const std::string& payload) {
            std::cout << "  setPower() " << endl;

            // Convert to lower cases
            auto localPayload = payload;
            std::transform(begin(localPayload), end(localPayload), begin(localPayload),::tolower);

            bool power = false;
            if(payload == "on" || payload == "true" || payload == "1") {
                power = true;
            }

            setOn(power);
        }

        std::shared_ptr<Color> LEDComponent::color() const {
            return m_color;
        }

        void LEDComponent::setColor(const std::shared_ptr<Color>& color) {
                Serial << "--- LEDComponent::setColor" << endl;
                setOn(true);
                setColorImpl(color);
                m_color = color;
        }

        void LEDComponent::setColor(const std::string& payload) {
            Serial << "  setColor() " << endl;
            auto color = Color::createColor(payload);
            Serial << "    Color:      " << color->toString().c_str() << endl;
            // TBD: Still required?
            // m_brightness = static_cast<uint8_t>(std::min(std::max(m_color->toHSV()->v * 255.0f, 0.0f), 255.0f));
            // setBrightness(m_brightness);
            // Serial << "    Brightness: " << m_brightness << endl;
            setColor(color);
        }

        std::shared_ptr<LEDEffect> LEDComponent::effect() const {
            return m_effect;
        }

        void LEDComponent::setEffect(const std::shared_ptr<LEDEffect>& selectedEffect) {   
            setOn(true); 
            stopEffect();
            m_effect = selectedEffect;
            startEffect();
        }

        void LEDComponent::setEffect(const std::string& payload) {
            Serial << "  LEDComponent::setEffect() " << endl;

            // TODO: Use effect name
            auto effectNumber = atoi(payload.c_str());

            auto selectedEffect = LEDEffectFactory::getEffect(effectNumber, this);
            setEffect(selectedEffect);
        }

        void LEDComponent::startEffect() {
            m_effect->start();
        }

        void LEDComponent::stopEffect() {
            m_effect->stop();
        }

        uint8_t LEDComponent::brightness() const {
            return m_brightness;
        }

        void LEDComponent::setBrightness(uint8_t brightness) {
            Serial << "--- LEDComponent::setBrightness to " << (uint32_t) brightness << endl;
            setOn(true);
            setBrightnessImpl(brightness);
            m_brightness = brightness;
        }

        void LEDComponent::setBrightness(const std::string& payload) {
            Serial << "  setBrightness() " << endl;

            // uint8_t brightness = 255;
            // stoi is not supported by PlatformIO... :(
            // try {
            //     brightness = std::stoi(payload);
            // } catch(const std::invalid_argument e& e) {
            //     Serial << "  Payload (" << payload << ") could not be converted to uint8_t" << endl << e.what() << endl;
            // }

            // TODO: Error handling
            auto brightness = atoi(payload.c_str());
            setBrightness(brightness);

            // TBD: Still required?
            // auto color = m_color->toHSV();
            // color->v = (float) m_brightness / 255.0f;
            // m_color = color->toRGB();
            // setColor(color);     
        }
    }
}