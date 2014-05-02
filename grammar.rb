require 'nlp_ruby'

class Terminal
  attr_accessor :w

  def initialize s
    @w = s
  end

  def to_s
    "T<#{@w}>"
  end
end

class NonTerminal
  attr_accessor :sym, :n

  def initialize s, n=1
    @sym = s
    @n = n
  end

  def to_s
    "NT<#{sym}>"
  end
end

class Span
  attr_accessor :left, :right

  def initialize left=-1, right=-1
    @left = left
    @right = right
  end
end

class Rule
  attr_accessor :lhs, :rhs, :span

  def initialize lhs=nil, rhs=nil, span=Span.new
    @lhs = ''
    @rhs = []
    @span = span
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.join ' '} (#{arity}) (#{@span.left}, #{@span.right})"
  end

  def arity
    rhs.reject { |i| i.class == Terminal }.size
  end

  def from_s s
    a = splitpipe s, 3
    @lhs = NonTerminal.new a[0].strip.gsub!(/(\[|\])/, "")
    a[1].split.each { |i|
      i.strip!
      if i[0]=='[' && i[i.size-1] == ']'
        @rhs << NonTerminal.new(i.gsub!(/(\[|\])/, "").split(',')[0])
      else
        @rhs << Terminal.new(i)
      end
    }
    @span = Span.new
  end

  def self.from_s s
    r = self.new
    r.from_s s
    return r
  end
end

class Grammar
  attr_accessor :rules

  def initialize fn
    @rules = []
    l = ReadFile.readlines_strip fn
    l.each { |i|
      @rules << Rule.from_s(i)
    }
  end

  def to_s
    s = ''
    @rules.each { |r| s += r.to_s+"\n" }
  end
end






