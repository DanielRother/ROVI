#ifndef __ICONS_H__
#define __ICONS_H__

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <enum.h>

extern "C" {
#include "crypto/base64.h"
}
#include <pngle.h>

namespace Rovi {
BETTER_ENUM(Icons, int, BANANA, WOLVERINE);

class IconsFactory {
public:
  static std::string getBase64EncodedIcon(const Icons icon) {
    switch (icon) {
    case Icons::BANANA:
      return std::string{
          "iVBORw0KGgoAAAANSUhEUgAAAAgAAAAICAIAAABLbSncAAAAAXNSR0IArs4c6QAAAJtJ"
          "REFU"
          "CB1jZkACc8M9DDhMDz6/giQGYy6KcKg19ATyGGEiUPrHg6A/"
          "l985FvxiQZbQVDZjFuZg95La+uUJE1xCU8Vs32ZpZi6Wf+//"
          "8kixQ416ejNYSI6Dg43575vvP06/+vbmD8utGeZijlL8KpxArX9ffv9x/"
          "tXLm5+9exmYH3/"
          "RCtH8/ufBl1+XPv599+Prq98BE9guPzgJALOLPiGN8SoeAAAAAElFTkSuQmCC"};
      break;
    case Icons::WOLVERINE:
      return std::string{
          "iVBORw0KGgoAAAANSUhEUgAAAAgAAAAICAIAAABLbSncAAAAAXNSR0IArs4c6QAAAIJJ"
          "REFU"
          "CB1jFFewN1VlQAOnbzOwvHxwkEHVfvM2Fbicr9cdkKDXrK9Aof+/kyHo/"
          "TQnIBcoyASkgKYxss4FMj7Mvi+YtQ/"
          "IBbJB4M+7x0ASqAOiHMgFyjEBMYuQLFgBlIBygRJ9zgZfDy8AKgQiIAPIBQoy+"
          "rjar6xLRNYBZIc3zQcACGo40ycedMMAAAAASUVORK5CYII="};
      break;
    default:
      return "";
      break;
    }
  }

  static std::vector<unsigned char> getBinaryIcon(const Icons icon) {
    auto base64Enc = getBase64EncodedIcon(icon);

    size_t outputLength;
    unsigned char *binary =
        base64_decode((const unsigned char *)base64Enc.c_str(),
                      base64Enc.size(), &outputLength);

    std::cout << "outputLength: " << outputLength << std::endl;
    auto output = std::vector<unsigned char>(outputLength);
    std::cout << "output.size(): " << output.size() << std::endl;
    std::memcpy(output.data(), binary, outputLength);
    return output;
  }
};
} // namespace Rovi

#endif /* __ICONS_H__ */