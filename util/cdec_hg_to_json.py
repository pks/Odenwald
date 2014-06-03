#!/usr/bin/env python2

import cdec
import sys, argparse

def hg2json(hg, weights):
  """
  output a JSON representation of a cdec hypegraph
  (see http://aclweb.org/aclwiki/index.php?title=Hypergraph_Format )
  """
  res = ''
  res += "{\n"
  res += '"weights":{'+"\n"
  a = []
  for i in weights:
    a.append( '"%s":%s'%(i[0], i[1]) )
  res += ", ".join(a)+"\n"
  res += "},\n"
  res += '"nodes":'+"\n"
  res += "[\n"
  a = []
  a.append( '{ "label":"root", "cat":"root" }' )
  for i in hg.nodes:
    a.append( '{ "label":"%s", "cat":"%s", "left":%d, "right":%d }'%(i.id, i.cat, i.span[0], i.span[1]) )
  res += ",\n".join(a)+"\n"
  res += "],\n"
  res += '"edges":'+"\n"
  res += "[\n"
  a = []
  for i in hg.edges:
    s = "{"
    s += '"head":"%s"'%(i.head_node.id)
    s += ', "rule":"%s"'%(i.trule)
    s += ', "left":%d'%(i.span[0])
    s += ', "right":%d'%(i.span[1])
    #s += ', "leftx":%d'%(i.src_span[0])
    #s += ', "rightx":%d'%(i.src_span[1])
    s += ', "spans":"'
    q = 0
    for z in i.tail_nodes:
      s+= "%s|||%d|||(%d,%d);"%(z.cat, q, z.span[0], z.span[1])
      q += 1
    s += '"'
    xs = ' "f":{'
    b = []
    for j in i.feature_values:
      b.append( '"%s":%s'%(j[0], j[1]) )
    xs += ", ".join(b)
    xs += "},"
    c = []
    for j in i.tail_nodes:
      c.append( '"'+str(j.id)+'"' )
    if len(c) > 0:
      s += ', "tails":[ %s ],'%(",".join(c))
    else:
      s += ', "tails":[ "root" ],'
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
  args = parser.parse_args()
  with open(args.config) as config:
    config = config.read()
  decoder = cdec.Decoder(config)
  decoder.read_weights(args.weights)
  ins = sys.stdin.readline().strip()
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

