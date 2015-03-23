#!/usr/bin/env ruby

require 'zipf'
require_relative 'parse'

def main
  fn = '../example/toy/in'
  #fn = '../example/glue/in'
  STDERR.write "> reading input from #{fn}\n"
  input = ReadFile.new(fn).readlines_strip.first.split
  n = input.size

  STDERR.write "> reading grammar\n"
  grammar = Grammar::Grammar.new '../example/toy/grammar'
  #grammar = Grammar::Grammar.new '../example/toy/grammar-test'
  #grammar = Grammar::Grammar.new '../example/glue/grammar'

  STDERR.write ">> adding glue grammar\n"
  grammar.add_glue_rules

  #STDERR.write ">> adding pass-through grammar\n"
  #grammar.add_pass_through_rules input

  STDERR.write "> initializing charts\n"
  passive_chart = Parse::Chart.new n
  active_chart = Parse::Chart.new n
  Parse::init input, n, active_chart, passive_chart, grammar

  STDERR.write "> parsing\n"
  Parse::parse input, n, active_chart, passive_chart, grammar

  STDERR.write "\n---\npassive chart\n"
  Parse::visit(1, 0, n) { |i,j| k=0; STDERR.write "#{i},#{j}\n"; passive_chart.at(i,j).each { |item| STDERR.write " #{k} #{item.to_s}\n"; k+=1 }; STDERR.write "\n" }

  weights_fn = '../example/toy/weights.toy'
  #weights_fn = nil
  weights = nil
  if weights_fn
    weights = SparseVector.from_kv(ReadFile.read(weights_fn), ' ', "\n")
  else
    weights = SparseVector.new
  end

  puts passive_chart.to_hg.to_json weights
end


main

