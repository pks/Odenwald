#!/usr/bin/env ruby

require_relative './grammar.rb'

# interescts a WFSA (WFST) with a WSCFG to produce a hypergraph



class Chart
  attr_accessor :m
  
  def initialize input
    @m = []
    (input.size+1).times {
      _ = []
      (input.size+1).times { _ << [] }
      @m << _
    }
  end

  def at span
    return @m[span.left][span.right]
  end

  def at i,j
    return @m[i][j]
  end
end

class Item < Rule
  attr_accessor :lhs, :rhs, :span, :dot

  def initialize rule, dot=-1
    @lhs = rule.lhs.dup
    @rhs = rule.rhs.dup
    @span = Span.new rule.span.left, rule.span.right
    @dot = dot
    if rule.class==Item
      @dot = rule.dot
    end
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} (dot:#{@dot}) (a:#{arity}) (#{@span.left}, #{@span.right})"
  end
end



#def shift
#end
#def reduce
#end
#def init
  #axioms,goals etc
#end
#def parse
#end


g = Grammar.new 'grammar'
input = "ich sah ein kleines haus".split.map { |i| Terminal.new i }
#start =  NonTerminal.new 'S'

passive_chart = Chart.new input
active_chart = Chart.new input



# 1st row: arity0 rules
unary_rules = g.rules.reject { |r| r.arity>0 }
input.each_with_index { |t,j|
  unary_rules.each { |r|
    if r.rhs.first.w == t.w
      passive_chart.at(j,j+1) << Item.new(r)
      passive_chart.at(j,j+1).last.span.left = j
      passive_chart.at(j,j+1).last.span.right = j+1
      passive_chart.at(j,j+1).last.dot = 1
    end
  }
}



# pre-fill active chart with non 0arity rules starting with a (single(!)) T
g.rules.reject{|r| r.rhs[0].class==NonTerminal||r.arity==0}.each {|r|
  input.each_with_index { |i,j|
    if i.w==r.rhs.first.w
      (j+2).upto(input.size) { |k| # no empty NT!
        active_chart.at(j,k) << Item.new(r)
        active_chart.at(j,k).last.span.left = j
        active_chart.at(j,k).last.span.right = j+1 # k
        active_chart.at(j,k).last.dot = 1
      }
    end
  }
}
# pre-fill active chart
s = g.rules.reject { |r| r.rhs.first.class!=NonTerminal}#.reject{|r| r.lhs.sym == 'S'}
(input.size).times { |k|
  0.upto(input.size-(k+2)) { |i|
    s.each { |r|
    active_chart.at(i,i+k+2) << Item.new(r)
    active_chart.at(i,i+k+2).last.span.left = i
    active_chart.at(i,i+k+2).last.span.right = i #i+k+2
    active_chart.at(i,i+k+2).last.dot = 0
    }
  }
}



# parse
puts "parse"
def visit i, j, sz, active_chart, passive_chart, g, input
  puts "| #{i},#{j}"

  # SCAN
  active_chart.at(i,j).each { |item|
    if item.rhs[item.dot].class == Terminal
      if item.rhs[item.dot].w == input[item.span.left+item.dot].w
        item.dot += 1
        if item.dot == item.rhs.size
          passive_chart.at(i,j) << Item.new(item)
          passive_chart.at(i,j).last.span.right = item.span.left+item.dot
        end
      end
    end
  }

  1.upto(sz) { |span|
    break if span==(j-i)
    i.upto(j-span) { |k|
      puts "  #{k},#{k+span} (##{span})"
      # COMPLETE
      active_chart.at(i,j).each { |active_item|
      passive_chart.at(k, k+span).each { |passive_item|
        if active_item.rhs[active_item.dot].class==NonTerminal && passive_item.lhs.sym == active_item.rhs[active_item.dot].sym
          next if not active_item.span.right==passive_item.span.left
          active_item.span.right = passive_item.span.right #if passive_item.span.right > active_item.span.right
          active_item.dot += 1
          if active_item.dot == active_item.rhs.size
            passive_chart.at(i,j) << Item.new(active_item)
          end
        end
      }
      }
    }

  # SCAN
  active_chart.at(i,j).each { |item|
    if item.rhs[item.dot].class == Terminal
      if item.rhs[item.dot].w == input[item.span.left+item.dot].w
        item.dot += 1
        if item.dot == item.rhs.size
          passive_chart.at(i,j) << Item.new(item)
          passive_chart.at(i,j).last.span.right = item.span.left+item.dot
        end
      end
    end
  }

  }
end

(input.size).times { |k|
  0.upto(input.size-(k+2)) { |i|
    visit i, i+k+2, input.size, active_chart, passive_chart, g, input 
  }
}
puts "---"

active_chart.at(0,5).each { |item|
  puts item.to_s
}

