#ifndef __SPIFFS_SETTINGS_INTERFACE_H__
#define __SPIFFS_SETTINGS_INTERFACE_H__

#include <Arduino.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <FS.h> // this needs to be first, or it all crashes and burns...
#include <WiFi.h>
#ifdef ESP32
#include <SPIFFS.h>
#endif

#include <iostream>
#include <string>

class SPIFFSSettingsInterface {
public:
  SPIFFSSettingsInterface(const std::string filename = "/settings.json")
      : filename{filename} {}

  virtual void saveSettings() {
    std::cout << "Saving settings" << std::endl;
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    saveSettingsImpl(json, jsonBuffer);
    json.prettyPrintTo(Serial);

    File configFile = SPIFFS.open(filename.c_str(), "w");
    if (!configFile) {
      std::cerr << "Failed to open config file for writing" << std::endl;
    }
    json.printTo(configFile);
    configFile.close();
  }

  virtual void restoreSettings() {
    // read configuration from FS json
    std::cout << "Mounting file system..." << std::endl;

    if (SPIFFS.begin()) {
      std::cout << "... file system mounted" << std::endl;
      if (SPIFFS.exists(filename.c_str())) {
        // file exists, reading and loading
        std::cout << "Reading config file..." << std::endl;
        File settingsFile = SPIFFS.open(filename.c_str(), "r");
        if (settingsFile) {
          std::cout << "...config file read" << std::endl;
          size_t size = settingsFile.size();
          // Allocate a buffer to store contents of the file.
          std::unique_ptr<char[]> buf(new char[size]);

          settingsFile.readBytes(buf.get(), size);
          DynamicJsonBuffer jsonBuffer;
          JsonObject &json = jsonBuffer.parseObject(buf.get());
          json.printTo(Serial);
          if (json.success()) {
            std::cout << std::endl << "JSON parsed" << std::endl;
            restoreSettingsImpl(json);
          } else {
            std::cerr << "Failed to load json config" << std::endl;
          }
        }
      }
    } else {
      std::cerr << "Failed to mount FS" << std::endl;
    }
  }

protected:
  // Why the hell can't these function be overloads of save/restoreSettings()?
  // Maybe https://isocpp.org/wiki/faq/strange-inheritance#overload-derived ?
  virtual void saveSettingsImpl(JsonObject &settings,
                                DynamicJsonBuffer &buffer) = 0;
  virtual void restoreSettingsImpl(JsonObject &settings) = 0;

  template <typename T>
  std::string restoreString(T &json, const std::string key) {
    char tmpDeviceName[DEFAULT_STRING_PARAMETER_SIZE];
    strcpy(tmpDeviceName, json[key.c_str()]);
    return std::string{tmpDeviceName};
  }

  std::string filename;

  static const int DEFAULT_STRING_PARAMETER_SIZE = 40;
};

#endif /* __SPIFFS_SETTINGS_INTERFACE_H__ */