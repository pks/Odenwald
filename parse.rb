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

def visit i, l, r, x=1
  i.upto(r-x) { |span|
    l.upto(r-span) { |k|
      yield k, k+span
    }
  }
end

def parse input, n, active_chart, passive_chart, grammar
  #visit(2, 0, 5) { |i,j|
  2.upto(n) { |span| # outer loops
    0.upto(n-span) { |k|

      puts " span(#{k},#{k+span})"

      # try to apply rules starting with T
      grammar.startt.select { |r| r.rhs.first.word == input[k] }.each { |r|
        new_item = Item.new r, k, k, 0
        active_chart.at(k,k+span) << new_item if scan new_item, input, k+span, passive_chart
      }

      # seed active chart
      grammar.startn.each { |r|
        next if r.rhs.size > span
        active_chart.at(k,k+span) << Item.new(r, k, k, 0)
      }

      active_chart.at(k,k+span).each { |active_item|
        #visit(1, k, k+span) { |k,l|
        1.upto(span-1) { |span2| # inner loops
          k.upto((k+span)-span2) { |l|

            if passive_chart.has active_item.rhs[active_item.dot].symbol, l, l+span2
              if l == active_item.span.right
                new_item = Item.new active_item, active_item.span.left, l+span2, active_item.dot+1
                if scan new_item, input, k+span, passive_chart
                  if new_item.dot == new_item.rhs.size # done with item -> passive chart
                    if new_item.span.left == k && new_item.span.right == k+span
                      passive_chart.add new_item, k, k+span
                    end
                  else
                    if new_item.rhs[new_item.dot].class == NT
                      if new_item.span.right+(new_item.rhs.size-(new_item.dot)) <= k+span
                        active_chart.at(k,k+span) << new_item
                      end
                    end
                  end
                end
              end
            end
          }
        }
      }

      # 'self-filling' step
      # FIXME slow!
      passive_chart.at(k,k+span).each { |passive_item|
        active_chart.at(k,k+span).each { |active_item|
          next if active_item.rhs[active_item.dot].class!=NT
          if passive_item.lhs.symbol == active_item.rhs[active_item.dot].symbol
            next if not active_item.span.right==passive_item.span.left
            new_item = Item.new active_item, active_item.span.left, passive_item.span.right, active_item.dot+1
            scan new_item, input, k+span, passive_chart
            if new_item.dot == new_item.rhs.size
              if new_item.span.left == k && new_item.span.right == k+span
                passive_chart.add new_item, k, k+span
              else
                puts "#{new_item}"
              end
            else
              if new_item.rhs[new_item.dot].class == NT && new_item.span.right+(new_item.rhs.size-(new_item.dot)) <= k+span
                puts "NA@#{k},#{k+span} #{new_item}"
                active_chart.at(k,k+span) << new_item
              end
            end
          end
        }
      }

    }
  }
end

def main
  STDERR.write "> reading input from TODO\n"
  input = 'ich sah ein kleines haus'.split
  #input = 'lebensmittel schuld an europäischer inflation'.split
  #input = 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'.split
  n = input.size

  STDERR.write "> reading grammar\n"
  grammar = Grammar.new 'example/grammar'
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
  visit(1, 0, 5, 0) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts ' '+item.to_s }; puts }
end


main

