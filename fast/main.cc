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
  Hg::Hypergraph hg;

  // nodes
  Hg::Node a; a.id = 0; a.symbol = "root"; a.left = false; a.right = false; a.mark = 0;
  Hg::Node b; b.id = 1; b.symbol = "NP";   b.left = 0;     b.right = 1; b.mark = 0;
  Hg::Node c; c.id = 2; c.symbol = "V";    c.left = 1;     c.right = 2; c.mark = 0;
  Hg::Node d; d.id = 3; d.symbol = "JJ";   d.left = 3;     d.right = 4; d.mark = 0;
  Hg::Node e; e.id = 4; e.symbol = "NN";   e.left = 3;     e.right = 5; e.mark = 0;
  Hg::Node f; f.id = 5; f.symbol = "NP";   f.left = 2;     f.right = 5; f.mark = 0;
  Hg::Node g; g.id = 6; g.symbol = "NP";   g.left = 1;     g.right = 5; g.mark = 0;
  Hg::Node h; h.id = 7; h.symbol = "S";    h.left = 0;     h.right = 6; h.mark = 0;

  hg.add_node(&a);
  hg.add_node(&h);
  hg.add_node(&g);
  hg.add_node(&c);
  hg.add_node(&d);
  hg.add_node(&f);
  hg.add_node(&b);
  hg.add_node(&e);

  // edges
  Hg::Edge q; q.head = hg.nodes_by_id[1]; q.tails.push_back(hg.nodes_by_id[0]); q.score = 0.367879441171;
  hg.nodes_by_id[1]->incoming.push_back(&q);
  hg.nodes_by_id[0]->outgoing.push_back(&q);
  q.arity = 1;
  q.mark = 0;
  hg.edges.push_back(&q);

  Hg::Edge p; p.head = hg.nodes_by_id[2]; p.tails.push_back(hg.nodes_by_id[0]); p.score = 0.606530659713;
  hg.nodes_by_id[2]->incoming.push_back(&p);
  hg.nodes_by_id[0]->outgoing.push_back(&p);
  p.arity = 1;
  p.mark = 0;
  hg.edges.push_back(&p);

  Hg::Edge r; r.head = hg.nodes_by_id[3]; r.tails.push_back(hg.nodes_by_id[0]); r.score = 1.0;
  hg.nodes_by_id[3]->incoming.push_back(&r);
  hg.nodes_by_id[0]->outgoing.push_back(&r);
  r.arity = 1;
  r.mark = 0;
  hg.edges.push_back(&r);

  Hg::Edge s; s.head = hg.nodes_by_id[3]; s.tails.push_back(hg.nodes_by_id[0]); s.score = 1.0;
  hg.nodes_by_id[3]->incoming.push_back(&s);
  hg.nodes_by_id[0]->outgoing.push_back(&s);
  s.arity = 1;
  s.mark = 0;
  hg.edges.push_back(&s);

  Hg::Edge t; t.head = hg.nodes_by_id[4]; t.tails.push_back(hg.nodes_by_id[0]); t.score = 1.0;
  hg.nodes_by_id[4]->incoming.push_back(&t);
  hg.nodes_by_id[0]->outgoing.push_back(&t);
  t.arity = 1;
  t.mark = 0;
  hg.edges.push_back(&t);

  Hg::Edge u; u.head = hg.nodes_by_id[4]; u.tails.push_back(hg.nodes_by_id[0]); u.score = 1.0;
  hg.nodes_by_id[4]->incoming.push_back(&u);
  hg.nodes_by_id[0]->outgoing.push_back(&u);
  u.arity = 1;
  u.mark = 0;
  hg.edges.push_back(&u);

  Hg::Edge v; v.head = hg.nodes_by_id[4]; v.tails.push_back(hg.nodes_by_id[3]); v.score = 1.0;
  hg.nodes_by_id[4]->incoming.push_back(&v);
  hg.nodes_by_id[3]->outgoing.push_back(&v);
  v.arity = 1;
  v.mark = 0;
  hg.edges.push_back(&v);

  Hg::Edge w; w.head = hg.nodes_by_id[4]; w.tails.push_back(hg.nodes_by_id[3]); w.score = 2.71828182846;
  hg.nodes_by_id[4]->incoming.push_back(&w);
  hg.nodes_by_id[3]->outgoing.push_back(&w);
  w.arity = 1;
  w.mark = 0;
  hg.edges.push_back(&w);

  Hg::Edge x; x.head = hg.nodes_by_id[5]; x.tails.push_back(hg.nodes_by_id[4]); x.score = 1.0;
  hg.nodes_by_id[5]->incoming.push_back(&x);
  hg.nodes_by_id[4]->outgoing.push_back(&x);
  x.arity = 1;
  x.mark = 0;
  hg.edges.push_back(&x);

  Hg::Edge y; y.head = hg.nodes_by_id[6]; y.tails.push_back(hg.nodes_by_id[2]); y.tails.push_back(hg.nodes_by_id[5]); y.score = 1.0;
  hg.nodes_by_id[6]->incoming.push_back(&y);
  hg.nodes_by_id[2]->outgoing.push_back(&y);
  hg.nodes_by_id[5]->outgoing.push_back(&y);
  y.arity = 2;
  y.mark = 0;
  hg.edges.push_back(&y);

  Hg::Edge z; z.head = hg.nodes_by_id[7]; z.tails.push_back(hg.nodes_by_id[1]); z.tails.push_back(hg.nodes_by_id[6]); z.score = 1.0;
  hg.nodes_by_id[7]->incoming.push_back(&z);
  hg.nodes_by_id[1]->outgoing.push_back(&z);
  hg.nodes_by_id[6]->outgoing.push_back(&z);
  z.arity = 2;
  z.mark = 0;
  hg.edges.push_back(&z);

  Hg::topological_sort(hg.nodes, hg.nodes.begin());
  //Hg::viterbi(nodes, hg.nodes, hg.nodes_by_id(0]);
}

