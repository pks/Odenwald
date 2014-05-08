require 'nlp_ruby'


class T
  attr_accessor :word

  def initialize word
    @word = word
  end

  def to_s
    "T<#{@word}>"
  end
end

class NT
  attr_accessor :symbol, :index

  def initialize symbol, index=0
    @symbol = symbol
    @index = index
  end

  def to_s
    "NT<#{@symbol},#{@index}>"
  end
end



class Rule
  attr_accessor :lhs, :rhs

  def initialize lhs=nil, rhs=[]
    @lhs = lhs
    @rhs = rhs
  end

  def to_s
    "#{lhs} -> #{rhs.map{ |i| i.to_s }.join ' '} [arity=#{arity}]"
  end

  def arity
    rhs.reject { |i| i.class==T }.size
  end

  def from_s s
    _ = splitpipe s, 3
    @lhs = NT.new _[0].strip.gsub!(/(\[|\])/, "")
    _[1].split.each { |i|
      i.strip!
      if i[0]=='[' && i[i.size-1] == ']'
        @rhs << NT.new(i.gsub!(/(\[|\])/, "").split(',')[0])
      else
        @rhs << T.new(i)
      end
    }
  end

  def self.from_s s
    r = self.new
    r.from_s s
    r
  end
end

class Grammar
  attr_accessor :rules

  def initialize fn
    @rules = []
    @glue_rules = []
    ReadFile.readlines_strip(fn).each_with_index { |s,j|
      STDERR.write '.'
      STDERR.write "\n" if (j+1)%80==0
      @rules << Rule.from_s(s)
    }
    STDERR.write "\n"
  end

  def to_s
    s = ''
    @rules.each { |r| s += r.to_s+"\n" }
    s
  end

  def add_glue_rules
    # see https://github.com/jweese/thrax/wiki/Glue-grammar
    @rules.map { |r| r.lhs.symbol }.reject { |s| s=='S' }.uniq.each { |s|
      @rules << Rule.new(NT.new('S'), [NT.new(s)])
      @glue_rules << @rules.last
      @rules << Rule.new(NT.new('S'), [NT.new('S'), NT.new('X')])
      @glue_rules << @rules.last
    }
  end

  def add_pass_through_rules input
    input.each { |terminal|
      @rules << Rule.new(NT.new('X'), [T.new(terminal.word)])
    }
  end
end

