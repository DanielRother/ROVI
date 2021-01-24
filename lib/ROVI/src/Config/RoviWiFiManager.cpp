#include "RoviWiFiManager.hpp"

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <FS.h> // this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#ifdef ESP32
#include <SPIFFS.h>
#endif

#include <iostream>
#include <sstream>
#include <stdio.h>

namespace Rovi {
namespace Config {
RoviWiFiManager::RoviWiFiManager()
    : wiFiDefaultConfig{}, mqttConfig{}, otaPassword{""}, shouldSaveConfig{
                                                              false} {
  otaPassword.reserve(40);

  setupSpiffs();

  // WiFiManager, Local intialization. Once its business is done, there is no
  // need to keep it around
  WiFiManager wm;

  // set config save notify callback
  wm.setSaveConfigCallback([&]() { saveConfigCallback(); });

  // setup custom parameters
  //
  // The extra parameters to be configured (can be either global or just in
  // the setup) After connecting, parameter.getValue() will get you the
  // configured value id/name placeholder/prompt default length
  std::cout << "MQTT config: " << std::endl;
  mqttConfig.print();
  std::cout << "OTA password: " << otaPassword << std::endl;

  WiFiManagerParameter custom_mqtt_server("server", "MQTT server",
                                          mqttConfig.server.c_str(),
                                          mqttConfig.server.capacity());
  const int PORT_STRING_LENGTH = 6;
  char mqttPortStr[PORT_STRING_LENGTH];
  sprintf(mqttPortStr, "%i", mqttConfig.port);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT port", mqttPortStr,
                                        PORT_STRING_LENGTH);
  WiFiManagerParameter custom_mqtt_user("mqttUser", "MQTT user",
                                        mqttConfig.user.c_str(),
                                        mqttConfig.user.capacity());
  // TODO: Don't show password in cleartext in frontend
  WiFiManagerParameter custom_mqtt_password("mqttPassword", "MQTT password",
                                            mqttConfig.password.c_str(),
                                            mqttConfig.password.capacity());
  // TODO: Don't show password in cleartext in frontend
  WiFiManagerParameter custom_ota_password("otaPassword", "OTA password",
                                           otaPassword.c_str(),
                                           otaPassword.capacity());

  // add all your parameters here
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_password);
  wm.addParameter(&custom_ota_password);

  // reset settings - wipe credentials for testing
  // wm.resetSettings();

  wm.setConfigPortalTimeout(180);

  // automatically connect using saved credentials if they exist
  // If connection fails it starts an access point with the specified name
  // here  "AutoConnectAP" if empty will auto generate basedcon chipid, if
  // password is blank it will be anonymous and goes into a blocking loop
  // awaiting configuration
  if (!wm.autoConnect(wiFiDefaultConfig.SSID.c_str(),
                      wiFiDefaultConfig.password.c_str())) {
    std::cerr << "Failed to connect and hit timeout" << std::endl;
    delay(3000);
    // if we still have not connected restart and try all over again
    ESP.restart();
    delay(5000);
  }

  // always start configportal for a little while
  // wm.setConfigPortalTimeout(60);
  // wm.startConfigPortal("AutoConnectAP","password");

  // if you get here you have connected to the WiFi
  std::cout << "Connected...yeey :)" << std::endl;

  // read updated parameters
  mqttConfig =
      MqttConfig{custom_mqtt_server.getValue(), custom_mqtt_port.getValue(),
                 custom_mqtt_user.getValue(), custom_mqtt_password.getValue()};
  std::cout << "Set MQTT config: " << std::endl;
  mqttConfig.print();
  otaPassword = custom_ota_password.getValue();
  std::cout << "Set OTA password: " << otaPassword << std::endl;

  // save the custom parameters to FS
  if (shouldSaveConfig) {
    std::cout << "Saving config" << std::endl;
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    JsonObject &mqtt = jsonBuffer.createObject();
    mqtt["server"] = mqttConfig.server.c_str();
    mqtt["port"] = mqttConfig.port;
    mqtt["user"] = mqttConfig.user.c_str();
    mqtt["password"] = mqttConfig.password.c_str();
    json["mqtt"] = mqtt;
    json["otaPassword"] = otaPassword.c_str();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      std::cerr << "Failed to open config file for writing" << std::endl;
    }

    json.prettyPrintTo(Serial);
    json.printTo(configFile);
    configFile.close();
    // end save
    shouldSaveConfig = false;
  }

  std::cout << "Network parameter" << std::endl;
  std::cout << "\tlocal IP: " << std::flush;
  Serial.println(WiFi.localIP()); // Don't write this to cout as IPAddress is
                                  // not handled correctly by default
  std::cout << "\tgateway IP: " << std::flush;
  Serial.println(WiFi.gatewayIP());
  std::cout << "\tsubnet mask: " << std::flush;
  Serial.println(WiFi.subnetMask());
}

// callback notifying us of the need to save config
void RoviWiFiManager::saveConfigCallback() {
  std::cout << "Should save config" << std::endl;
  shouldSaveConfig = true;
}

void RoviWiFiManager::setupSpiffs() {
  // clean FS, for testing
  // SPIFFS.format();

  // read configuration from FS json
  std::cout << "Mounting file system..." << std::endl;

  if (SPIFFS.begin()) {
    std::cout << "... file system mounted" << std::endl;
    if (SPIFFS.exists("/config.json")) {
      // file exists, reading and loading
      std::cout << "Reading config file..." << std::endl;
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        std::cout << "...config file read" << std::endl;
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          std::cout << std::endl << "JSON parsed" << std::endl;
          std::cout << std::endl << "Loaded MQTT config: " << std::endl;
          auto mqtt = json["mqtt"];
          mqttConfig = MqttConfig{mqtt["server"], mqtt["port"], mqtt["user"],
                                  mqtt["password"]};
          char tmpOtaPassword[40];
          strcpy(tmpOtaPassword, json["otaPassword"]);
          otaPassword = tmpOtaPassword;
        } else {
          std::cerr << "Failed to load json config" << std::endl;
        }
      }
    }
  } else {
    std::cerr << "Failed to mount FS" << std::endl;
  }
  // end read
}

} // namespace Config
} // namespace Rovi