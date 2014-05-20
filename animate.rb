s = 'ich sah ein kleines haus .'
def visit n, depth, skip=0 # FIXME
  (depth-skip).times { |i|
    i += skip
    0.upto(n-(i+1)) { |j|
      yield j, j+i+1
    }
  }
end

0,5


1    2
0,1  1,2  2,3  3,4      4,5
ich  sah  ein  kleines  haus

