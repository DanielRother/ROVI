#include "LEDComponent.hpp"

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"

#include <ThreadUtil.hpp>


LEDComponent::LEDComponent(const std::string& name) 
: RoviComponent(name), effect(std::make_shared<LEDEffect>(this)) {
    handler[std::string("setPower")]        = std::bind(&LEDComponent::setPowerMQTT,        this, std::placeholders::_1);
    handler[std::string("setColor")]        = std::bind(&LEDComponent::setColorMQTT,        this, std::placeholders::_1);
    handler[std::string("setBrightness")]   = std::bind(&LEDComponent::setBrightnessMQTT,   this, std::placeholders::_1);
    handler[std::string("setEffect")]       = std::bind(&LEDComponent::setEffectMQTT,       this, std::placeholders::_1);
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
    Serial << "  setColor() " << endl;
    m_rgb = Color::createColor(payload);

    Serial << "    Color: " << m_rgb->toString().c_str() << endl;

    setColor(m_rgb);
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

    setBrightness(brightness);        
}

void LEDComponent::setEffectMQTT(const std::string& payload) {
    Serial << "  LEDComponent::setEffect() " << endl;
    Serial << "!!!! NOT IMPLEMENTED YET !!!!" << endl;
    // TODO


    // TODO: CLeanup
    // // Creating our Task    
    // LEDEffect effect(this);                             // <- Currently a raw pointer, because I'm not able to create a 
                                                        // shared_ptr to LEDCompontente due to it's abstractness
                                                        // With the latests tries the object was delete together with this class
                                                        // because the ref counter accedently drops to zero... :( 
    // Something like this would be better
    // std::shared_ptr<LEDComponent> thisPtr;//(this);
    // thisPtr.reset(this);
    // LEDEffect effect(std::make_shared<LEDComponent>(*this));         // <- Not compiling due to pure virtual...
    // LEDEffect effect;

    effect = std::make_shared<LEDEffect>(this);

    //Creating a thread to execute our task
    std::thread th([&]()
    {
        effect->run();
    });

    // TODO: Move to stopThread()...
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Asking Task to Stop" << std::endl;
    // Stop the Task
    effect->stop();
    std::cout << "Asking Thread to Join" << std::endl;

    //Waiting for thread to join
    th.join();
    std::cout << "Thread Joined" << std::endl;
}

void LEDComponent::stopThread() {
    
}


void LEDComponent::colorFlow() {
    for(double h = 0.0; h < 360.0; ++h) {
        setColor(std::make_shared<HSVColor>(h, 1.0, 0.5));
        delay(100);
    }

    Serial << "colorFlow finished" << endl;
}