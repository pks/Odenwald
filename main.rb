#!/usr/bin/env ruby

require 'trollop'
require 'xmlsimple'
require_relative 'parse'


def read_grammar fn, add_glue, add_pass_through, input=nil
  STDERR.write "> reading grammar '#{fn}'\n"
  grammar = Grammar::Grammar.new fn
  if add_glue
    STDERR.write ">> adding glue rules\n"
    grammar.add_glue
  end
  if add_pass_through
    STDERR.write ">> adding pass-through rules\n"
    grammar.add_pass_through input
  end
  return grammar
end

def main
  cfg = Trollop::options do
    opt :input,            "", :type => :string, :default => '-',   :short => '-i'
    opt :grammar,          "", :type => :string, :default => nil,   :short => '-g'
    opt :weights,          "", :type => :string, :default => nil,   :short => '-w'
    opt :add_glue,         "", :type => :bool,   :default => false, :short => '-l'
    opt :add_pass_through, "", :type => :bool,   :default => false, :short => '-p'
  end

  grammar = nil
  if cfg[:grammar]
    grammar = read_grammar cfg[:grammar], cfg[:add_glue], cfg[:add_pass_through]
  end

  STDERR.write "> reading input from '#{cfg[:input]}'\n"
  ReadFile.readlines_strip(cfg[:input]).each { |input|

    x = XmlSimple.xml_in(input)
    input = x['content'].split
    n = input.size

    if x['grammar']
      grammar = read_grammar x['grammar'], cfg[:add_glue], cfg[:add_pass_through], input
    end

    STDERR.write "> initializing charts\n"
    passive_chart = Parse::Chart.new n
    active_chart = Parse::Chart.new n
    Parse::init input, n, active_chart, passive_chart, grammar

    STDERR.write "> parsing\n"
    Parse::parse input, n, active_chart, passive_chart, grammar

    weights = SparseVector.from_kv(ReadFile.read(cfg[:weights]), ' ', "\n")
    if !weights
      weights = SparseVector.new
    end

    hypergraph = passive_chart.to_hg weights

    STDERR.write "> viterbi\n"
    semiring = ViterbiSemiring.new
    path, score = HG::viterbi_path hypergraph, hypergraph.nodes_by_id[-1], semiring
    s = HG::derive path, path.last.head, []
    STDERR.write " #{s.map { |i| i.word }.join ' '} ||| #{Math.log score}\n"
  }
end


main

