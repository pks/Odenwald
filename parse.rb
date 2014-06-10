#!/usr/bin/env ruby

require 'nlp_ruby'
require_relative 'grammar'


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

Span = Struct.new(:left, :right)

class Item < Grammar::Rule
  attr_accessor :left, :right, :tail_spans, :dot

  def initialize rule_or_item, left, right, dot
    @lhs = Grammar::NT.new(rule_or_item.lhs.symbol, rule_or_item.lhs.index)
    @left = left
    @right = right
    @rhs = []
    @tail_spans = {}
    @map = (rule_or_item.map ? rule_or_item.map.dup : [])
    rule_or_item.rhs.each_with_index { |x,i|
      if x.class == Grammar::T
        @rhs << Grammar::T.new(x.word)
      end
      if x.class == Grammar::NT
        @rhs << Grammar::NT.new(x.symbol, x.index)
        begin
          @tail_spans[i] = rule_or_item.tail_spans[i].dup
        rescue
          @tail_spans[i] = Span.new(-1, -1)
        end
      end
    }
    @dot = dot
    @target = rule_or_item.target
  end

  def to_s
    "(#{@left}, #{@right}) [#{tail_spans.map{|k,v| k.to_s+'('+v.left.to_s+','+v.right.to_s+')'}.join ' '}] {#{@map.to_s.delete('[]')}} #{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} [dot@#{@dot}] ||| #{@target.map{|x|x.to_s}.join ' '}"
  end
end

def init input, n, active_chart, passive_chart, grammar
  grammar.flat.each { |r|
    input.each_index { |i|
      if input[i, r.rhs.size] == r.rhs.map { |x| x.word }
        passive_chart.add Item.new(r, i, i+r.rhs.size, r.rhs.size), i, i+r.rhs.size
      end
    }
  }
end

def scan item, input, limit, passive_chart
  while item.rhs[item.dot].class == Grammar::T
    return false if item.right==limit
    if item.rhs[item.dot].word == input[item.right]
      item.dot += 1
      item.right += 1
    else
      return false
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
 
    # parse
    new_symbols = []
    remaining_items = []
    while !active_chart.at(i,j).empty?
      active_item = active_chart.at(i,j).pop
      advanced = false
      visit(1, i, j, 1) { |k,l|
        if passive_chart.has active_item.rhs[active_item.dot].symbol, k, l
          if k == active_item.right
            new_item = Item.new active_item, active_item.left, l, active_item.dot+1
            new_item.tail_spans[new_item.dot-1] = Span.new(k,l)
            if scan new_item, input, j, passive_chart
              if new_item.dot == new_item.rhs.size
                if new_item.left == i && new_item.right == j
                  new_symbols << new_item.lhs.symbol if !new_symbols.include? new_item.lhs.symbol
                  passive_chart.add new_item, i, j
                  advanced = true
                end
              else
                if new_item.right+(new_item.rhs.size-(new_item.dot)) <= j
                  active_chart.at(i,j) << new_item
                  advanced = true
                end
              end
            end
          end
        end
      }
      if !advanced
        remaining_items << active_item
      end
    end

    # 'self-filling' step
    new_symbols.each { |s|
      remaining_items.each { |active_item|
        next if active_item.dot!=0
        next if active_item.rhs[active_item.dot].class!=Grammar::NT
        if active_item.rhs[active_item.dot].symbol == s
          new_item = Item.new active_item, i, j, active_item.dot+1
          new_item.tail_spans[new_item.dot-1] = Span.new(i,j)
          if new_item.dot==new_item.rhs.size
            new_symbols << new_item.lhs.symbol if !new_symbols.include? new_item.lhs.symbol
            passive_chart.add new_item, i, j
          end
        end
      }
    }

  }
end

