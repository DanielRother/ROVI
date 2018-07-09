#include "LEDComponent.hpp"

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"
#include "LEDEffect.hpp"

#include <ThreadUtil.hpp>


LEDComponent::LEDComponent(const std::string& name) 
: RoviComponent(name) {
    handler[std::string("setPower")]        = std::bind(&LEDComponent::setPowerMQTT,        this, std::placeholders::_1);
    handler[std::string("setColor")]        = std::bind(&LEDComponent::setColorMQTT,        this, std::placeholders::_1);
    handler[std::string("setBrightness")]   = std::bind(&LEDComponent::setBrightnessMQTT,   this, std::placeholders::_1);
    handler[std::string("setEffect")]       = std::bind(&LEDComponent::setEffectMQTT,       this, std::placeholders::_1);
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

    // // t = std::thread(&LEDComponent::colorFlow, this);        // <- First try
    // // t.join();

    // Second try
    // (moved to sublcass due to pure virtual...)
    // TODO: CLeanup
    // // Creating our Task
    // // LEDEffect effect(std::make_shared<LEDComponent>(*this));         // <- Not compiling due to pure virtual...
    // LEDEffect effect;

    // //Creating a thread to execute our task
    // std::thread th([&]()
    // {
    //     effect.run();
    // });

    // std::this_thread::sleep_for(std::chrono::seconds(10));

    // std::cout << "Asking Task to Stop" << std::endl;
    // // Stop the Task
    // effect.stop();

    // //Waiting for thread to join
    // th.join();
    // std::cout << "Thread Joined" << std::endl;
}

void LEDComponent::colorFlow() {
    for(double h = 0.0; h < 360.0; ++h) {
        // TODO: Reactivate
        // hsv hsvValue;
        // hsvValue.h = h;
        // hsvValue.s = 1.0;
        // hsvValue.v = 0.5;

        // rgb rgbValue = hsv2rgb(hsvValue);

        // m_rgb = std::make_shared<RGBColor>(rgbValue.r * 255, rgbValue.g * 255, rgbValue.b * 255);
        setColor(std::make_shared<HSVColor>(h, 1.0, 0.5));
        delay(100);
    }

    Serial << "colorFlow finished" << endl;
}