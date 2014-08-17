#include "grammar.hh"


namespace G {

/*
 * G::NT
 *
 */
NT::NT(string& s)
{
  s.erase(0, 1); s.pop_back(); // remove '[' and ']'
  istringstream ss(s);
  if (ss >> index) { // [i]
    symbol = "";
    index = stoi(s);

    return;
  } else { // [X]
    symbol = s;
    index = 0;

    return;
  }
  string buf;
  size_t j = 0;
  index = 0; // default
  while (ss.good() && getline(ss, buf, ',')) {
    if (j == 0) {
      symbol = buf;
    } else {
      index = stoi(buf);
    }
    j++;
  }
}

string
NT::repr() const
{
  ostringstream os;
  os << "NT<" << symbol << "," << index << ">";

  return os.str();
}

string
NT::escaped() const
{
  ostringstream os;
  os << "[" << symbol;
  if (index > 0)
    os << "," << index;
  os << "]";

  return os.str();
}

ostream&
operator<<(ostream& os, const NT& nt)
{
  return os << nt.repr();
}

/*
 * G::T
 *
 */
T::T(const string& s)
{
  word = s;
}

string
T::repr() const
{
  ostringstream os;
  os << "T<" << word << ">";

  return os.str();
}

string
T::escaped() const
{
  return util::json_escape(word);
}

ostream&
operator<<(ostream& os, const T& t)
{
  return os << t.repr();
}


/*
 * G::Item
 *
 * Better solve this by inheritance
 *  -> rhs, target as vector<base class> ?
 *
 */
Item::Item(string& s)
{
  if (s.front() == '[' && s.back() == ']') {
    type = NON_TERMINAL;
    nt = new NT(s);
  } else {
    type = TERMINAL;
    t = new T(s);
  }
}

string
Item::repr() const
{
  ostringstream os;
  if (type == TERMINAL)
    os << t->repr();
  else
    os << nt->repr();

  return os.str();
}

string
Item::escaped() const
{
  ostringstream os;
  if (type == TERMINAL)
    os << t->escaped();
  else
    os << nt->escaped();

  return os.str();
}

ostream&
operator<<(ostream& os, const Item& i)
{
  return os << i.repr();
}

/*
 * G::Rule
 *
 */
Rule::Rule(const string& s)
{
  from_s(this, s);
}

void
Rule::from_s(Rule* r, const string& s)
{
  stringstream ss(s);
  size_t j = 0;
  string buf;
  r->arity = 0;
  size_t index = 1;
  vector<G::NT*> rhs_nt;
  r->f = new Sv::SparseVector<string, score_t>();
  while (ss >> buf) {
    if (buf == "|||") { j++; continue; }
    if (j == 0) {        // LHS
      r->lhs = new NT(buf);
    } else if (j == 1) { // RHS
      r->rhs.push_back(new Item(buf));
      if (r->rhs.back()->type == NON_TERMINAL) {
        rhs_nt.push_back(r->rhs.back()->nt);
        r->arity++;
      }
    } else if (j == 2) { // TARGET
      r->target.push_back(new Item(buf));
      if (r->target.back()->type == NON_TERMINAL) {
        r->order.insert(make_pair(index, r->target.back()->nt->index));
        if (r->target.back()->nt->symbol == "")
          r->target.back()->nt->symbol = rhs_nt[r->target.back()->nt->index-1]->symbol;
        index++;
      }
    } else if (j == 3) { // F TODO
      Sv::SparseVector<string, score_t>::from_s(r->f, buf); // FIXME this is slow!!!
    } else if (j == 4) { // A TODO
    } else { 
      // ERROR
    }
    if (j == 4) break;
  }
}

string
Rule::repr() const
{
  ostringstream os;
  os << "Rule<lhs=" << lhs->repr() << \
   ", rhs:{";
  for (auto it = rhs.begin(); it != rhs.end(); it++) {
    os << (**it).repr();
    if (next(it) != rhs.end()) os << " ";
  }
  os << "}, target:{";
  for (auto it = target.begin(); it != target.end(); it++) {
    os << (**it).repr();
    if (next(it) != target.end()) os << " ";
  }
  os << "}" \
   ", f:" << f->repr() << \
   ", arity=" << arity << \
   ", map:" << "TODO" << \
   ">";

  return os.str();
}

string
Rule::escaped() const
{
  ostringstream os;
  os << lhs->escaped() << " ||| ";
  for (auto it = rhs.begin(); it != rhs.end(); it++) {
    os << (**it).escaped();
    if (next(it) != rhs.end()) os << " ";
  }
  os << " ||| ";
  for (auto it = target.begin(); it != target.end(); it++) {
    os << (**it).escaped();
    if (next(it) != target.end()) os << " ";
  }
  os << " ||| ";
  os << f->escaped();
  os << " ||| ";
  os << "TODO(alignment)";

  return os.str();
}

ostream&
operator<<(ostream& os, const Rule& r)
{
  return os << r.repr();
}

/*
 * G::Grammmar
 *
 */
Grammar::Grammar(const string& fn)
{
  ifstream ifs(fn);
  string line;
  while (getline(ifs, line)) {
    G::Rule* r = new G::Rule(line);
    rules.push_back(r);
    if (r->arity == 0)
      flat.push_back(r);
    else if (r->rhs.front()->type == NON_TERMINAL)
      start_nt.push_back(r);
    else
      start_t.push_back(r);
  }
}

ostream&
operator<<(ostream& os, const Grammar& g)
{
  for (const auto it: g.rules)
    os << it->repr() << endl;

  return os;
}

} // namespace G

