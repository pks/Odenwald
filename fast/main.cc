#include "hypergraph.hh"


int
main(void)
{
/*
{
"weights":{"logp":2.0,"use_house":0.0,"use_shell":1.0},
"nodes":
[
{ "id":0, "cat":"root", "span":[0,0] },
{ "id":1, "cat":"NP", "span":[1,2] },
{ "id":2, "cat":"V", "span":[2,3] },
{ "id":3, "cat":"JJ", "span":[4,5] },
{ "id":4, "cat":"NN", "span":[4,6] },
{ "id":5, "cat":"NP", "span":[3,6] },
{ "id":6, "cat":"VP", "span":[2,6] },
{ "id":7, "cat":"S", "span":[1,6] }
],
"edges":
[
{ "head":1, "rule":"[NP] ||| ich ||| i", "tails":[0], "f":{"logp":-0.5,"use_i":1.0} },
{ "head":2, "rule":"[V] ||| sah ||| saw", "tails":[0], "f":{"logp":-0.25,"use_saw":1.0} },
{ "head":3, "rule":"[JJ] ||| kleines ||| small", "tails":[0], "f":{"logp":0.0,"use_small":1.0} },
{ "head":3, "rule":"[JJ] ||| kleines ||| little", "tails":[0], "f":{"logp":0.0,"use_little":1.0} },
{ "head":4, "rule":"[NN] ||| kleines haus ||| small house", "tails":[0], "f":{"logp":0.0,"use_house":1.0} },
{ "head":4, "rule":"[NN] ||| kleines haus ||| little house", "tails":[0], "f":{"logp":0.0,"use_house":1.0} },
{ "head":4, "rule":"[NN] ||| [JJ,1] haus ||| [JJ,1] shell", "tails":[3], "f":{"logp":0.0,"use_shell":1.0} },
{ "head":4, "rule":"[NN] ||| [JJ,1] haus ||| [JJ,1] house", "tails":[3], "f":{"logp":0.0,"use_house":1.0} },
{ "head":5, "rule":"[NP] ||| ein [NN,1] ||| a [NN,1]", "tails":[4], "f":{"logp":0.0,"use_a":1.0} },
{ "head":6, "rule":"[VP] ||| [V,1] [NP,2] ||| [V,1] [NP,2]", "tails":[2, 5], "f":{"logp":0.0} },
{ "head":7, "rule":"[S] ||| [NP,1] [VP,2] ||| [NP,1] [VP,2]", "tails":[1, 6], "f":{"logp":0.0} }
]
}
*/

  // nodes
  Hg::Node a; a.id = 0; a.symbol = "root"; a.left = false; a.right = false;
  Hg::Node b; b.id = 1; b.symbol = "NP";   b.left = 0;     b.right = 1;
  Hg::Node c; c.id = 2; c.symbol = "V";    c.left = 1;     c.right = 2;
  Hg::Node d; d.id = 3; d.symbol = "JJ";   d.left = 3;     d.right = 4;
  Hg::Node e; e.id = 4; e.symbol = "NN";   e.left = 3;     e.right = 5;
  Hg::Node f; f.id = 5; f.symbol = "NP";   f.left = 2;     f.right = 5;
  Hg::Node g; g.id = 6; g.symbol = "NP";   g.left = 1;     g.right = 5;
  Hg::Node h; h.id = 7; h.symbol = "S";    g.left = 0;     g.right = 5;

  vector<Hg::Node*> nodes;
  nodes.push_back(&a);
  nodes.push_back(&b);
  nodes.push_back(&c);
  nodes.push_back(&d);
  nodes.push_back(&e);
  nodes.push_back(&f);
  nodes.push_back(&g);
  nodes.push_back(&h);

  // nodes_by_id
  map<unsigned int, Hg::Node*> nodes_by_id;
  nodes_by_id[0] = &a;
  nodes_by_id[1] = &b;
  nodes_by_id[2] = &c;
  nodes_by_id[3] = &d;
  nodes_by_id[4] = &e;
  nodes_by_id[5] = &f;
  nodes_by_id[6] = &g;
  nodes_by_id[7] = &h;

  // edges
  Hg::Hyperedge q; q.head = nodes_by_id[1]; q.tails.push_back(nodes_by_id[0]); q.score = 0.367879441171;
  nodes_by_id[1]->incoming.push_back(&q);
  nodes_by_id[0]->outgoing.push_back(&q);
  q.arity_ = 1;
  q.mark = 0;

  Hg::Hyperedge p; p.head = nodes_by_id[2]; p.tails.push_back(nodes_by_id[0]); p.score = 0.606530659713;
  nodes_by_id[2]->incoming.push_back(&p);
  nodes_by_id[0]->outgoing.push_back(&p);
  p.arity_ = 1;
  p.mark = 0;

  Hg::Hyperedge r; r.head = nodes_by_id[3]; r.tails.push_back(nodes_by_id[0]); r.score = 1.0;
  nodes_by_id[3]->incoming.push_back(&r);
  nodes_by_id[0]->outgoing.push_back(&r);
  r.arity_ = 1;
  r.mark = 0;
  Hg::Hyperedge s; s.head = nodes_by_id[3]; s.tails.push_back(nodes_by_id[0]); s.score = 1.0;
  nodes_by_id[3]->incoming.push_back(&s);
  nodes_by_id[0]->outgoing.push_back(&s);
  s.arity_ = 1;
  s.mark = 0;

  Hg::Hyperedge t; t.head = nodes_by_id[4]; t.tails.push_back(nodes_by_id[0]); t.score = 1.0;
  nodes_by_id[4]->incoming.push_back(&t);
  nodes_by_id[0]->outgoing.push_back(&t);
  t.arity_ = 1;
  t.mark = 0;
  Hg::Hyperedge u; u.head = nodes_by_id[4]; u.tails.push_back(nodes_by_id[0]); u.score = 1.0;
  nodes_by_id[4]->incoming.push_back(&u);
  nodes_by_id[0]->outgoing.push_back(&u);
  u.arity_ = 1;
  u.mark = 0;

  Hg::Hyperedge v; v.head = nodes_by_id[4]; v.tails.push_back(nodes_by_id[3]); v.score = 1.0;
  nodes_by_id[4]->incoming.push_back(&v);
  nodes_by_id[3]->outgoing.push_back(&v);
  v.arity_ = 1;
  v.mark = 0;
  Hg::Hyperedge w; w.head = nodes_by_id[4]; w.tails.push_back(nodes_by_id[3]); w.score = 2.71828182846;
  nodes_by_id[4]->incoming.push_back(&w);
  nodes_by_id[3]->outgoing.push_back(&w);
  w.arity_ = 1;
  w.mark = 0;

  Hg::Hyperedge x; x.head = nodes_by_id[5]; x.tails.push_back(nodes_by_id[4]); x.score = 1.0;
  nodes_by_id[5]->incoming.push_back(&x);
  nodes_by_id[4]->outgoing.push_back(&x);
  x.arity_ = 1;
  x.mark = 0;

  Hg::Hyperedge y; y.head = nodes_by_id[6]; y.tails.push_back(nodes_by_id[2]); y.tails.push_back(nodes_by_id[5]); y.score = 1.0;
  nodes_by_id[6]->incoming.push_back(&y);
  nodes_by_id[2]->outgoing.push_back(&y);
  nodes_by_id[5]->outgoing.push_back(&y);
  y.arity_ = 2;
  y.mark = 0;

  Hg::Hyperedge z; z.head = nodes_by_id[7]; z.tails.push_back(nodes_by_id[1]); z.tails.push_back(nodes_by_id[6]); z.score = 1.0;
  nodes_by_id[7]->incoming.push_back(&z);
  nodes_by_id[1]->outgoing.push_back(&z);
  nodes_by_id[6]->outgoing.push_back(&z);
  z.arity_ = 2;
  z.mark = 0;

  // test
  Hg::viterbi(nodes, nodes_by_id, nodes_by_id[0]);
}

