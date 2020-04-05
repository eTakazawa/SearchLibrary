
#include "util.hpp"

namespace mancala {

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::string item;
  for (char ch: s) {
    if (ch == delim) {
      if (!item.empty())
        elems.push_back(item);
        item.clear();
    }
    else {
      item += ch;
    }
  }
  if (!item.empty())
    elems.push_back(item);
  return elems;
}

}