#!/usr/bin/env ruby

require_relative 'hypergraph'

class NewObjSemiring < Semiring
  def initialize
    @add = Proc.new { |a,b| [a,b].min }
    @multiply =  Proc.new { |a,b| a*b }
    @one = 1.0
    @null = 0.0
    @convert = Proc.new { |v| v }
  end
end

def main
  #semiring = NewObjSemiring.new
  semiring = ViterbiSemiring.new
  #h = { "logp" => 0, "use_house" => 100, "use_small" => 100 }
  #model = [[1, SparseVector.from_h(h)]]
  hypergraph, nodes_by_id = HG::read_hypergraph_from_json('739.json.gz', semiring, true)
  path, score = HG::viterbi_path hypergraph, nodes_by_id[-1], semiring
  path.each { |e| puts "#{e.head.to_s}\t#{e.rule.to_s}" }
  puts path.size
  puts "---"
  s = HG::derive path, path.last.head, []
  puts s.size
  puts "#{s.map { |i| i.word }.join ' '}"
  puts Math.log(score)
  puts

end

main

