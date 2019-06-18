#ifndef __LED_EFFECT_H__
#define __LED_EFFECT_H__

#include <stdint.h>

namespace Rovi {
    // class LEDEffect;
    // #include "Components/Output/LEDComponent.hpp"       // TBD: Required?
    namespace Components {
        class LEDComponent;
    }

    class LEDEffect {
    public:
        // LEDEffect(std::shared_ptr<Components::LEDComponent> led);
        LEDEffect(Components::LEDComponent* led, uint32_t delay_ms = 100);       // <- Currently a raw pointer, because I'm not able to create a 
                                                                    // shared_ptr to LEDCompontente due to it's abstractness
                                                                    // With the latests tries the object was delete together with this class
                                                                    // because the ref counter accedently drops to zero... :( 

        void update();
        void start();
        void stop();

    protected:
        // Function to be implemented by actual effect
        virtual void step() = 0;

        float getCurrentBrightness();

        // std::shared_ptr<LEDComponent> led;      // TODO: Synchronize access
        Components::LEDComponent* led;      // TODO: Synchronize access
        uint32_t delay_ms;
        unsigned long lastUpdate_ms;   

        bool isStopped;
    };
}

#endif /* __LED_EFFECT_H__ */