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
  end

  def at i, j
    @m[i][j]
  end

  def add r, i, j, right, dot=0
    item = Item.new(r)
    item.span.left = i
    item.span.right = right
    item.dot = dot
    at(i, j) << item
  end
end

class Item < Rule
  attr_accessor :lhs, :rhs, :span, :dot

  def initialize rule
    @lhs = rule.lhs.dup
    @rhs = rule.rhs.dup
    @span = Span.new rule.span.left, rule.span.right
    @dot = rule.dot if rule.class==Item
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} [dot@#{@dot}] [arity=#{arity}] (#{@span.left}, #{@span.right})"
  end
end

def visit n, depth, skip=0
  (depth-skip).times { |i|
    i += skip
    0.upto(n-(i+1)) { |j|
      yield j, j+i+1 if block_given?
    }
  }
end

def init active_chart, passive_chart, grammar, input, n
  # pre-fill passive chart w/ 0-arity rules
  grammar.rules.select { |r| r.rhs.first.class==Terminal }.each { |r|
    input.each_index.select { |i| input[i].w==r.rhs.first.w }.each { |j|
      k = 1
      if r.rhs.size > 1
        z = r.rhs.index { |i| i.class==NonTerminal }
        if z
          z -= 1
        else
          z = r.rhs.size-1
        end
        if input[j..j+z].map { |i| i.w } == r.rhs[0..z].map { |i| i.w }
          k = z+1
        else
          next
        end
      end
      if k == r.rhs.size
        passive_chart.add(r, j, j+k, j+k, k)
      else
        (j+k).upto(n) { |l|  active_chart.add(r, j, l, j+k, k) }
      end
    }
  }
  # seed active chart
  s = grammar.rules.reject { |r| r.rhs.first.class!=NonTerminal }
  visit(n, n, 1) { |i,j|
    s.each { |r| active_chart.add(r, i, j, i) }
  }
end

def scan item, passive_chart, input, i, j
  while item.rhs[item.dot].class == Terminal
    if item.rhs[item.dot].w == input[item.span.left+item.dot].w
      item.dot += 1
      item.span.right = item.span.left+item.dot
      if item.dot == item.rhs.size
        passive_chart.add(item, i, j, item.span.left+item.dot, item.dot)
        break
      end
    end
  end
end

def parse i, j, sz, active_chart, passive_chart, g, input
  1.upto(sz) { |span|
    break if span==(j-i)
    i.upto(j-span) { |k|
      active_chart.at(i,j).each { |active_item|
        passive_chart.at(k, k+span).each { |passive_item|
          if active_item.rhs[active_item.dot].class==NonTerminal && passive_item.lhs.sym == active_item.rhs[active_item.dot].sym
            next if not active_item.span.right==passive_item.span.left
            active_item.span.right = passive_item.span.right
            active_item.dot += 1
            scan active_item, passive_chart, input, i, j
            passive_chart.at(i,j) << Item.new(active_item) if active_item.dot==active_item.rhs.size
          end
        }
      }
    }
  }
end

def main
  g = Grammar.new 'grammar'
  input = "ich sah ein kleines haus".split.map { |i| Terminal.new i }
  n = input.size
  passive_chart = Chart.new n
  active_chart = Chart.new n
  init active_chart, passive_chart, g, input, n
  visit(n, n, 1) { |i,j| parse i, j, n, active_chart, passive_chart, g, input }
  visit(n, n, 0) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts item.to_s } }
end


main

