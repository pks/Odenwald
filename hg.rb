#!/usr/bin/env ruby

require 'nlp_ruby'
require 'json'
require_relative 'grammar'


module HG


class HG::Node
  attr_accessor :id, :cat, :outgoing, :incoming, :score

  def initialize id=nil, cat=nil, outgoing=[], incoming=[], score=nil
    @id       = id 
    @cat      = cat
    @outgoing = outgoing
    @incoming = incoming
    @score    = nil
  end

  def to_s
    "Node<id:#{@id}, cat:\"#{@cat}\", outgoing:#{@outgoing.size}, incoming:#{@incoming.size}>"
  end
end

class HG::Hypergraph
  attr_accessor :nodes, :edges

  def initialize nodes=[], edges=[]
    @nodes = nodes
    @edges = edges
  end

  def arity
    @edges.map { |e| e.arity }.max
  end

  def reset
    @edges.each { |e| e.mark = 0 }
  end

  def to_s
    "Hypergraph<nodes:[#{@nodes.to_s}], edges:[#{@edges.to_s}], arity:#{arity}>"
  end
end

class HG::Hyperedge
  attr_accessor :head, :tails, :weight, :f, :mark, :rule

  def initialize head=nil, tails=[], weight=0.0, f=SparseVector.new, rule=nil
    @head   = head
    @tails  = tails
    @weight = weight
    @f      = f
    @mark   = 0
    @rule   = Grammar::Rule.from_s rule if rule
  end

  def arity
    return @tails.size
  end

  def marked?
    arity == @mark
  end

  def to_s
    "Hyperedge<head:\"#{@head.id}\", rule:\"#{@rule.to_s}, \"tails:#{@tails.map{|n|n.id}}, arity:#{arity}, weight:#{@weight}, f:#{f.to_s}, mark:#{@mark}>"
  end
end

def HG::topological_sort nodes
  sorted = []
  s = nodes.reject { |n| !n.incoming.empty? }
  while !s.empty?
    sorted << s.shift
    sorted.last.outgoing.each { |e|
      next if e.marked?
      e.mark += 1
      s << e.head if e.head.incoming.reject{ |f| f.mark==f.arity }.empty?
    }
  end
  return sorted
end

def HG::init nodes, semiring, root
  nodes.each { |n| n.score=semiring.null }
  root.score = semiring.one
end

def HG::viterbi hypergraph, root, semiring=ViterbiSemiring.new
  toposorted = topological_sort hypergraph.nodes
  init toposorted, semiring, root
  toposorted.each { |n|
    n.incoming.each { |e|
      s = semiring.one
      e.tails.each { |m|
        s = semiring.multiply.call(s, m.score)
      }
      n.score = semiring.add.call(n.score, semiring.multiply.call(s, e.weight))
    }
  }
end

def HG::viterbi_path hypergraph, root, semiring=ViterbiSemiring.new
  toposorted = topological_sort hypergraph.nodes
  init toposorted, semiring, root
  best_path = []
  toposorted.each { |n|
    best_edge = nil
    n.incoming.each { |e|
      s = semiring.one
      e.tails.each { |m|
        s = semiring.multiply.call(s, m.score)
      }
      if n.score < semiring.multiply.call(s, e.weight) # ViterbiSemiring add
        best_edge = e
      end
      n.score = semiring.add.call(n.score, semiring.multiply.call(s, e.weight))
    }
    best_path << best_edge if best_edge
  }
  return best_path, toposorted.last.score
end

def HG::viterbi_string hypergraph, root, semiring=ViterbiSemiring.new
  toposorted = topological_sort hypergraph.nodes
  init toposorted, semiring, root
  s = ''
  toposorted.each { |n|
    best_s = nil
    n.incoming.each { |e|
      s = semiring.one
      e.tails.each { |m|
        s = semiring.multiply.call(s, m.score)
      }
      if n.score < semiring.multiply.call(s, e.weight) # ViterbiSemiring add
        best_s = e.e
      end
      n.score = semiring.add.call(n.score, semiring.multiply.call(s, e.weight))
    }
    s += best_s if best_s
  }
  return s, toposorted.last.score
end

def HG::all_paths hypergraph, root
  toposorted = topological_sort hypergraph.nodes
  paths = [[]]
  toposorted.each { |n|
    next if n.incoming.empty?
    new_paths = []
    while !paths.empty?
      p = paths.pop
      n.incoming.each { |e|
        new_paths << p+[e]
      }
    end
    paths = new_paths
  }
  return paths
end

def HG::read_hypergraph_from_json fn, semiring=RealSemiring.new, log_weights=false
  nodes = []
  edges = []
  nodes_by_id = {}
  h = JSON.parse File.new(fn).read
  w = SparseVector.from_h h['weights']
  h['nodes'].each { |x|
    n = Node.new x['id'], x['cat']
    nodes << n
    nodes_by_id[n.id] = n
  }
  h['edges'].each { |x|
    e = Hyperedge.new(nodes_by_id[x['head']], \
                      x['tails'].map { |j| nodes_by_id[j] }.to_a, \
                      (x['weight'] ? semiring.convert.call(x['weight'].to_f) : nil), \
                      (x['f'] ? SparseVector.from_h(x['f']) : nil), \
                      x['rule'])
    if x['f']
      if log_weights
        e.weight = Math.exp(w.dot(e.f))
      else
        e.weight = w.dot(e.f)
      end
    end
    e.tails.each { |m|
      m.outgoing << e
    }
    e.head.incoming << e
    edges << e
  }
  return Hypergraph.new(nodes, edges), nodes_by_id
end

def HG::derive path, cur, carry
  edge = path.select { |e|    e.rule.lhs.symbol==cur.symbol \
                           && e.rule.lhs.left==cur.left \
                           && e.rule.lhs.right==cur.right }.first
  edge.rule.target.each { |i|
    if i.class == Grammar::NT
      derive path, i, carry
    else
      carry << i
    end
  }
  return carry
end


end #module

