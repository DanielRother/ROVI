#ifndef __LED_COMPONENT_MQTT_H__
#define __LED_COMPONENT_MQTT_H__

#include "BaseClasses/RoviComponent.hpp"
#include "Components/Output/LEDComponent.hpp"
#include "Components/Output/NeoPixelComponent.hpp"

namespace Rovi {
    namespace Components {
        namespace Deprecated {
        //     class LEDComponentMQTT : public LEDComponent, public Rovi::Deprecated::RoviComponent {
        //         public:
        //         LEDComponentMQTT(const std::string& name = "LEDComponentsMQTT")
        //             : LEDComponent(name)
        //             , RoviComponent(name) {
        //                 // TODO: (Re-)move
        //                 // Always stop a possibly running effect thread first
        //                 handler[std::string("setPower")]        = [this](const std::string payload){
        //                     setOn(payload);};
        //                 handler[std::string("setColor")]        = [this](const std::string payload){
        //                     stopEffect();
        //                     setColor(payload);};
        //                 handler[std::string("setBrightness")]   = [this](const std::string payload){
        //                     setBrightness(payload);};
        //                 handler[std::string("setEffect")]       = [this](const std::string payload){
        //                     setEffect(payload);};
        //             }
        //     };

        // class NeoPixelComponentMQTT : public LEDComponentMQTT, public NeoPixelComponent {
        //     public:
        //     NeoPixelComponentMQTT(uint16_t nbPixel, uint8_t pin, const std::string& name = "NeoPixelMQTT") 
        //     : LEDComponentMQTT(name)
        //     , NeoPixelComponent(nbPixel, pin, name) {

        //     }
        // };

            class NeoPixelComponentMQTT : public NeoPixelComponent, public Rovi::Deprecated::RoviComponent {
                public:
                NeoPixelComponentMQTT(uint16_t nbPixel, uint8_t pin, const std::string& name = "NeoPixelMQTT") 
                : NeoPixelComponent(nbPixel, pin, name)
                , RoviComponent(name)
                {
                    // TODO: (Re-)move
                    // Always stop a possibly running effect thread first
                    handler[std::string("setPower")]        = [this](const std::string payload){
                        setOn(payload);};
                    handler[std::string("setColor")]        = [this](const std::string payload){
                        stopEffect();
                        setColor(payload);};
                    handler[std::string("setBrightness")]   = [this](const std::string payload){
                        setBrightness(payload);};
                    handler[std::string("setEffect")]       = [this](const std::string payload){
                        setEffect(payload);};
                }
            };
        }
    }
}

#endif /* __LED_COMPONENT_MQTT_H__ */