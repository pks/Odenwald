#!/usr/bin/env ruby

require_relative 'hg'


semiring = ViterbiSemiring.new
hypergraph, nodes_by_id = HG::read_hypergraph_from_json('example/json/test.json', semiring, true)
path, score = HG::viterbi_path hypergraph, nodes_by_id[-1], semiring
#s = HG::derive path, path.last.rule.lhs, []
#puts "#{s.map { |i| i.word }.join ' '} ||| #{score}"

hypergraph.reset
paths = HG::all_paths hypergraph, nodes_by_id[-1]
paths.each { |p|
  s = HG::derive p, p.last.rule.lhs, []
puts "#{s.map { |i| i.word }.join ' '} ||| #{score}"
}

