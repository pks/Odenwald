#include "parse.hh"


namespace Parse {


} // 


vector<G::T> tokenize(string s)
{
  istringstream ss(s);
  vector<G::T> res; 
  while (ss.good()) {
    string t;
    ss >> t;
    G::T i(t);
    cout << i.word << endl;
    res.push_back(i); 
  }
  return res;
}


bool operator==(vector<G::Item> const& a, vector<G::Item> const& b)
{
  if (a.size() != b.size()) return false;
  for (auto it: a)
}

int main(int argc, char** argv)
{
  string in("karten haie");
  vector<G::T> tok = tokenize(in);
  for (auto it: tok)
    cout << it.word << ",";
  cout << endl;
  size_t n = tok.size();

  G::Grammar g(argv[1]);

  vector<Span> spans; 
  Parse::visit(spans, 1, 0, 6);
  for (auto it: spans) {
    cout << "(" << it.first << "," << it.second << ")" << endl;
  }

  Parse::Chart active(n);
  Parse::Chart passive(n);

  //init(tok, n, active, passive, g);
  
  cout << *(g.flat.at(0)) << endl;
}

