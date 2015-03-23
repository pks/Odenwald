#!/usr/bin/env ruby

require_relative 'hypergraph'

def main
  # viterbi
  semiring = ViterbiSemiring.new
  hypergraph, nodes_by_id = HG::read_hypergraph_from_json('../example/toy/toy.json', semiring, true)
  path, score = HG::viterbi_path hypergraph, nodes_by_id[-1], semiring
  s = HG::derive path, path.last.head, []
  path.each { |e| puts "#{e.rule}" }
  puts "---"
  puts "#{s.map { |i| i.word }.join ' '}"
  puts Math.log score
  puts

  # all paths
  hypergraph.reset
  paths = HG::all_paths hypergraph, nodes_by_id[-1]
  paths.each_with_index { |p,i|
    s = HG::derive p, p.last.head, []
    puts "#{i+1}. #{s.map { |x| x.word }.join ' '}"
  }
end

main

