#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <string>
#include <sstream>

using namespace std;

namespace Grammar {


class NT {
  public:
    string       symbol;
    unsigned int index;

    string s();
};

class T {
  public:
    string word;
};

class Rule {
  public:
};


} // namespace

#endif
