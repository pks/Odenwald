#!/usr/bin/env ruby

require_relative './grammar.rb'
STDOUT.sync = true


class Chart

  def initialize n
    @m = []
    (n+1).times {
      _ = []
      (n+1).times { _ << [] }
      @m << _
    }
  end

  def at i, j
    @m[i][j]
  end

  def add rule_or_item, i, j, right, dot=0
    item = Item.new(rule_or_item)
    item.span.left = i
    item.span.right = right
    item.dot = dot
    at(i, j) << item
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

  def initialize rule_or_item
    @lhs = rule_or_item.lhs.dup
    @rhs = rule_or_item.rhs.dup
    if rule_or_item.class == Item
      @span = Span.new rule_or_item.span.left, rule_or_item.span.right
      @dot = rule_or_item.dot
    else
      @span = Span.new
      @dot = 0
    end
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} [dot@#{@dot}] [arity=#{arity}] (#{@span.left}, #{@span.right})"
  end
end

def visit n, depth, skip=0
  (depth-skip).times { |i|
    i += skip
    0.upto(n-(i+1)) { |j|
      yield j, j+i+1
    }
  }
end

def init active_chart, passive_chart, grammar, input, n
  # pre-fill passive chart w/ 0-arity rules
  #grammar.rules.select { |r| r.rhs.first.class==T }.each { |r|
  grammar.rules.select { |r| r.arity==0 }.each { |r|
    input.each_index.select { |i| input[i].word==r.rhs.first.word }.each { |j|
      k = 1
      if r.rhs.size > 1
        z = r.rhs.index { |i| i.class==NT }
        if z
          z -= 1
        else
          z = r.rhs.size-1
        end
        if input[j..j+z].map { |i| i.word } == r.rhs[0..z].map { |i| i.word }
          k = z+1
        else
          next
        end
      end
      if k == r.rhs.size
        passive_chart.add(r, j, j+k, j+k, k)
      #else
      #  (j+k).upto(n) { |l| active_chart.add r, j, l, j+k, k }
      end
    }
  }
  # seed active chart
  #s = grammar.rules.reject { |r| r.rhs.first.class!=NT }
  #visit(n, n, 1) { |i,j|
  #  s.each { |r| active_chart.add(r, i, j, i) }
  #}
end

def scan item, passive_chart, input
  while item.rhs[item.dot].class == T
    break if item.span.right > input.size-1
    if item.rhs[item.dot].word == input[item.span.right].word
      item.dot += 1
      item.span.right += 1
      break if item.dot == item.rhs.size
    else
      break
    end
  end
end

def parse i, j, n, active_chart, passive_chart, input, grammar
  grammar.rules.each { |r|
    if r.rhs.first.class==T&&r.rhs.first.word==input[i].word
      new_item = Item.new r
      scan new_item, passive_chart, input
    end
  }
  #active_chart.at(i,j).each { |active_item|
    1.upto(n) { |span|
      break if span==(j-i)
      i.upto(j-span) { |k|
        STDERR.write " #{k},#{k+span}\n"
        <<-DOC
        passive_chart.at(k, k+span).each { |passive_item|
          if active_item.rhs[active_item.dot].class==NT && passive_item.lhs.symbol == active_item.rhs[active_item.dot].symbol
            next if not active_item.span.right==passive_item.span.left
            new_item = Item.new active_item
            new_item.span.right = passive_item.span.right
            new_item.dot += 1
            scan new_item, passive_chart, input
            if new_item.dot == new_item.rhs.size
              passive_chart.at(i,j) << new_item if new_item.span.left==i&&new_item.span.right==j
            else
              # item is dead unless it has an NT to fill
              active_chart.at(i,j) << new_item if new_item.span.left==i&&new_item.rhs[new_item.dot].class==NT
            end
          end
        }
        DOC
      }
    }
  #}
  # self-filling
  to_add_active = []
  to_add_passive = []
  passive_chart.at(i,j).each { |passive_item|
    active_chart.at(i,j).each { |active_item|
      next if active_item.rhs[active_item.dot].class!=NT
      if passive_item.lhs.symbol == active_item.rhs[active_item.dot].symbol
        next if not active_item.span.right==passive_item.span.left
        new_item = Item.new active_item
        new_item.span.right = passive_item.span.right
        new_item.dot += 1
        scan new_item, passive_chart, input
        if new_item.dot == new_item.rhs.size
          to_add_passive << new_item if new_item.span.left==i&&new_item.span.right==j
        else
          to_add_active << new_item
        end
      end
    }
  }
  to_add_active.each { |item| active_chart.at(i,j) << item }
  to_add_passive.each { |item| passive_chart.at(i,j) << item }
end

def preprocess s
  s.split.map { |i| T.new i }
end

def main
  input = preprocess 'ich sah ein kleines haus'
  #input = preprocess 'lebensmittel schuld an europäischer inflation'
  #input = preprocess 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'
  n = input.size

  puts 'reading grammar'
  g = Grammar.new 'example/grammar'
  #g = Grammar.new 'example/grammar.x'
  #g = Grammar.new 'example/grammar.3.gz' # 4th segment of newstest2008

  puts 'adding glue rules'
  g.add_glue_rules

  #puts 'adding pass-through rules'
  #g.add_pass_through_rules input

  puts 'initializing charts'
  passive_chart = Chart.new n
  active_chart = Chart.new n
  init active_chart, passive_chart, g, input, n

  puts 'parsing'
  visit(n, n, 1) { |i,j|
    STDERR.write "span (#{i}, #{j})\n"
    parse i, j, n, active_chart, passive_chart, input, g
  }

  #puts "---\npassive chart"
  #visit(n, n, 0) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts ' '+item.to_s if item.span.left==i&&item.span.right==j }; puts }
end


main

