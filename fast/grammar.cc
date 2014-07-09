#include "grammar.hh"

namespace Grammar {


string
NT::s()
{
  ostringstream os;
  os << "NT<" << this->symbol << "," << this->index << ">";
  return os.str();
}


} // namespace

