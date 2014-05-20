#!/usr/bin/env ruby

require_relative './grammar.rb'
STDOUT.sync = true


class Chart
  attr_accessor :has

  def initialize n
    @m = []
    (n+1).times {
      _ = []
      (n+1).times { _ << [] }
      @m << _
    }
    @has = {}
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

def preprocess s
  s.split.map { |i| T.new i }
end

def init input, n, active_chart, passive_chart, grammar
  grammar.rules.select { |r| r.arity==0 }.each { |r|
    input.each_index { |i|
      if input[i, r.rhs.size].map{|x|x.word}==r.rhs.map{|x|x.word}
        passive_chart.add Item.new(r), i, i+r.rhs.size, i+r.rhs.size, r.rhs.size
        passive_chart.d["#{i},#{i+r.rhs.size},#{r.lhs.symbol}"] = true
      end
    }
  }
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

def parse input, n, active_chart, passive_chart, grammar
  rules_starting_with_t = grammar.rules.select { |r| r.arity > 0 && r.rhs.first.class==T }
  rules_starting_with_nt = grammar.rules.select { |r| r.rhs.first.class == NT }

  # outer loop
  2.upto(n) { |span|
    0.upto(n-span) { |k|

      # try to apply rules starting with T
      rules_starting_with_t.select { |r| r.rhs.first.word == input[k].word }.each { |r|
        new_item = Item.new r
        new_item.span.left = k
        new_item.span.right = k
        new_item.dot = 0
        scan new_item, passive_chart, input
        active_chart.at(k, k+span) << new_item if new_item.span.right < k+span
      }

      # seed active chart
      rules_starting_with_nt.each { |r|
        # no eps-productions!
        next if r.rhs.size > span
        #puts "#{r.rhs.size} #{span}"
        new_item = Item.new r
        new_item.span.left = k
        new_item.span.right = k
        new_item.dot = 0
        active_chart.at(k, k+span) << new_item
      }

      puts "#{k} #{k+span}"

      # inner loop
      puts "SIZE #{active_chart.at(k,k+span).size}"
      while !active_chart.at(k,k+span).empty?
      active_item = active_chart.at(k, k+span).pop

      1.upto(span-1) { |span2|
        k.upto((k+span)-span2) { |l|

        #puts "passive chart sz #{passive_chart.at(l, l+span2).size} #{l},#{l+span2}"
        #passive_chart.at(l, l+span2).select { |item| item.lhs.symbol==active_item.rhs[active_item.dot].symbol }.each { |passive_item|
        #  if l==0&&span2==2
        #    puts "   #{passive_item}"
        #  end
        if passive_chart.d["#{l},#{l+span2},#{active_item.rhs[active_item.dot].symbol}"]
          if l == active_item.span.right
              new_item = Item.new active_item
              new_item.span.left = active_item.span.left
              new_item.span.right = l+span2
              new_item.dot += 1
              scan new_item, passive_chart, input
              if new_item.dot == new_item.rhs.size
                if new_item.span.left==k&&new_item.span.right==(k+span)
                  #puts "NP@#{k},#{k+span} #{new_item}"
                  passive_chart.at(k,k+span) << new_item
                end
              else
                if new_item.rhs[new_item.dot].class==NT && new_item.span.right < (k+span)
                  #puts "NA@#{k},#{k+span} #{new_item}"
                  active_chart.at(k,k+span) << new_item
                end
              end
          end
        #}
        end

#          # try to advance items
#          passive_chart.at(l, l+span2).each { |passive_item|
#            active_chart.at(k, k+span).select { |active_item| active_item.rhs[active_item.dot].symbol == passive_item.lhs.symbol }.each { |active_item|
#              if passive_item.span.left == active_item.span.right
#                new_item = Item.new active_item
#                new_item.span.left = active_item.span.left
#                new_item.span.right = passive_item.span.right
#                new_item.dot += 1
#                scan new_item, passive_chart, input
#                if new_item.dot == new_item.rhs.size
#                  if new_item.span.left==k&&new_item.span.right==(k+span)
#                    puts "NP@#{k},#{k+span} #{new_item}"
#                    passive_chart.at(k,k+span) << new_item
#                  end
#                else
#                  # item is dead unless it has an NT to fill
#                  if new_item.rhs[new_item.dot].class==NT && new_item.span.right < (k+span)
#                    puts "NA@#{k},#{k+span} #{new_item}"
#                    active_chart.at(k,k+span) << new_item
#                  end
#                end
#              end
#            }
#          }
#

        }
      }

      end

    }
  }
end

def main
  #input = preprocess 'ich sah ein kleines haus'
  #input = preprocess 'lebensmittel schuld an europäischer inflation'
  input = preprocess 'offizielle prognosen sind von nur 3 prozent ausgegangen , meldete bloomberg .'
  n = input.size
  g = Grammar.new 'example/grammar.3.gz'
  #g.add_glue_rules
  passive_chart = Chart.new n
  active_chart = Chart.new n
  init input, n, active_chart, passive_chart, g
  parse input, n, active_chart, passive_chart, g
  #puts "---\npassive chart"
  #visit(n, n, 0) { |i,j| puts "#{i},#{j}"; passive_chart.at(i,j).each { |item| puts ' '+item.to_s if item.span.left==i&&item.span.right==j }; puts }
  #puts "\nactive chart"
  #visit(n, n, 0) { |i,j| puts "#{i},#{j}"; active_chart.at(i,j).each { |item| puts ' '+item.to_s }; puts }

end


main

