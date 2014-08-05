#include "hypergraph.hh"


int
main(int argc, char** argv)
{
  Hg::Hypergraph hg;
  G::Grammar g;
//Hg::io::read(hg, g.rules, argv[1]);
  Hg::io::manual(hg, g.rules);

  Hg::Path p;
  Hg::viterbi_path(hg, p);
  vector<string> s;
  Hg::derive(p, p.back()->head, s);
  for (auto it: s)
    cout << it << " ";
  cout << endl;
  

  return 0;
}

