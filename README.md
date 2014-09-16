TODO
  * sparse vector (unordered_map) -> where to store?
  * parser
  * Rule -> ChartItem -> Node ?
  * k-best
  * other semirings
  * include language model
  * compress/hash words/feature strings?
  * cast? Rule -> Edge, ChartItem -> Node
  * feature factory, observer

Dependencies:
 * MessagePack for object serialization [1]
 * kenlm language model [2]

This is Linux only.


[1] http://msgpack.org
[2] http://kheafield.com/code/kenlm/


stuff to have a look at:
http://math.nist.gov/spblas/
http://lapackpp.sourceforge.net/
http://www.cvmlib.com/
http://sourceforge.net/projects/lpp/
http://math-atlas.sourceforge.net/
http://www.netlib.org/lapack/
http://bytes.com/topic/c/answers/702569-blas-vs-cblas-c
http://www.netlib.org/lapack/#_standard_c_language_apis_for_lapack
http://www.osl.iu.edu/research/mtl/download.php3
http://scicomp.stackexchange.com/questions/351/recommendations-for-a-usable-fast-c-matrix-library
https://software.intel.com/en-us/tbb_4.2_doc
http://goog-perftools.sourceforge.net/doc/tcmalloc.html
http://www.sgi.com/tech/stl/Rope.html
http://www.cs.unc.edu/Research/compgeom/gzstream/
https://github.com/facebook/folly/blob/6e46d468cf2876dd59c7a4dddcb4e37abf070b7a/folly/docs/Overview.md
---
not much to see here, yet
(SCFG machine translation decoder in ruby, currently implements CKY+ parsing and hypergraph viterbi)

helpful stuff
 * https://github.com/jweese/thrax/wiki/Glue-grammar
 * http://aclweb.org/aclwiki/index.php?title=Hypergraph_Format
 * http://kheafield.com/code/kenlm/developers/

todo
====
 * integrate with HG (chart to json)
 * kbest
 * feature interface
 * (global) word ids instead of strings
 * animate parsing

