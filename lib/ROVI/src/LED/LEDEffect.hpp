#ifndef __LED_EFFECT_H__
#define __LED_EFFECT_H__

class LEDEffect;
#include "LEDComponent.hpp"

/*
 * A Task class that extends the Stoppable Task
 */
class LEDEffect : public Stoppable
{
public:
    // LEDEffect(std::shared_ptr<LEDComponent> led);
    LEDEffect(LEDComponent* led);                       // <- Currently a raw pointer, because I'm not able to create a 
                                                        // shared_ptr to LEDCompontente due to it's abstractness
                                                        // With the latests tries the object was delete together with this class
                                                        // because the ref counter accedently drops to zero... :( 

	// Function to be executed by thread function
	void run();

protected:
    // std::shared_ptr<LEDComponent> led;      // TODO: Synchronize access
    LEDComponent* led;      // TODO: Synchronize access
};

#endif /* __LED_EFFECT_H__ */