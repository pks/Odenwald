#!/usr/bin/env python2

import cdec
import sys, argparse
import json, gzip

def hg2json(hg, weights):
  """
  hackishly output a JSON representation of a cdec hypegraph

  TODO: cdec now uses boost's serialization instead of JSON
        (github repo @b88176dc4fd53480e77d601ff63bf5300cf8fc7f
         still uses the old format)
  """
  res = ''
  res += "{\n"
  # rules
  res += '"rules":[\n'
  rules = []
  for i in hg.edges:
    s = json.dumps(str(i.trule))
    try:
      rules.index(s)
    except:
      rules.append(s)
  res += ",\n".join(rules)
  res += "\n],\n"
  # nodes
  res += '"nodes":'+"\n"
  res += "[\n"
  a = []
  a.append('{ "id":0, "symbol":"root", "span":[-1,-1] }')
  for i in hg.nodes:
    a.append('{ "id":%d, "symbol":"%s", "span":[%d,%d] }'%(i.id+1, i.cat, i.span[0], i.span[1]))
  res += ",\n".join(a)+"\n"
  res += "],\n"
  # edges
  res += '"edges":'+"\n"
  res += "[\n"
  a = []
  for i in hg.edges:
    s = "{"
    s += '"head":%d'%(i.head_node.id+1)
    s += ', "rule":%s'%(rules.index(json.dumps(str(i.trule))))
    # tails
    if len(list(i.tail_nodes)) > 0:
      s += ', "tails":[ %s ],'%(",".join([str(n.id+1) for n in i.tail_nodes]))
    else:
      s += ', "tails":[ 0 ],'
    s += ' "score":%s }'%(i.prob)
    a.append(s)
  res += ",\n".join(a)+"\n"
  res += "]\n"
  res += "}\n"
  return res

def main():
  parser = argparse.ArgumentParser(description='get a nice json representation of cdec hypergraphs')
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
    if args.grammar.split('.')[-1] == 'gz':
      with gzip.open(args.grammar) as grammar:
        grammar = grammar.read()
    else:
      with open(args.grammar) as grammar:
        grammar = grammar.read()
    hg = decoder.translate(ins, grammar=grammar)
  else:
    hg = decoder.translate(ins)

  sys.stderr.write("input:\n '%s'\n"%(ins))
  sys.stderr.write("viterbi translation:\n '%s'\n"%(hg.viterbi()))
  num_nodes = 0
  for i in hg.nodes: num_nodes+=1
  sys.stderr.write("# nodes = %s\n"%(num_nodes))
  num_edges = 0
  for i in hg.edges: num_edges+=1
  sys.stderr.write("# edges = %s\n"%(num_edges))
  sys.stderr.write("viterbi score = %s\n"%(round(hg.viterbi_features().dot(decoder.weights), 2)))

  print hg2json(hg, decoder.weights).encode('utf-8')

if __name__=="__main__":
  main()

