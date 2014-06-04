#!/usr/bin/env ruby

require_relative 'hg'



semiring = ViterbiSemiring.new
hypergraph, nodes_by_label, _ = HG::read_hypergraph_from_json('example/json/test.json', semiring, true)
path, _ = HG::viterbi_path hypergraph, nodes_by_label['root'], semiring
s = HG::derive path, path.last.rule.lhs, []
puts s.map { |i| i.word }.join ' '

