#ifndef __FILEIOUTILS_H__
#define __FILEIOUTILS_H__

#include <Arduino.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

// TODO: Namespace

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

template<typename T>
static std::string to_string(T value) {     //TODO: Andere Methoden haben keine '_'
    std::stringstream ss;
    ss << value;
    return ss.str();
}

static std::string toLower(const std::string& in) {
    std::string lower;
    lower.resize(in.size());                         // allocate space
    std::transform(in.begin(), in.end(), lower.begin(), ::tolower);

    return lower;
}


bool checkStringForAllowedCharacters(const std::string& inputString, const std::string& allowedChars) {
    bool ok = true;
    for(auto& character : inputString) {
        ok = allowedChars.find(character) != std::string::npos;
        if(!ok) {
            break;
        }
    }

    return ok;
}

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string removeCharsFromString(const std::string& str, const std::string& charsToRemove) {
    auto retStr = str;
    for ( unsigned int i = 0; i < charsToRemove.size(); ++i ) {
        retStr.erase( remove(retStr.begin(), retStr.end(), charsToRemove[i]), retStr.end() );
    }
    return retStr;
}

#endif /* __FILEIOUTILS_H__ */