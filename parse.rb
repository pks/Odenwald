#!/usr/bin/env ruby

require_relative './grammar.rb'


class Chart

  def initialize n
    @m = []
    (n+1).times {
      _ = []
      (n+1).times { _ << [] }
      @m << _
    }
    @b = {}
  end

  def at i, j
    @m[i][j]
  end

  def add item, i, j
    at(i,j) << item
    @b["#{i},#{j},#{item.lhs.symbol}"] = true
  end

  def has symbol, i, j
    return @b["#{i},#{j},#{symbol}"]
  end
end

class Span
  attr_accessor :left, :right

  def initialize left=nil, right=nil
    @left = left
    @right = right
  end
end

class Item < Rule
  attr_accessor :lhs, :rhs, :span, :dot

  def initialize rule_or_item, left, right, dot
    @lhs = rule_or_item.lhs
    @rhs = rule_or_item.rhs.dup
    @span = Span.new left, right
    @dot = dot
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} [dot@#{@dot}] [arity=#{arity}] (#{@span.left}, #{@span.right})"
  end
end

def init input, n, active_chart, passive_chart, grammar
  grammar.flat.each { |r|
    input.each_index { |i|
      if input[i, r.rhs.size] == r.rhs.map { |x| x.word }
        new_item = Item.new r, i, i+r.rhs.size, r.rhs.size
        passive_chart.add new_item, i, i+r.rhs.size
      end
    }
  }
end

def scan item, input, limit, passive_chart
  while item.rhs[item.dot].class == T 
    return false if item.span.right==limit
    if item.rhs[item.dot].word == input[item.span.right]
      item.dot += 1
      item.span.right += 1
    else
      break
    end
  end
  return true
end

def visit i, l, r, x=0
  i.upto(r-x) { |span|
    l.upto(r-span) { |k|
      yield k, k+span
    }
  }
end

def parse input, n, active_chart, passive_chart, grammar
  visit(1, 0, n) { |i,j|

    STDERR.write " span(#{i},#{j})\n"

    # try to apply rules starting with T
    grammar.startt.select { |r| r.rhs.first.word == input[i] }.each { |r|
      new_item = Item.new r, i, i, 0
      active_chart.at(i,j) << new_item if scan new_item, input, j, passive_chart
    }

    # seed active chart
    grammar.startn.each { |r|
      next if r.rhs.size > j-i
      active_chart.at(i,j) << Item.new(r, i, i, 0)
    }

    active_chart.at(i,j).each { |active_item|
      visit(1, i, j, 1) { |k,l|
        if passive_chart.has active_item.rhs[active_item.dot].symbol, k, l
          if k == active_item.span.right
            new_item = Item.new active_item, active_item.span.left, l, active_item.dot+1
            if scan new_item, input, j, passive_chart
              if new_item.dot == new_item.rhs.size # done with item -> passive chart
                if new_item.span.left == i && new_item.span.right == j
                  passive_chart.add new_item, i, j
                end
              else
                if new_item.rhs[new_item.dot].class == NT
                  if new_item.span.right+(new_item.rhs.size-(new_item.dot)) <= j
                    active_chart.at(i,j) << new_item
                  end
                end
              end
            end
          end
        end
      }
    }

    # 'self-filling' step
    active_chart.at(i,j).each { |active_item|
      next if active_item.dot!=0
      next if active_item.rhs[active_item.dot].class!=NT
      if passive_chart.has active_item.rhs[active_item.dot].symbol, i, j
        new_item = Item.new active_item, i, j, active_item.dot+1
        passive_chart.add new_item, i, j if new_item.dot==new_item.rhs.size
      end
    }
  }
end

def main
  STDERR.write "> reading input from TODO\n"
  #input = 'ich sah ein kleines haus'.split
  #input = 'lebensmittel schuld an europäischer inflation'.split
  input = 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'.split
  n = input.size

  STDERR.write "> reading grammar\n"
  grammar = Grammar.new 'example/grammar.3.gz'
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

  puts "\n---\npassive chart"
  visit(1, 0, 5) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts ' '+item.to_s }; puts }
end


main

