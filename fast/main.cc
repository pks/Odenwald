#include "hypergraph.hh"
#include <ctime>


int
main(int argc, char** argv)
{
  Hg::Hypergraph hg;
  G::Grammar g;
  Hg::io::read(hg, g.rules, argv[1]);
  //Hg::io::manual(hg, g.rules);
  clock_t begin = clock();
  Hg::Path p;
  Hg::viterbi_path(hg, p);
  vector<string> s;
  Hg::derive(p, p.back()->head, s);
  for (auto it: s)
    cout << it << " ";
  cout << endl;
  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << elapsed_secs << " s" << endl;

  return 0;
}

