#include "LEDComponent.hpp"

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"
#include "LED/Effects/AllEffects.hpp"

#include <ThreadUtil.hpp>


LEDComponent::LEDComponent(const std::string& name) 
: RoviComponent(name), m_rgb(std::make_shared<RGBColor>(128,128,128)), effect(std::make_shared<LEDEffect>(this)) {
    // Always stop a possibly running effect thread first
    handler[std::string("setPower")]        = [this](const std::string payload){
        stopThread();
        setPowerMQTT(payload);};
    handler[std::string("setColor")]        = [this](const std::string payload){
        stopThread();
        setColorMQTT(payload);};
    handler[std::string("setBrightness")]   = [this](const std::string payload){
        stopThread();
        setBrightnessMQTT(payload);};
    handler[std::string("setEffect")]       = [this](const std::string payload){
        stopThread();
        setEffectMQTT(payload);};
}

LEDComponent::LEDComponent(const LEDComponent& other)
: RoviComponent(other.name), m_rgb(other.m_rgb) {}

LEDComponent::~LEDComponent() {
    Serial << "LEDComponent dtor" << endl;
}

// The convertion between string and concrete data type can be done in this abstract class
// But the actual action have to be implemented in an derived class
// Names due to overloaded function and bind make problems and would led to long typecasts...
void LEDComponent::setPowerMQTT(const std::string& payload) {
    stopThread();           // <- TODO: in Handler einbauen...

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
    stopThread();           // <- TODO: in Handler einbauen...

    Serial << "  setColor() " << endl;
    m_rgb = Color::createColor(payload);

    Serial << "    Color: " << m_rgb->toString().c_str() << endl;

    setColor(m_rgb);
}

void LEDComponent::setBrightnessMQTT(const std::string& payload) {
    stopThread();           // <- TODO: in Handler einbauen...

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

    auto color = m_rgb->toHSV();
    color->v = (float) brightness / 255.0f;
    m_rgb = color->toRGB();

    setColor(color);     
}

void LEDComponent::setEffectMQTT(const std::string& payload) {
    stopThread();           // <- TODO: in Handler einbauen...

    Serial << "  LEDComponent::setEffect() " << endl;

    // TODO: Move to some kind of factory (simple map not possible due to internal promise...)
    std::string payloardLower = payload;
    std::transform(payloardLower.begin(), payloardLower.end(), payloardLower.begin(), ::tolower);
    if(payloardLower == "colorflow" || payloardLower == "colorflownormal") {
        effect = std::make_shared<ColorFlow>(this);
    } else if(payloardLower == "colorflowslow") {
        effect = std::make_shared<ColorFlow>(this, 1000);
    } else if(payloardLower == "colorflowfast") {
        effect = std::make_shared<ColorFlow>(this, 10);
    } else if(payloardLower == "randomcolor") {
        effect = std::make_shared<RandomColor>(this);
    } else {
        effect = std::make_shared<LEDEffect>(this);
    } 

    //Creating a thread to execute our task
    t = std::thread([&]()
    {
        effect->run();
    });
}

void LEDComponent::stopThread() {

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