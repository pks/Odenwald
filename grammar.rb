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
  attr_accessor :sym, :idx

  def initialize sym, idx=0
    @sym = sym
    @idx = idx
  end

  def to_s
    "NT<#{sym},#{idx}>"
  end
end

class Span
  attr_accessor :left, :right

  def initialize left=nil, right=nil
    @left = left
    @right = right
  end
end

class Rule
  attr_accessor :lhs, :rhs, :span

  def initialize lhs=nil, rhs=nil, span=nil
    @lhs = ''
    @rhs = []
  end

  def to_s
    "#{lhs} -> #{rhs.map{|i|i.to_s}.join ' '} a:#{arity} (#{@span.left}, #{@span.right})"
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
    r
  end
end

class Grammar
  attr_accessor :rules

  def initialize fn
    @rules = []
    a = ReadFile.readlines_strip fn
    a.each { |s| @rules << Rule.from_s(s) }
  end

  def to_s
    s = ''
    @rules.each { |r| s += r.to_s+"\n" }
    s
  end
end

