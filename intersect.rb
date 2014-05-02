#!/usr/bin/env ruby

require_relative './grammar.rb'


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
    "#{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} dot:#{@dot} a:#{arity} (#{@span.left}, #{@span.right})"
  end
end

# set-up
g = Grammar.new 'grammar'
input = "ich sah ein kleines haus".split.map { |i| Terminal.new i }
passive_chart = Chart.new input
active_chart = Chart.new input

# pre-fill passive chart w/ arity-0 rules
g.rules.reject { |r| r.arity>0 }.each { |r|
  input.each_index.select{ |j| input[j].w==r.rhs.first.w }.each { |j|
    k = 1
    if r.rhs.size > 1
      slice = input[j,j+(r.rhs.size-1)].map { |i| i.w }
      if slice == r.rhs.map { |i| i.w }
        k = r.rhs.size
      else
        next
      end
    end
    passive_chart.at(j,j+k) << Item.new(r)
    passive_chart.at(j,j+k).last.span.left = j
    passive_chart.at(j,j+k).last.span.right = j+k
    passive_chart.at(j,j+k).last.dot = k 
  }
}

# pre-fill active chart with non 0arity rules starting with a (single(!)) T
g.rules.reject{|r| r.rhs[0].class==NonTerminal||r.arity==0}.each {|r|
  input.each_with_index { |i,j|
    if i.w==r.rhs.first.w
      (j+2).upto(input.size) { |k| # no empty NT!
        active_chart.at(j,k) << Item.new(r)
        active_chart.at(j,k).last.span.left = j
        active_chart.at(j,k).last.span.right = j+1
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
    active_chart.at(i,i+k+2).last.span.right = i
    active_chart.at(i,i+k+2).last.dot = 0
    }
  }
}

# scan
def scan i, j, active_chart, passive_chart, input
  active_chart.at(i,j).each { |item|
    while item.rhs[item.dot].class == Terminal
      if item.rhs[item.dot].w == input[item.span.left+item.dot].w
        item.dot += 1
        if item.dot == item.rhs.size
          passive_chart.at(i,j) << Item.new(item)
          passive_chart.at(i,j).last.span.right = item.span.left+item.dot
        end
      end
    end
  }
end

# parse
def visit i, j, sz, active_chart, passive_chart, g, input
  puts "| #{i},#{j}"

  # SCAN
  scan i, j, active_chart, passive_chart, input

  1.upto(sz) { |span|
    break if span==(j-i)
    i.upto(j-span) { |k|
      puts "  #{k},#{k+span} (##{span})"
      # COMPLETE
      active_chart.at(i,j).each { |active_item|
      passive_chart.at(k, k+span).each { |passive_item|
        if active_item.rhs[active_item.dot].class==NonTerminal && passive_item.lhs.sym == active_item.rhs[active_item.dot].sym
          next if not active_item.span.right==passive_item.span.left
          active_item.span.right = passive_item.span.right
          active_item.dot += 1
          if active_item.dot == active_item.rhs.size
            passive_chart.at(i,j) << Item.new(active_item)
          end
        end
      }
      }
    }

  scan i, j, active_chart, passive_chart, input

  }
end

# once for each cell
(input.size).times { |k|
  0.upto(input.size-(k+2)) { |i|
    visit i, i+k+2, input.size, active_chart, passive_chart, g, input
  }
}

puts "---"
passive_chart.at(3,5).each { |item|
  puts item.to_s
}

