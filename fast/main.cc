#include "hypergraph.hh"


int
main(int argc, char** argv)
{
  Hg::Hypergraph hg;
  //Hg::io::manual(hg);
  Hg::io::read(hg, argv[1]);
  /*cout << "---" << endl;
  for (auto it = hg.nodes.begin(); it!=hg.nodes.end(); it++)
    cout << **it << endl;
  for (auto it = hg.edges.begin(); it!=hg.edges.end(); it++)
    cout << **it << endl;
  cout << "---" << endl;*/
  Hg::viterbi(hg);

  return 0;
}

