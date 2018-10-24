#include "LEDComponent.hpp"

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>
#include <ThreadUtil.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"
#include "LEDEffectFactory.hpp"
#include "Effects/AllEffects.hpp"

LEDComponent::LEDComponent(const std::string& name) 
: RoviComponent(name), lastColor(std::make_shared<HSVColor>(0,0,0.2)), effect(std::make_shared<WhiteStatic>(this)) {
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

LEDComponent::LEDComponent(const LEDComponent& other)
: RoviComponent(other.name), lastColor(other.lastColor) {}

LEDComponent::~LEDComponent() {
    Serial << "LEDComponent dtor" << endl;
}

// The convertion between string and concrete data type can be done in this abstract class
// But the actual action have to be implemented in an derived class
// Names due to overloaded function and bind make problems and would led to long typecasts...
void LEDComponent::setPowerMQTT(const std::string& payload) {
    Serial << "  setPower() " << endl;

    // Convert to lower cases
    std::string localPayload = payload;
    std::transform(localPayload.begin(), localPayload.end(), localPayload.begin(),::tolower);

    bool power = false;
    effect->stop();
    if(payload == "on" || payload == "true" || payload == "1") {
        power = true;
        effect->start();
    }

    setPower(power);
}

void LEDComponent::setColorMQTT(const std::string& payload) {
    Serial << "  setColor() " << endl;
    lastColor = Color::createColor(payload);

    // TODO: Reset brightness if this was changed internaly (e.g. by a rotary encoder)

    Serial << "    Color: " << lastColor->toString().c_str() << endl;

    setColor(lastColor);
}

void LEDComponent::setBrightnessMQTT(const std::string& payload) {
    Serial << "  setBrightness() " << endl;

    uint8_t brightness = 255;
    // stoi is not supported by PlatformIO... :(
    // try {
    //     brightness = std::stoi(payload);
    // } catch(const std::invalid_argument e& e) {
    //     Serial << "  Payload (" << payload << ") could not be converted to uint8_t" << endl << e.what() << endl;
    // }

    // TODO: Error handling
    brightness = atoi(payload.c_str());

    auto color = lastColor->toHSV();
    color->v = (float) brightness / 255.0f;
    lastColor = color->toRGB();

    setColor(color);     
}

void LEDComponent::setEffectMQTT(const std::string& payload) {
    Serial << "  LEDComponent::setEffect() " << endl;

    auto selectedEffect = LEDEffectFactory::getEffect(payload, this);   // TODO: Better use shared_ptr instead of this
    setEffect(selectedEffect);
}

void LEDComponent::startEffect() {
    effect->start();
}

void LEDComponent::stopEffect() {
    effect->stop();
}

void LEDComponent::update() {
    effect->update();
}

void LEDComponent::setEffect(std::shared_ptr<LEDEffect> selectedEffect) {    
    effect = selectedEffect;
}


std::shared_ptr<Color> LEDComponent::getLastColor() const {
    return lastColor;
}

bool LEDComponent::getPowerStatus() const {
    return powerStatus;
}