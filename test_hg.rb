#!/usr/bin/env ruby

require_relative 'hg'


semiring = ViterbiSemiring.new
hypergraph, nodes_by_id = HG::read_hypergraph_from_json('example/3/3.json', semiring, true)
path, score = HG::viterbi_path hypergraph, nodes_by_id[-1], semiring
s = HG::derive path, path.last.head, []
puts "#{s.map { |i| i.word }.join ' '}"
puts Math.log score
puts
#hypergraph.reset
#paths = HG::all_paths hypergraph, nodes_by_id[-1]
#paths.each_with_index { |p,i|
#  s = HG::derive p, p.last.head, []
#  puts "#{i+1}. #{s.map { |i| i.word }.join ' '}"
#}

