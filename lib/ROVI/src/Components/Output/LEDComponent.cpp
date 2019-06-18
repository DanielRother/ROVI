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
        : RoviComponent(name), 
          m_on(true), m_brightness(128), m_color(std::make_shared<HSVColor>(0,0,0.2)), m_effect(std::make_shared<WhiteStatic>(this)) {
          
            // TODO: (Re-)move
            // Always stop a possibly running effect thread first
            handler[std::string("setPower")]        = [this](const std::string payload){
                setPowerMQTT(payload);};
            handler[std::string("setColor")]        = [this](const std::string payload){
                stopEffect();
                setColorMQTT(payload);};
            handler[std::string("setBrightness")]   = [this](const std::string payload){
                setBrightnessMQTT(payload);};
            handler[std::string("setEffect")]       = [this](const std::string payload){
                setEffectMQTT(payload);};
        }

        // LEDComponent::LEDComponent(const LEDComponent& other)
        // : RoviComponent(other.name),
        //   m_on(other.on) {}

        LEDComponent::~LEDComponent() {
            std::cout << "LEDComponent dtor" << endl;
        }

        // The convertion between string and concrete data type can be done in this abstract class
        // But the actual action have to be implemented in an derived class
        // Names due to overloaded function and bind make problems and would led to long typecasts...
        void LEDComponent::setPowerMQTT(const std::string& payload) {
            std::cout << "  setPower() " << endl;

            // Convert to lower cases
            std::string localPayload = payload;
            std::transform(localPayload.begin(), localPayload.end(), localPayload.begin(),::tolower);

            bool power = false;
            m_effect->stop();
            if(payload == "on" || payload == "true" || payload == "1") {
                power = true;
                m_effect->start();
            }

            setOn(power);
        }

        void LEDComponent::setColorMQTT(const std::string& payload) {
            Serial << "  setColor() " << endl;
            m_color = Color::createColor(payload);

            m_brightness = static_cast<uint8_t>(std::min(std::max(m_color->toHSV()->v * 255.0f, 0.0f), 255.0f));
            setBrightness(m_brightness);

            setColor(m_color);

            Serial << "    Color:      " << m_color->toString().c_str() << endl;
            Serial << "    Brightness: " << m_brightness << endl;

        }

        void LEDComponent::setBrightnessMQTT(const std::string& payload) {
            Serial << "  setBrightness() " << endl;

            // uint8_t brightness = 255;
            // stoi is not supported by PlatformIO... :(
            // try {
            //     brightness = std::stoi(payload);
            // } catch(const std::invalid_argument e& e) {
            //     Serial << "  Payload (" << payload << ") could not be converted to uint8_t" << endl << e.what() << endl;
            // }

            // TODO: Error handling
            m_brightness = atoi(payload.c_str());

            auto color = m_color->toHSV();
            color->v = (float) m_brightness / 255.0f;
            m_color = color->toRGB();

            setColor(color);     
        }

        void LEDComponent::setEffectMQTT(const std::string& payload) {
            Serial << "  LEDComponent::setEffect() " << endl;

            auto effectNumber = atoi(payload.c_str());

            auto selectedEffect = LEDEffectFactory::getEffect(effectNumber, this);   // TODO: Better use shared_ptr instead of this
            setEffect(selectedEffect);
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
            } else {
                auto tmpLastColor = m_color;
                setColor(std::make_shared<RGBColor>(0,0,0));
                m_color = tmpLastColor;
            }

            m_on = power;
        };

        std::shared_ptr<Color> LEDComponent::color() const {
            return m_color;
        }

        void LEDComponent::setColor(const std::shared_ptr<Color>& color) {
                Serial << "--- LEDComponent::setColor" << endl;
                m_on = true;
                setColorImpl(color);
                m_color = color;
        }

        std::shared_ptr<LEDEffect> LEDComponent::effect() const {
            return m_effect;
        }

        void LEDComponent::setEffect(std::shared_ptr<LEDEffect> selectedEffect) {    
            m_effect = selectedEffect;
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
            m_on = true;
            setBrightnessImpl(brightness);
        }
    }
}