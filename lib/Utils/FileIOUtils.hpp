#ifndef __FILEIOUTILS_H__
#define __FILEIOUTILS_H__

#include <Arduino.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

static std::string fullfile(const std::string& baseDirectory, const std::string& additionDirectoryOrFile) {
    std::string ret = baseDirectory;

    // Check if the baseDirectory ends with an slash and add one if not 
    if (ret.find_last_of("/") != ret.length() - 1) {
        ret += "/";
    }

    return ret + additionDirectoryOrFile;
}

static std::vector<std::string> splitString(const std::string& s, char delimiter) {
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

#endif /* __FILEIOUTILS_H__ */