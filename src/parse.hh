#pragma once

#include <vector>
#include <utility>
#include <sstream>
#include <unordered_map>
#include <set>

#include "grammar.hh"
#include "util.hh"
#include "types.hh"

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
  for (size_t s = i; s <= r-x; s++) {
    for (size_t k = l; k <= r-s; k++) {
      p.push_back(Span(k,k+s));
    }
  }
}

struct ChartItem
{
             Span span;
   G::Rule const* rule;
     vector<Span> tails_spans;
           size_t dot;

  ChartItem() {}

  ChartItem(G::Rule* r) : rule(r), dot(0) {}

  ChartItem(G::Rule* r, Span s, size_t dot)
    : rule(r), span(s), dot(dot) {}

  ChartItem(ChartItem const& o)
    : span(o.span),
      rule(o.rule),
      tails_spans(o.tails_spans),
      dot(o.dot)
  {
  }

  ostream&
  repr(ostream& os) const
  {
    os << "ChartItem<";
    os << "span=(" << span.first << "," << span.second << "), lhs=";
    rule->lhs->repr(os);
    os << ", dot=" << dot;
    os << ", tails=" << tails_spans.size() << ", ";
    os << "rule=";
    rule->repr(os);
    os << ">";
    os << endl;

    return os;
  }

  friend ostream&
  operator<<(ostream& os, ChartItem item)
  {
    item.repr(os);

    return os;
  }
};

struct Chart
{
  size_t n_;
  map<Span, vector<ChartItem*> > m_;
  unordered_map<string,bool> b_;

  vector<ChartItem*>& at(Span s)
  {
    return m_[s];
  }

  string h(symbol_t sym, Span s)
  {
    ostringstream ss;
    ss << sym;
    ss << s.first;
    ss << s.second;

    return ss.str();
  }

  bool
  has_at(symbol_t sym, Span s)
  {
    return b_[h(sym, s)];
  }

  void add(ChartItem* item, Span s)
  {
    if (m_.count(s) > 0)
      m_[s].push_back(item);
    else {
      m_.insert(make_pair(s, vector<ChartItem*>{item}));
    }
    b_[h(item->rule->lhs->symbol(), s)] = true;
  }

  Chart(size_t n) : n_(n) {}

  friend ostream&
  operator<<(ostream& os, Chart const& chart)
  {
    for (map<Span, vector<ChartItem*> >::const_iterator it = chart.m_.cbegin();
         it != chart.m_.cend(); it++) {
      os << "(" << it->first.first << "," << it->first.second << ")" << endl;
      size_t j = 0;
      for (auto jt: it->second) {
        os << j << " "; jt->repr(os);
        j++;
      }
    }

    return os;
  }
};

bool
scan(ChartItem* item, vector<symbol_t> in, size_t limit, Chart& passive)
{
  while (item->dot < item->rule->rhs.size() &&
         item->rule->rhs[item->dot]->type() == G::TERMINAL)  {
    if (item->span.second == limit)  return false;
    if (item->rule->rhs[item->dot]->symbol() == in[item->span.second]) {
      item->dot++;
      item->span.second++;
    } else {
      return false;
    }
  }

  return true;
}

void
init(vector<symbol_t> const& in, size_t n, Chart& active,  Chart& passive, G::Grammar const& g)
{
  for (auto rule: g.flat) {
    size_t j = 0;
    for (auto it: in) {
      if (it == rule->rhs.front()->symbol()) {
        Span span(j, j+rule->rhs.size());
        passive.add(new ChartItem(rule, span, rule->rhs.size()), span);
      }
      j++;
    }
  }
}

void
parse(vector<symbol_t> const& in, size_t n, Chart& active, Chart& passive, G::Grammar const& g, size_t max_span_size)
{
  vector<Span> spans;
  Parse::visit(spans, 1, 0, n);
  for (auto span: spans) {

  size_t span_size = span.second-span.first;

  cout << "Span (" << span.first << "," << span.second << ")" << endl;

  for (auto it: g.start_terminal)  {
    ChartItem* item = new ChartItem(it, Span(span.first,span.first), 0);
    if (scan(item, in, span.second, passive)
        && span.first + item->rule->rhs.size() <= span.second) {
      active.add(item, span);
    }
  }

  for (auto it: g.start_non_terminal) {
    if (it->rhs.size() > span.second-span.first
        || (span_size>max_span_size)) continue;
    active.add(new ChartItem(it, Span(span.first,span.first), 0), span);
  }

  set<symbol_t> new_symbols;
  vector<ChartItem*> remaining_items;

  while (true) {
    if (active.at(span).empty()) break;
    ChartItem* item = active.at(span).back();
    active.at(span).pop_back();
    while (item->rule->rhs[item->dot]->type() == G::NON_TERMINAL) {
      symbol_t cur_sym = item->rule->rhs[item->dot]->symbol();
    }
  }

  /*while (true) {
    if (active.at(span).empty()) break;
    ChartItem* item = active.at(span).back();
    active.at(span).pop_back();
    bool advanced = false;
    vector<Span> spans2;
    Parse::visit(spans2, 1, span.first, span.second, 1);
    for (auto span2: spans2) {
      if (item->rule->rhs[item->dot]->type() == G::NON_TERMINAL) {
        if (passive.has_at(item->rule->rhs[item->dot]->symbol(), span2)) {
          if (span2.first == item->span.second) {
            ChartItem* new_item = new ChartItem(*item);
            new_item->span.second = span2.second;
            new_item->dot++;
            new_item->tails_spans.push_back(span2);
            if (scan(new_item, in, span.second, passive)) {
              if (new_item->dot == new_item->rule->rhs.size()) {
                if (new_item->span.first == span.first && new_item->span.second == span.second) {
                  new_symbols.insert(new_item->rule->lhs->symbol());
                  passive.add(new_item, span);
                  advanced = true;
                }
              } else {
                if (new_item->span.second+(new_item->rule->rhs.size()-new_item->dot) <= span.second) {
                  active.add(new_item, span);
                }
              }
            }
          }
        }
      }
    }
    if (!advanced) {
      remaining_items.push_back(item);
    }
  }

  for (auto new_sym: new_symbols) {
    for (auto rem_item: remaining_items) {
      if (rem_item->dot != 0 ||
          rem_item->rule->rhs[rem_item->dot]->type() != G::NON_TERMINAL) {
        continue;
      }
      if (rem_item->rule->rhs[rem_item->dot]->symbol() == new_sym) {
        ChartItem* new_item = new ChartItem(*rem_item);
        new_item->tails_spans.push_back(span);
        new_item->dot++;
        if (new_item->dot == new_item->rule->rhs.size()) {
          new_symbols.insert(new_item->rule->lhs->symbol());
          passive.add(new_item, span);
        }
      }
    }
  }*/
  }
}

} //

