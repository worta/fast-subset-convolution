#ifndef MOBIUS_H_INCLUDED
#define MOBIUS_H_INCLUDED
#include "functions.h"


template <typename T>
vector<T> fastMobius(Function<T> &f,int n);
template <typename T>
vector<T> rankedMobius(Function<T> &f,int n,int subsetRank);
template <typename T>
vector<T> naive_convolute(Function<T> &f, Function<T> &g, int n);
template <typename T>
void naive_convolute2(Function<T> &f, Function<T> &g, int n, int8_t * result);
template <typename T>
vector<vector<T> > ranked_convolute(RankedFunction<T> &f, RankedFunction <T> &g, int n);
template <typename T>
vector<T> ranked_Mobius_inversion(RankedFunction<T> &f,int n);
template<typename T>
vector<T> advanced_convolute(Function<T> &f,Function<T> &g,int n);


#include "mobius.cpp"

#endif // MOBIUS_H_INCLUDED
