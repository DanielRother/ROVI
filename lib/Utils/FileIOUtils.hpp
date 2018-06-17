#ifndef __FILEIOUTILS_H__
#define __FILEIOUTILS_H__

#include <Arduino.h>

#include <stdio.h>
#include <string>

std::string fullfile(const std::string& baseDirectory, const std::string& additionDirectoryOrFile) {
    std::string ret = baseDirectory;

    // Check if the baseDirectory ends with an slash and add one if not 
    if (ret.find_last_of("/") != ret.length() - 1) {
        ret += "/";
    }

    return ret + additionDirectoryOrFile;
}


#endif /* __FILEIOUTILS_H__ */