#!/usr/bin/env ruby

require_relative 'parse'


def main
  STDERR.write "> reading input from TODO\n"
  input = 'ich sah ein kleines haus'.split
  #input = 'lebensmittel schuld an europäischer inflation'.split
  #input = 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'.split
  n = input.size

  STDERR.write "> reading grammar\n"
  grammar = Grammar::Grammar.new '../example/toy/grammar'
  #grammar = Grammar::Grammar.new '../example/toy/grammar-test'
  #grammar = Grammar::Grammar.new '../example/glue/grammar'
  #grammar = Grammar::Grammar.new '../example/3/grammar.3.gz'

  STDERR.write ">> adding glue grammar\n"
  #grammar.add_glue_rules

  STDERR.write ">> adding pass-through grammar\n"
  #grammar.add_pass_through_rules input

  STDERR.write "> initializing charts\n"
  passive_chart = Parse::Chart.new n
  active_chart = Parse::Chart.new n
  Parse::init input, n, active_chart, passive_chart, grammar

  STDERR.write "> parsing\n"
  Parse::parse input, n, active_chart, passive_chart, grammar

  puts "\n---\npassive chart"
  Parse::visit(1, 0, 5) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts " #{j} #{item.to_s}" }; puts }

  weights_file = '../example/toy/weights'
  #weights_file = '../example/glue/weights'
  #weights_file = '../example/3/weights.init'
  weights = SparseVector.from_kv(ReadFile.read(weights_file), ' ', "\n")
  if !weights
    weights = SparseVector.new 
  end

  puts passive_chart.to_hg.to_json weights
end


main

