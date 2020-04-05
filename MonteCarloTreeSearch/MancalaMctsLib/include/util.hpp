#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

class XorShift {
public:
  XorShift(unsigned int seed = 123456789) {
    x = seed;
    y = 362436069;
    z = 521288629;
    w = 88675123;
  }
  unsigned int rand() {
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w;return( w=(w^(w>>19))^(t^(t>>8)) );
  }
  void set_seed(int seed) {
    x = seed;
  }
private:
  unsigned int x, y, z, w;
};

namespace mancala {

std::vector<std::string> split(const std::string &s, char delim);

}

#endif // UTIL_H