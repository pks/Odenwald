#!/usr/bin/env python2

import cdec
import sys, argparse
import json
import gzip


#FIXME new format
def hg2json(hg, weights):
  """
  output a JSON representation of a cdec hypegraph
  """
  res = ''
  res += "{\n"
  res += '"weights":{'+"\n"
  a = []
  for i in weights:
    if i[1] != 0:
      a.append( '"%s":%s'%(i[0], i[1]) )
  res += ", ".join(a)+"\n"
  res += "},\n"
  res += '"nodes":'+"\n"
  res += "[\n"
  a = []
  a.append( '{ "id":0, "cat":"root", "span":[-1,-1] }' )
  for i in hg.nodes:
    a.append('{ "id":%d, "cat":"%s", "span":[%d,%d] }'%(i.id+1, i.cat, i.span[0], i.span[1]))
  res += ",\n".join(a)+"\n"
  res += "],\n"
  res += '"edges":'+"\n"
  res += "[\n"
  a = []
  for i in hg.edges:
    s = "{"
    s += '"head":%d'%(i.head_node.id+1)
    s += ', "rule":%s'%(json.dumps(str(i.trule)))
    # f
    xs = ' "f":{'
    b = []
    for j in i.feature_values:
      b.append( '"%s":%s'%(j[0], j[1]) )
    xs += ", ".join(b)
    xs += "},"
    # tails
    if len(list(i.tail_nodes)) > 0:
      s += ', "tails":[ %s ],'%(",".join([str(n.id+1) for n in i.tail_nodes]))
    else:
      s += ', "tails":[ 0 ],'
    s += xs
    s += ' "weight":%s }'%(i.prob)
    a.append(s)
  res += ",\n".join(a)+"\n"
  res += "]\n"
  res += "}\n"
  return res

def main():
  parser = argparse.ArgumentParser(description='get a proper json representation of cdec hypergraphs')
  parser.add_argument('-c', '--config', required=True, help='decoder configuration')
  parser.add_argument('-w', '--weights', required=True, help='feature weights')
  parser.add_argument('-g', '--grammar', required=False, help='grammar')
  args = parser.parse_args()
  with open(args.config) as config:
    config = config.read()
  decoder = cdec.Decoder(config)
  decoder.read_weights(args.weights)
  ins = sys.stdin.readline().strip()
  if args.grammar:
    with gzip.open(args.grammar) as grammar:
      grammar = grammar.read()
    hg = decoder.translate(ins, grammar=grammar)
  else:
    hg = decoder.translate(ins)

  sys.stderr.write( "input:\n '%s'\n"%(ins) )
  sys.stderr.write( "viterbi translation:\n '%s'\n"%(hg.viterbi()) )
  num_nodes = 0
  for i in hg.nodes: num_nodes+=1
  sys.stderr.write( "# nodes = %s\n"%(num_nodes) )
  num_edges = 0
  for i in hg.edges: num_edges+=1
  sys.stderr.write( "# edges = %s\n"%(num_edges) )
  sys.stderr.write( "viterbi score = %s\n"%(round(hg.viterbi_features().dot(decoder.weights), 2)) )

  print hg2json(hg, decoder.weights)


if __name__=="__main__":
  main()

