#include "parse.hh"

int main(int argc, char** argv)
{
  //string in("ich sah ein kleines haus");
  //string in("europa bildet den ersten oder zweiten markt für die zehn am häufigsten von indien exportierten produkte , erklärte der europäische kommissar weiter . die asiatischen und europäischen giganten tauschen jährlich güter im wert von 47 milliarden euro und dienstleistungen im wert von 10 milliarden euro aus , hatte diese woche daniéle smadja , vorsitzende der abordnung der europäischen kommission in neu delhi , erklärt , und bedauert , dass der gegenseitige handel sein potential noch nicht ausgeschöpft hat . die eu und indien treffen sich am freitag zu ihrem achten diplomatischen in neu delhi , bei dem premierminister manmohan singh und der präsident der europäischen kommission josé manuel durao barrosso anwesend sein werden .");
  //string in("aber schon bald nach seinem eintritt kam der erste große erfolg .");
  string in("lebensmittel schuld an europäischer inflation");
  vector<symbol_t> tok = util::tokenize(in);
  size_t n = tok.size();
  G::Vocabulary v;
  G::Grammar g(argv[1], v);
  g.add_glue(v);
  Parse::Chart active(n);
  Parse::Chart passive(n);
  init(tok, n, active, passive, g);
  parse(tok, n, active, passive, g);
}

