#include <Arduino.h>
#include <ArduinoOTA.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include <Devices/SimpleXmasTree.hpp>

Rovi::Deprecated::RoviDevice myRovi(iot);

const auto nbPixel = 50;
const auto pin = 15;
auto name = "weihnachtsbaum";
auto colorCircleDelay = 500;

std::shared_ptr<Rovi::Devices::SimpleXmasTree<pin, nbPixel>> xmastree; 
// auto xmastree = std::make_shared<Rovi::Components::FastLedComponent<nbPixel, pin>>(name);


void setup()
{
  std::cout << "--- setup() ---" << endl;
  sleep(5);
  iot.begin();
  myRovi.setupRovi();

  xmastree = std::make_shared<Rovi::Devices::SimpleXmasTree<pin, nbPixel>>(colorCircleDelay, name);

  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword"); // The prefedined variable OTAPassword is always reset to '1'; TODO: Check why/fix?
  std::cout << "*******************************************" << endl
         << "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
         << "*******************************************" << endl;

  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();
}

void loop()
{
    ArduinoOTA.handle();
    xmastree->update();
}