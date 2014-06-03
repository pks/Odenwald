#!/usr/bin/env ruby

require_relative 'hg'

semiring = ViterbiSemiring.new
hypergraph, nodes_by_label, _ = HG::read_hypergraph_from_json('example/json/test.json', semiring, true)
path, score = HG::viterbi_path hypergraph, nodes_by_label['root'], semiring
path.each { |e|
  #puts e.to_s
  puts " "+e.rule.to_s
}
s, score = HG::viterbi_string hypergraph, nodes_by_label['root'], semiring
puts s


