//
// Created by dominik on 7/9/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
#define FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
#include "functions.h"

template<class T>
class FastSubsetConvolution {
    int n;

    int *rows;
   // T *buffer;
    void fast_mobius( T f[],T result[]);
    void fast_mobius_inversion( T f_mobius[], T result[]);

    void ranked_mobius(Function<T> &f,int rank,T result[]);
    void ranked_convolute(T f[], T g[],T result[]);
    void ranked_mobius_inversion( T f[],T result[]);

public:
    int set_count;
    FastSubsetConvolution(int _n);
    void advanced_convolute(Function<T> &f,T result[]);
    void advanced_convolute(Function<T> &f,T &g, T result[]);

};


#endif //FAST_SUBSET_CONVOLUTION_FASTSUBSETCONVOLUTION_H
