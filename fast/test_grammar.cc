#include <fstream>

#include "grammar.hh"

using namespace std;


int
main(int argc, char** argv)
{
  G::Grammar g(argv[1]);
  cout << g << endl;
  return 0;
}

