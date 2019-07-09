//
// Created by dominik on 7/9/19.
//

#include "FastSubsetConvolution.h"
#include "utility.h";

template<class T>
FastSubsetConvolution<T>::FastSubsetConvolution(const int _n) {
    n=_n;
    set_count=((set_t)1)<<n;
}


template<class T>
void FastSubsetConvolution<T>::advanced_convolute(const Function<T> &f, T *result) {
    T ranked_transform[(n+1)*set_count];
    for(int i=0;i<=n;++i){
        rankedMobius(f,n,i,ranked_transform);
    }
    T ranked_convolution(n*set_count);
    ranked_convolute(ranked_transform,ranked_transform,ranked_convolution);
    ranked_mobius_inversion(ranked_convolution,result); //TODO vll hier in vektor üpberführen und den zurückgeben
}

template<class T>
void FastSubsetConvolution<T>::advanced_convolute(const Function<T> &f, const T &g, T *result) {

}

template<class T>
void FastSubsetConvolution<T>::fast_mobius(const T *f, T *result) {

}

template<class T>
void FastSubsetConvolution<T>::fast_mobius_inversion(const T *f_mobius, T *result) {

}

template<class T>
void FastSubsetConvolution<T>::ranked_mobius(const Function<T> &f, const int rank, T *result) {

}

template<class T>
void FastSubsetConvolution<T>::ranked_convolute(const T *f, const T *g, T *result) {

}

template<class T>
void FastSubsetConvolution<T>::ranked_mobius_inversion(const T *f, T *result) {

}
