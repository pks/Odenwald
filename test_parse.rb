#!/usr/bin/env ruby

require_relative 'parse'


def chart2json passive_chart, n
  id = 0
  spancat2id = {}
  puts "{"
  puts "\"weights\":{ \"logp\":2.0, \"use_shell\":1.0 },"
  puts "\"nodes\":"
  puts "["
  # nodes
  visit(1, 0, n) { |i,j| 
    seen = {}
    passive_chart.at(i,j).each { |item|
     if !seen[item.lhs.symbol]
       puts "{ \"id\":#{id}, \"cat\":\"#{item.lhs.symbol}\", \"span\":[#{i},#{j}] },"
       spancat2id["#{item.lhs.symbol},#{i},#{j}"] = id
       id += 1
     end
     seen[item.lhs.symbol]=true
    }
  }
  puts "{ \"id\":-1, \"cat\":\"root\", \"span\":[-1, -1] }"
  puts "],"
  puts "\"edges\":"
  puts "["
  a = []
  visit(1, 0, n) { |i,j| 
    passive_chart.at(i,j).each { |item|
      if item.tail_spans.empty?
      a << "{ \"head\":#{spancat2id[item.lhs.symbol+','+i.to_s+','+j.to_s]}, \"rule\":\"[#{item.lhs.symbol}] ||| #{item.rhs.map{|x|(x.class==Grammar::NT ? '['+x.symbol+','+x.index.to_s+']' : x.word.gsub('"','\"'))}.join(' ')} ||| #{item.target.map{|x|(x.class==Grammar::NT ? '['+x.symbol+','+(x.index+0).to_s+']' : x.word.gsub('"', '\"'))}.join(' ')}\ |||\", \"tails\":[-1], \"f\":#{item.f.to_s.gsub('=>',':')} }"
      else
      a << "{ \"head\":#{spancat2id[item.lhs.symbol+','+i.to_s+','+j.to_s]}, \"rule\":\"[#{item.lhs.symbol}] ||| #{item.rhs.map{|x|(x.class==Grammar::NT ? '['+x.symbol+','+x.index.to_s+']' : x.word.gsub('"','\"'))}.join(' ')} ||| #{item.target.map{|x|(x.class==Grammar::NT ? '['+x.symbol+','+(x.index+1).to_s+']' : x.word.gsub('"', '\"'))}.join(' ')}\ |||\", \"tails\":[#{item.rhs.zip((0..item.rhs.size-1).map{|q|item.tail_spans[q]}).select{|x|x[0].class==Grammar::NT}.map{|x|spancat2id[x[0].symbol+','+x[1].left.to_s+','+x[1].right.to_s]}.join ', '}], \"f\":#{item.f.to_s.gsub('=>',':')} }"
      end
    }
  }
  puts a.join ",\n"
  puts "]"
  puts "}"
end


def main
  STDERR.write "> reading input from TODO\n"
  #input = 'ich sah ein kleines haus'.split
  #input = 'lebensmittel schuld an europäischer inflation'.split
  input = 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'.split
  n = input.size

  STDERR.write "> reading grammar\n"
  grammar = Grammar::Grammar.new 'example/grammars/grammar.3.gz'
  STDERR.write ">> adding glue grammar\n"
  grammar.add_glue_rules
  STDERR.write ">> adding pass-through grammar\n"
  #grammar.add_pass_through_rules input

  STDERR.write "> initializing charts\n"
  passive_chart = Chart.new n
  active_chart = Chart.new n
  init input, n, active_chart, passive_chart, grammar

  STDERR.write "> parsing\n"
  parse input, n, active_chart, passive_chart, grammar

  #puts "\n---\npassive chart"
  #visit(1, 0, 5) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts " #{j} #{item.to_s}" }; puts }

  chart2json passive_chart, n
end


main

