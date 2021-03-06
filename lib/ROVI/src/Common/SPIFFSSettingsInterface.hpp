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
    settingsToJson(json, jsonBuffer);
    json.prettyPrintTo(Serial);

    File configFile = SPIFFS.open(filename.c_str(), "w");
    if (!configFile) {
      std::cerr << "Failed to open config file for writing" << std::endl;
    }
    json.printTo(configFile);
    configFile.close();
  }

  virtual void restoreSettings() {
    // clean FS, for testing
    // SPIFFS.format();

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
            jsonToSettings(json);
          } else {
            std::cerr << "Failed to load json config" << std::endl;
          }
        }
      }
    } else {
      std::cerr << "Failed to mount FS" << std::endl;
    }
  }

  // Why the hell can't these function be overloads of save/restoreSettings()?
  // Maybe https://isocpp.org/wiki/faq/strange-inheritance#overload-derived ?
  virtual void settingsToJson(JsonObject &settings,
                              DynamicJsonBuffer &buffer) = 0;
  virtual void jsonToSettings(JsonObject &settings) = 0;

protected:
  template <typename T>
  std::string restoreString(T &json, const std::string key) {
    char tmpDeviceName[DEFAULT_STRING_PARAMETER_SIZE];
    auto value = json[key.c_str()];
    if ((const char *)value) {
      std::cout << "read successful" << std::endl;
      strcpy(tmpDeviceName, value);
      return std::string{tmpDeviceName};
    } else {
      std::cout << "read failed" << std::endl;
      return "";
    }
  }

  std::string filename;

  static const int DEFAULT_STRING_PARAMETER_SIZE = 40;
};

#endif /* __SPIFFS_SETTINGS_INTERFACE_H__ */