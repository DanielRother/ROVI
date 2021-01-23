#ifndef __ROVI_WIFI_MANAGER_H__
#define __ROVI_WIFI_MANAGER_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <FS.h> // this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#ifdef ESP32
#include <SPIFFS.h>
#endif

namespace Rovi {
namespace Config {
struct WiFiDefaultConfig {
  WiFiDefaultConfig() : SSID{""}, password{"hakunamatata"} {
    uint32_t id = 0;
    for (int i = 0; i < 17; i = i + 8) {
      id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    const int ID_STRING_LENGTH = 40;
    char idStr[ID_STRING_LENGTH];
    sprintf(idStr, "RoviConnectAP-%08X\n", id);
    SSID = idStr;
  }
  std::string SSID;
  std::string password;
};

struct MqttConfig {
  MqttConfig() : server{""}, port{1889}, user{""}, password{""} {
    server.reserve(DEFAULT_PARAMETER_SIZE);
    user.reserve(DEFAULT_PARAMETER_SIZE);
    password.reserve(DEFAULT_PARAMETER_SIZE);
  }
  MqttConfig(const char *server, const char *port, const char *user,
             const char *password)
      : server{server}, port{atoi(port)}, user{user}, password{password} {}
  // TODO: Fix me (overload ambigous)
  // MqttConfig(const char *server, const int port, const char *user,
  //            const char *password)
  //     : server{server}, port{port}, user{user}, password{password} {}

  std::string toString() {
    std::stringstream ss;
    ss << "MqttConfig - server = " << server << ", port = " << port
       << ", user = " << user << ", password = " << password;
    return ss.str();
  }

  void print() { std::cout << toString() << std::endl; }

  std::string server;
  int port;
  std::string user;
  std::string password;

  static const int DEFAULT_PARAMETER_SIZE = 40;
};

class RoviWiFiManager {
public:
  RoviWiFiManager()
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
    mqttConfig = MqttConfig{
        custom_mqtt_server.getValue(), custom_mqtt_port.getValue(),
        custom_mqtt_user.getValue(), custom_mqtt_password.getValue()};
    std::cout << "Set MQTT config: " << std::endl;
    mqttConfig.print();
    otaPassword = custom_ota_password.getValue();
    std::cout << "Set OTA password: " << otaPassword << std::endl;

    // save the custom parameters to FS
    if (shouldSaveConfig) {
      Serial.println("saving config");
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
        Serial.println("failed to open config file for writing");
      }

      json.prettyPrintTo(Serial);
      json.printTo(configFile);
      configFile.close();
      // end save
      shouldSaveConfig = false;
    }

    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());
  }

  // callback notifying us of the need to save config
  void saveConfigCallback() {
    Serial.println("Should save config");
    shouldSaveConfig = true;
  }

  void setupSpiffs() {
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

protected:
  // define your default values here, if there are different values in
  // config.json, they are overwritten.
  WiFiDefaultConfig wiFiDefaultConfig;
  MqttConfig mqttConfig;
  std::string otaPassword;

  // flag for saving data
  bool shouldSaveConfig;
};

} // namespace Config
} // namespace Rovi
#endif /* __ROVI_WIFI_MANAGER_H__ */