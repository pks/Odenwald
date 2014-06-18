require 'zipf'
require_relative 'grammar'


module Parse


def Parse::visit i, l, r, x=0
  i.upto(r-x) { |span|
    l.upto(r-span) { |k|
      yield k, k+span
    }
  }
end

class Chart

  def initialize n
    @n = n
    @m = []
    (n+1).times {
      a = []
      (n+1).times { a << [] }
      @m << a
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

  def to_json weights=nil
    id = 0
    json_s = "{\n"
    json_s += "\"weights\":#{weights.to_json},\n"
    json_s += "\"nodes\":\n"
    json_s += "[\n"
    seen = {}
    Parse::visit(1, 0, @n) { |i,j|
      self.at(i,j).each { |item|
       _ = "#{item.lhs.symbol},#{i},#{j}"
       if !seen[_]
         json_s += "{ \"id\":#{id}, \"cat\":\"#{item.lhs.symbol.to_json.slice(1..-1).chomp('"')}\", \"span\":[#{i},#{j}] },\n"
         seen[_] = id
         id += 1
       end
      }
    }
    json_s += "{ \"id\":-1, \"cat\":\"root\", \"span\":[-1, -1] }\n"
    json_s += "],\n"
    json_s += "\"edges\":\n"
    json_s += "[\n"
    a = []
    Parse::visit(1, 0, @n) { |i,j|
      self.at(i,j).each { |item|
        _ = '{ '
        _ += "\"head\":#{seen[item.lhs.symbol+','+i.to_s+','+j.to_s]}, "
        _ += "\"rule\":\"[#{item.lhs.symbol.to_json.slice(1..-1).chomp('"')}] ||| "
        _ += "#{item.rhs.map{|x| (x.class==Grammar::NT ? '['+x.symbol.to_json.slice(1..-1).chomp('"')+','+x.index.to_s+']' : x.word.to_json).slice(1..-1).chomp('"') }.join(' ')} ||| #{item.target.map{|x| (x.class==Grammar::NT ? '['+x.symbol.to_json.slice(1..-1).chomp('"')+','+(x.index+1).to_s+']' : x.word.to_json.slice(1..-1).chomp('"')) }.join(' ')}\ |||\", "
        if item.tail_spans.empty?
          _ += "\"tails\":[-1], "
        else
          _ += "\"tails\":[#{item.rhs.zip((0..item.rhs.size-1).map{|q| item.tail_spans[q] }).select{|x| x[0].class==Grammar::NT }.map{|x| seen[x[0].symbol.to_json.slice(1..-1).chomp('"')+','+x[1].left.to_s+','+x[1].right.to_s]}.join ', '}], "
        end
        _ += "\"f\":#{(item.f ? item.f.to_json : '{}')} }"
        a << _
      }
    }
    json_s += a.join ",\n"
    json_s += "\n]\n"
    json_s += "}\n"

    return json_s
  end
end

Span = Struct.new(:left, :right)

class Item < Grammar::Rule
  attr_accessor :left, :right, :tail_spans, :dot, :f

  def initialize rule_or_item, left, right, dot
    @lhs        = Grammar::NT.new rule_or_item.lhs.symbol, rule_or_item.lhs.index
    @left       = left
    @right      = right
    @rhs        = []
    @tail_spans = {} # refers to source side, use @map
    @f          = rule_or_item.f
    @map        = (rule_or_item.map ? rule_or_item.map.dup : [])
    rule_or_item.rhs.each_with_index { |x,i| # duplicate rhs partially
      @rhs << x
      if x.class == Grammar::NT
        begin
          if i >= dot
            @tail_spans[i] = Span.new(-1, -1)
          else
            @tail_spans[i] = rule_or_item.tail_spans[i].dup
          end
        rescue
          @tail_spans[i] = Span.new(-1, -1)
        end
      end
    }
    @dot    = dot
    @target = rule_or_item.target
  end

  def to_s
    "(#{@left}, #{@right}) [#{tail_spans.map{|k,v| k.to_s+'('+v.left.to_s+','+v.right.to_s+')'}.join ' '}] {#{@map.to_s.delete('[]')}} #{lhs} -> #{rhs.map{|i|i.to_s}.insert(@dot,'*').join ' '} [dot@#{@dot}] ||| #{@target.map{|x|x.to_s}.join ' '}"
  end
end

def Parse::init input, n, active_chart, passive_chart, grammar
  grammar.flat.each { |r|
    input.each_index { |i|
      if input[i, r.rhs.size] == r.rhs.map { |x| x.word }
        passive_chart.add Item.new(r, i, i+r.rhs.size, r.rhs.size), i, i+r.rhs.size
      end
    }
  }
end

def Parse::scan item, input, limit, passive_chart
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

def Parse::parse input, n, active_chart, passive_chart, grammar
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
      remaining_items.each { |item|
        next if item.dot!=0
        next if item.rhs[item.dot].class!=Grammar::NT
        if item.rhs[item.dot].symbol == s
          new_item = Item.new item, i, j, item.dot+1
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


end #module

