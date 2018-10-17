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

LEDComponent::LEDComponent(const std::string& name) 
: RoviComponent(name), lastColor(std::make_shared<RGBColor>(128,128,128)), effect(std::make_shared<LEDEffect>(this)) {
    // Always stop a possibly running effect thread first
    handler[std::string("setPower")]        = [this](const std::string payload){
        stopEffect();
        setPowerMQTT(payload);};
    handler[std::string("setColor")]        = [this](const std::string payload){
        stopEffect();
        setColorMQTT(payload);};
    handler[std::string("setBrightness")]   = [this](const std::string payload){
        stopEffect();
        setBrightnessMQTT(payload);};
    handler[std::string("setEffect")]       = [this](const std::string payload){
        stopEffect();
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
    stopEffect();           // <- TODO: in Handler einbauen...

    Serial << "  setPower() " << endl;

    // Convert to lower cases
    std::string localPayload = payload;
    std::transform(localPayload.begin(), localPayload.end(), localPayload.begin(),::tolower);

    bool power = false;
    if(payload == "on" || payload == "true" || payload == "1") {
        power = true;
    }

    setPower(power);
}

void LEDComponent::setColorMQTT(const std::string& payload) {
    stopEffect();           // <- TODO: in Handler einbauen...

    Serial << "  setColor() " << endl;
    lastColor = Color::createColor(payload);

    // TODO: Reset brightness if this was changed internaly (e.g. by a rotary encoder)

    Serial << "    Color: " << lastColor->toString().c_str() << endl;

    setColor(lastColor);
}

void LEDComponent::setBrightnessMQTT(const std::string& payload) {
    stopEffect();           // <- TODO: in Handler einbauen...

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
    stopEffect();           // <- TODO: in Handler einbauen...

    Serial << "  LEDComponent::setEffect() " << endl;

    auto selectedEffect = LEDEffectFactory::getEffect(payload, this);   // TODO: Better use shared_ptr instead of this
    setEffect(selectedEffect);
}

void LEDComponent::stopEffect() {

    std::cout << "Asking Task to Stop" << std::endl;
    // Stop the Task
    effect->stop();
    std::cout << "Asking Thread to Join" << std::endl;

    //Waiting for thread to join
    if(t.joinable()) {
        t.join();
    }
    std::cout << "Thread Joined" << std::endl;

    effect = std::make_shared<LEDEffect>(this);         // <- Otherwise effect->stop() crashes the next time this method is called
                                                        // TODO: Find a better solution
}

void LEDComponent::setEffect(std::shared_ptr<LEDEffect> selectedEffect) {    
    stopEffect();
    effect = selectedEffect;

    //Creating a thread to execute our task
    t = std::thread([&]()
    {
        effect->run();
    });
}


std::shared_ptr<Color> LEDComponent::getLastColor() const {
    Serial << "--- LEDComponent::getLastColor(): " << lastColor->toString() << endl;

    return lastColor;
}

bool LEDComponent::getPowerStatus() const {
    return powerStatus;
}