#include <fstream>

#include "grammar.hh"

using namespace std;


int
main(int argc, char** argv)
{
  G::Grammar g(argv[1]);
  for (auto it: g.rules)
    cout << it->escaped() << endl;

  return 0;
}

