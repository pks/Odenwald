#include <fstream>

#include "grammar.hh"

using namespace std;


int
main(int argc, char** argv)
{
  G::Vocabulary y;
  G::Grammar g(argv[1], y);
  for (auto it: g.rules) {
    it->escaped(cout);
    cout << endl;
  }

  return 0;
}

