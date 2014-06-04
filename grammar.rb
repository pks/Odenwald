module Grammar

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
  attr_accessor :symbol, :index, :left, :right

  def initialize symbol=nil, index=nil, left=nil, right=nil
    @symbol = symbol
    @index = index
    @left = left
    @right = right
  end

  def from_s s
    s.delete! '[]'
    @symbol, meta = s.split '@'
    if meta
      span, index = meta.split ','
      @left, @right = span.split(':').map { |x| x.to_i }
      @index = index.to_i
    end
  end

  def self.from_s s
    n = NT.new
    n.from_s s
    return n
  end

  def to_s
    "NT(#{@left},#{@right})<#{@symbol},#{@index}>"
  end
end

class Rule
  attr_accessor :lhs, :rhs, :target, :map

  def initialize lhs=nil, rhs=[], target=[]
    @lhs = lhs
    @rhs = rhs
    @target = target
    @arity_ = nil
  end

  def to_s
    "#{@lhs} -> #{@rhs.map{ |i| i.to_s }.join ' '} ||| #{@target.map{ |i| i.to_s }.join ' '} [arity=#{arity}]" #FIXME
  end

  def arity
    return @arity_ if @arity_
    rhs.select { |i| i.class == NT }.size
  end

  def read_right_ s
    _ = []
    s.split.each { |x|
      x.strip!
      if x[0]=='[' && x[x.size-1] == ']'
        _ << NT.from_s(x)
      else
        _ << T.new(x)
      end
    }
    return _
  end

  def from_s s
    lhs, rhs, target = splitpipe s, 3
    @lhs = NT.from_s lhs
    @rhs = read_right_ rhs
    @target = read_right_ target
  end

  def self.from_s s
    r = self.new
    r.from_s s
    return r
  end
end

class Grammar
  attr_accessor :rules, :startn, :startt, :flat

  def initialize fn
    @rules = []; @startn = []; @startt = []; @flat = []
    ReadFile.readlines_strip(fn).each_with_index { |s,i|
      STDERR.write '.'; STDERR.write " #{i+1}\n" if (i+1)%80==0
      @rules << Rule.from_s(s)
      if @rules.last.rhs.first.class == NT
        @startn << @rules.last
      else
        if rules.last.arity == 0
          @flat << @rules.last
        else
          @startt << @rules.last
        end
      end
    }
    STDERR.write "\n"
  end

  def to_s
    s = ''
    @rules.each { |r| s += r.to_s+"\n" }
    return s
  end

  def add_glue_rules
    @rules.map { |r| r.lhs.symbol }.select { |s| s != 'S' }.uniq.each { |symbol|
      @rules << Rule.new(NT.new('S'), [NT.new(symbol)])
      @startn << @rules.last
      @rules << Rule.new(NT.new('S'), [NT.new('S'), NT.new('X')])
      @startn << @rules.last
    }
  end

  def add_pass_through_rules s
    s.each { |word|
      @rules << Rule.new(NT.new('X'), [T.new(word)])
      @flat << @rules.last
    }
  end
end


end # module

