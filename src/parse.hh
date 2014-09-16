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
      for (auto jt: it->second)
        jt->repr(os); os << endl;
    }

    return os;
  }
};

bool
scan(ChartItem* item, vector<symbol_t> in, size_t limit, Chart& passive)
{
  //cout << "S1" << endl;
  while (item->dot < item->rule->rhs.size() &&
         item->rule->rhs[item->dot]->type() == G::TERMINAL)  {
  //cout << "S2" << endl;
    if (item->span.second == limit)  return false;
  //cout << "S3" << endl;
    if (item->rule->rhs[item->dot]->symbol() == in[item->span.second]) {
  //cout << "S4" << endl;
      item->dot++;
  //cout << "S5" << endl;
      item->span.second++;
  //cout << "S6" << endl;
    } else {
  //cout << "S7" << endl;
      return false;
    }
  }
  //cout << "S8" << endl;
  return true;
}


void
init(vector<symbol_t> const& in, size_t n, Chart& active,  Chart& passive, G::Grammar const& g)
{
  for (auto rule: g.flat) {
    size_t j = 0;
    for (auto it: in) {
      if (it == rule->rhs.front()->symbol()) {
        cout << it << " " << j << j+rule->rhs.size() << endl;
        Span span(j, j+rule->rhs.size());
        passive.add(new ChartItem(rule, span, rule->rhs.size()), span);
        cout << "new passive item [1] " << *passive.at(span).back() << endl;
      }
      j++;
    }
  }
}

void
parse(vector<symbol_t> const& in, size_t n, Chart& active, Chart& passive, G::Grammar const& g)
{
  vector<Span> spans;
  Parse::visit(spans, 1, 0, n);
  for (auto span: spans) {

  cout << "Span (" << span.first << "," << span.second << ")" << endl;

  for (auto it: g.start_terminal)  {
    ChartItem* item = new ChartItem(it, Span(span.first,span.first), 0);
    if (scan(item, in, span.second, passive)
        && span.first + item->rule->rhs.size() <= span.second) {
      active.add(item, span);
      cout << "new active item [1] " << *active.at(span).back();
    }
  }

  for (auto it: g.start_non_terminal) {
    if (it->rhs.size() > span.second-span.first) continue;
    active.add(new ChartItem(it, Span(span.first,span.first), 0), span);
    cout << "new active item [2] " << *active.at(span).back();
  }

  set<symbol_t> new_symbols;
  vector<ChartItem*> remaining_items;

  while (true) {
    cout << "active size at (" << span.first << "," << span.second << ") " << active.at(span).size() << endl;
    cout << "passive size at (" << span.first << "," << span.second << ") " << passive.at(span).size() << endl;
    if (active.at(span).empty()) break;
    ChartItem* item = active.at(span).back();
    cout << "current item " << *item;
    active.at(span).pop_back();
    bool advanced = false;
    vector<Span> spans2;
    Parse::visit(spans2, 1, span.first, span.second, 1);
    for (auto span2: spans2) {
      cout << "A" << endl;
      if (item->rule->rhs[item->dot]->type() == G::NON_TERMINAL) {
      cout << "B" << endl;
        if (passive.has_at(item->rule->rhs[item->dot]->symbol(), span2)) {
      cout << "C" << endl;
          if (span2.first == item->span.second) {
      cout << "D" << endl;
            ChartItem* new_item = new ChartItem(*item);
            cout << "D1" << endl;
            new_item->span.second = span2.second;
            cout << "D2" << endl;
            new_item->dot++;
            cout << "D3" << endl;
            new_item->tails_spans.push_back(span2);
            cout << "D4" << endl;
            if (scan(new_item, in, span.second, passive)) {
      cout << "E" << endl;
              if (new_item->dot == new_item->rule->rhs.size()) {
      cout << "F" << endl;
                if (new_item->span.first == span.first && new_item->span.second == span.second) {
      cout << "G" << endl;
      cout << "H" << endl;
                  new_symbols.insert(new_item->rule->lhs->symbol());
                  passive.add(new_item, span);
                  cout << "new passive item [2] " << *new_item;
                  advanced = true;
                }
              } else {
                if (new_item->span.second+(new_item->rule->rhs.size()-new_item->dot) <= span.second) {
                  active.add(new_item, span);
                  cout << "new active item [3] " << *new_item;
                }
              }
            }
            cout << "I" << endl;
          }
        }
      }
    }
            cout << "J" << endl;
    if (!advanced) {
            cout << "K" << endl;
      remaining_items.push_back(item);
    }
  }

  for (auto new_sym: new_symbols) {
    cout << "new sym "  << new_sym << endl;
    for (auto rem_item: remaining_items) {
      if (rem_item->dot != 0 ||
          rem_item->rule->rhs[rem_item->dot]->type() != G::NON_TERMINAL) {
        continue;
        cout << "K1" << endl;
      }
        cout << "K2" << endl;
      if (rem_item->rule->rhs[rem_item->dot]->symbol() == new_sym) {
        cout << "K3" << endl;
        ChartItem* new_item = new ChartItem(*rem_item);
        cout << "K31" << endl;
        //new_item->tails_spans[new_item->dot-1] = span;
        new_item->tails_spans.push_back(span);
        new_item->dot++;
        cout << "K32" << endl;
        if (new_item->dot == new_item->rule->rhs.size()) {
        cout << "K4" << endl;
          new_symbols.insert(new_item->rule->lhs->symbol());
          passive.add(new_item, span);
        }
      }
    }
  }

  cout << "L" << endl;
  cout << "-------------------" << endl;
  cout << endl;
  }

  //cout << "ACTIVE" << endl << active << endl;
  cout << "PASSIVE" << endl << passive << endl;
}

} //

