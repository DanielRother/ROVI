#include <Arduino.h>
#include <ArduinoOTA.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>


// //***************************************************************************//
// // Basecamp and Rovi initialize 
// //***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include <Devices/AbsoluteHueMQTT.h>

// RotaryEncoder
uint8_t rotaryPinA = 12;
uint8_t rotaryPinB = 13;
uint8_t rotaryPinButton = 14;

// LED
uint8_t  neoPixelPin    = 15;
uint16_t nbNeoPixelLEDs = 20;

std::shared_ptr<Rovi::Devices::AbsoluteHueMQTT> absolutHue;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
    std::cout << "--- setup() ---" << endl;

    sleep(5);
    iot.begin();

    absolutHue = std::make_shared<Rovi::Devices::AbsoluteHueMQTT>(
      iot,
      rotaryPinA, rotaryPinB, rotaryPinButton,
      neoPixelPin, nbNeoPixelLEDs);

	// Setup LED
	auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
	for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
		swapRGValues[pixelIdx] = 1;
	}
	absolutHue->setSwapRGValues(swapRGValues);

	// Activate Over-the-Air updates
	String otaPassword = iot.configuration.get("OTAPassword");
	std::cout << "*******************************************" << endl
			<< "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
			<< "*******************************************" << endl;
	ArduinoOTA.setPassword(otaPassword.c_str());
	ArduinoOTA.begin();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
bool showEnableMessage = false;

void loop() {
	ArduinoOTA.handle();

	absolutHue->update();
}

