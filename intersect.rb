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
    return @m[i][j]
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
  s = grammar.rules.select { |r| r.rhs.first.class==Terminal }
  s.each { |r|
    input.each_index.select { |j| input[j].w==r.rhs.first.w }.each { |j|
      k = 1
      if r.rhs.size > 1
        z = r.rhs.index { |i| i.class==NonTerminal }
        if z
          z -= 1
        else
          z = r.rhs.size-1
        end
        slice = input[j..j+z].map { |i| i.w }
        if slice == r.rhs[0..z].map { |i| i.w }
          k = z+1
        else
          next
        end
      end
      if k == r.rhs.size
        passive_chart.at(j,j+k) << Item.new(r)
        passive_chart.at(j,j+k).last.span.left = j
        passive_chart.at(j,j+k).last.span.right = j+k
        passive_chart.at(j,j+k).last.dot = k
      else
        (j+k).upto(input.size) { |l|
        active_chart.at(j,l) << Item.new(r)
        active_chart.at(j,l).last.span.left = j
        active_chart.at(j,l).last.span.right = j+k
        active_chart.at(j,l).last.dot = k
        }
      end
    }
  }
  # seed active chart
  s = grammar.rules.reject { |r| r.rhs.first.class!=NonTerminal }
  visit(n, n, 1) { |i,j|
    s.each { |r|
      active_chart.at(i,j) << Item.new(r)
      active_chart.at(i,j).last.span.left = i
      active_chart.at(i,j).last.span.right = i
      active_chart.at(i,j).last.dot = 0
    }
  }
end

def scan item, passive_chart, input, i, j
  while item.rhs[item.dot].class == Terminal
    if item.rhs[item.dot].w == input[item.span.left+item.dot].w
      item.dot += 1
      item.span.right += 1
      if item.dot == item.rhs.size
        passive_chart.at(i,j) << Item.new(item)
        passive_chart.at(i,j).last.span.right = item.span.left+item.dot
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
            if active_item.dot == active_item.rhs.size
              passive_chart.at(i,j) << Item.new(active_item)
            end
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
  visit(n, n, 1)  { |i,j| parse i, j, n, active_chart, passive_chart, g, input }
  passive_chart.at(0,5).each { |item| puts item.to_s }
end


main

