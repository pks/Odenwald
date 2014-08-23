#pragma once

#include <vector>
#include <utility>
#include <sstream>
#include <unordered_map>

#include "grammar.hh"


using namespace std;

typedef pair<size_t,size_t> Span;
namespace std {
  template <>
  struct hash<Span>
  {
    size_t
    operator()(Span const& k) const
    {
      return ((hash<size_t>()(k.first)
               ^ (hash<size_t>()(k.second) << 1)) >> 1);
    }
  };
}

namespace Parse {

void visit(vector<Span>& p,
           size_t i, size_t l, size_t r, size_t x=0)
{
  for (size_t span = i; span <= r-x; span++) {
    for (size_t k = l; k <= r-span; k++) {
      p.push_back(Span(k,k+span));
    }
  }
}

struct ChartItem
{
             Span span;
   G::Rule const* rule;
    vector<ChartItem*> tails;
           size_t dot;

  ChartItem(G::Rule* r)
  {
    rule = r;
    dot = 0;
  }

  ChartItem(ChartItem const& o)
  {
    span.first = o.span.first; 
    span.second = o.span.second; 
    rule = o.rule;
    for (auto it: o.tails)
      tails.push_back(it);
    dot = o.dot;
  }
};

struct Chart
{
  size_t n_;
  unordered_map<Span, vector<ChartItem*> > m_;
  unordered_map<string,bool> b_;

  vector<ChartItem*>& at(Span s)
  {
    return m_[s];
  }

  string h(ChartItem* item, Span s)
  {
    ostringstream ss;
    ss << item->rule->lhs->symbol;
    ss << s.first;
    ss << s.second;
    return ss.str();
  }

  void add(ChartItem* item, Span s)
  {
    m_[s].push_back(item); 
    b_[h(item, s)] = true;
  }

  Chart(size_t n)
  {
  }
};


void init(vector<G::T> const& in, size_t n, Chart& active,  Chart& passive, G::Grammar const& g)
{
  for (auto rule: g.flat) {
  }
}


} // 

