Not quite finished machine translation decoder.
(For Linux only)

TODO
====
 * proper parsing (Rico Sennrich's [1][2]?)
 * k-best derivations [3]
 * serialization for sparse vectors
 * Rule-ChartItem-Node transition?
 * cube pruning [4] and integrate kenlm [5]
 * feature factory and observer patterns
 * map all strings to ints?
 * glue grammar [6] alright?
 * read/writed gzipped files [11]
 * integrate some BLAS lib for vector ops [12][13]

Dependencies:
 * MessagePack for object serialization [8]
 * Google's gperftools  [9]
 * json-cpp [10]


[1]  http://aclweb.org/anthology/W/W14/W14-4011.pdf
[2]  https://github.com/redpony/cdec/commit/448b451aa481b1509566ddb11abc3476466def6a
[3]  http://www.cis.upenn.edu/~lhuang3/huang-iwpt-correct.pdf
[4]  http://cui.unige.ch/~gesmundo/papers/gesmundo-iwslt10-fcp.pdf
[5]  http://kheafield.com/code/kenlm/developers/2
[6]  https://github.com/jweese/thrax/wiki/Glue-grammar
[7]  http://aclweb.org/aclwiki/index.php?title=Hypergraph_Format
[8]  http://msgpack.org
[9]  https://code.google.com/p/gperftools/
[10] https://github.com/ascheglov/json-cpp
[11] http://www.cs.unc.edu/Research/compgeom/gzstream/
[12] http://scicomp.stackexchange.com/questions/351/recommendations-for-a-usable-fast-c-matrix-library
[13] http://www.cvmlib.com/

