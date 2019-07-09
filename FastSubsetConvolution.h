//
// Created by dominik on 7/9/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
#define FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
#include "functions.h"
template<class T>
class FastSubsetConvolution {
    const int n;
    const int set_count;
    void fast_mobius(const T f[],T result[]);
    void fast_mobius_inversion(const T f_mobius[], T result[]);

    void ranked_mobius(const Function<T> &f,const int rank,T result[]);
    void ranked_convolute(const T f[],const T g[],T result[]);
    void ranked_mobius_inversion(const T f[],T result[]);


public:
    FastSubsetConvolution(const int _n);
    void advanced_convolute(const Function<T> &f,T result[]);
    void advanced_convolute(const Function<T> &f,const T &g, T result[]);

};


#endif //FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
