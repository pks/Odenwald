#pragma once
#include <msgpack.hpp>


struct DummyVector {
  double CountEF;
  double EgivenFCoherent;
  double Glue;
  double IsSingletonF;
  double IsSingletonFE;
  double LanguageModel;
  double LanguageModel_OOV;
  double MaxLexFgivenE;
  double MaxLexEgivenF;
  double PassThrough;
  double PassThrough_1;
  double PassThrough_2;
  double PassThrough_3;
  double PassThrough_4;
  double PassThrough_5;
  double PassThrough_6;
  double SampleCountF;
  double WordPenalty;

  MSGPACK_DEFINE(CountEF, EgivenFCoherent, Glue, IsSingletonF, IsSingletonFE, LanguageModel, LanguageModel_OOV, MaxLexEgivenF, MaxLexFgivenE, PassThrough, PassThrough_1, PassThrough_2, PassThrough_3, PassThrough_4, PassThrough_5, PassThrough_6, SampleCountF, WordPenalty);
};

