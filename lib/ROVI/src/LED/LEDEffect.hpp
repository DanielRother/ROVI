#ifndef __LED_EFFECT_H__
#define __LED_EFFECT_H__

#include "LEDComponent.hpp"

/*
 * A Task class that extends the Stoppable Task
 */
class LEDEffect : public Stoppable
{
public:
    // LEDEffect(std::shared_ptr<LEDComponent> led);

	// Function to be executed by thread function
	void run();

protected:
    std::shared_ptr<LEDComponent> led;      // TODO: Synchronize access
};

#endif /* __LED_EFFECT_H__ */