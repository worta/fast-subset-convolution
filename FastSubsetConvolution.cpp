//
// Created by dominik on 7/9/19.
//

#include <iostream>
#include "FastSubsetConvolution.h"
#include "utility.h"
#include "MinSumRingEmbedd.h"
#include "MinSumRingEmbeddBigInt.h"
#include "SmallMinSumRing64Bit.h"
template class FastSubsetConvolution<int>; //need this for every type usen in the program
template class FastSubsetConvolution<int8_t >;
//template class FastSubsetConvolution<MinSumRingEmbedd>;
template class FastSubsetConvolution<MinSumRingEmbeddBigInt>;
//template class FastSubsetConvolution<SmallMinSumRing64Bit>;

template<class T>
FastSubsetConvolution<T>::FastSubsetConvolution(int _n) {
    n = _n;
    set_count = ((set_t) 1) << (n);
    buffer = new T[set_count * (n+1)];
    rows = new int[n + 1];
    for (int i = 0; i < n + 1; ++i) {
        rows[i] = set_count * i;
    }
}

template<class T>
FastSubsetConvolution<T>::FastSubsetConvolution(int _n,bool only_covering_product) {
    n = _n;
    set_count = ((set_t) 1) << (n);
    rows = new int[n + 1];
    if(only_covering_product){
        buffer = new T[2*set_count ];
    }
    else{
        buffer = new T[set_count * (n+1)];
    }
    for (int i = 0; i < n + 1; ++i) {
        rows[i] = set_count * i;
    }
}

template<class T>
void FastSubsetConvolution<T>::advanced_covering_product(Function<T> &f, T *result) {
    T* hadamard=new T[set_count];
    fast_mobius(f,hadamard);
    //element wise product= hadamard product
    for(int i=0;i<set_count;++i){
        hadamard[i]=hadamard[i]*hadamard[i]; //hadamard on the rhs is the mobius transform of f, just so thatr
        //not two buffers are needed
    }
    fast_mobius_inversion(hadamard,result);
    delete[] hadamard;
}


template<class T>
void FastSubsetConvolution<T>::advanced_covering_product(Function<T> &f, Function<T> &g,T *result) {
    T* hadamard=new T[set_count];
    fast_mobius(f,hadamard);
    T* mult=new T[set_count];
    fast_mobius(g,mult);
    //element wise product= hadamard product
    for(int i=0;i<set_count;++i){
        hadamard[i]=hadamard[i]*mult[i]; //the previous value in hadamard is the mobius transform of f, just so that
        //not two buffers are needed
    }
    fast_mobius_inversion(hadamard,result);
    delete[] mult;
    delete[] hadamard;
}


template<class T>
void FastSubsetConvolution<T>::advanced_convolute(Function<T> &f, T *result) {
    T* ranked_transform=new T[(n + 1) * set_count];
    for (int i = 0; i <= n; ++i) {
        ranked_mobius(f, i, &ranked_transform[rows[i]]);
    }
    T* ranked_convolution= new T[(n+1) * set_count];
    ranked_convolute(ranked_transform, ranked_transform, ranked_convolution);
    ranked_mobius_inversion(ranked_convolution, result); //TODO vll hier in vektor überführen und den zurückgeben
    delete[] ranked_convolution;
    delete[] ranked_transform;
}

template<class T>
void FastSubsetConvolution<T>::advanced_convolute(Function<T> &f, Function<T> &g, T *result) {
    T* ranked_transform1=new T[(n + 1) * set_count];
    T* ranked_transform2=new T[(n + 1) * set_count];
    for (int i = 0; i <= n; ++i) {
        ranked_mobius(f, i, &ranked_transform1[rows[i]]);
        ranked_mobius(g, i, &ranked_transform2[rows[i]]);
    }
    T* ranked_convolution= new T[(n+1) * set_count];
    ranked_convolute(ranked_transform1, ranked_transform2, ranked_convolution);
    ranked_mobius_inversion(ranked_convolution, result); //TODO vll hier in vektor überführen und den zurückgeben
    delete[] ranked_convolution;
    delete[] ranked_transform1;
    delete[] ranked_transform2;
}

//uses buffer
template<class T>
void FastSubsetConvolution<T>::ranked_mobius(Function<T> &f,int rank, T *result) {
    //Initinialize buffer
 //   T* buffer=new T[set_count*(n)];
    for (int subset = 0; subset < set_count; ++subset) {
        buffer[0 + subset] = 0; //T muss=0 implementieren
    }
    vector<set_t> subsets = generate_subsets_of_size_k(0, rank, n); //I suspect this is faster than 2^n popcounts
    for (set_t s:subsets) {
        buffer[s] = f(s); //buffer[0][s]
    }
    //finish initialization
    for (int j = 1; j < n; j++) {
        int index = j - 1;
        int index_set = 1 << index;
        int buffer_to_write_level=j*set_count; //equivalent to rows[j], maybe change?
        int buffer_to_read_level=index*set_count;
        for (int k = 0; k < set_count; k++) {
            if (k & (1 << (index)))   //is j in subset k?
            {
                buffer[buffer_to_write_level + k] = buffer[buffer_to_read_level+ k] + buffer[buffer_to_read_level + k ^ (index_set)];
            } else {
                buffer[buffer_to_write_level + k] = buffer[buffer_to_read_level + k];
            }
        }
    }

    int n_index = n - 1;
    int n_set = 1 << n_index;
    int row = n_index * set_count;


    for (int k = 0; k < set_count; k++) {
        if (k & (1 << (n_index)))   //is n in subset k?
        {
            result[k] = buffer[row + k]+ buffer[row + k ^ (n_set)];
        } else {
            result[k] = buffer[row + k];
        }
    }
 //   delete[] buffer;
}


//uses buffer
template<class T>
void FastSubsetConvolution<T>::fast_mobius(Function<T> &f, T *result) {
    for(int subset=0;subset<set_count;++subset){
        buffer[subset]=f(subset); //initialize buffer[0][subsez] with function value
    }
    for (set_t j = 1; j < n; j++) {
        int previous_row=(j-1)%2;
        int current_row=j%2;
        set_t singleton_j=(1 << (j - 1));
        for (set_t k = 0; k < set_count; k++) {
            if (k & singleton_j) { //if j is in K
                buffer[rows[current_row]+k] = buffer[rows[previous_row]+k] + buffer[rows[previous_row]+k ^ singleton_j]; //k ^ singleton_j == set k without j
            } else {
                buffer[rows[current_row]+k] = buffer[rows[previous_row]+k];
            }
        }
    }
    //same as above, just with with j=n
    int previous_row=(n-1)%2;
    set_t singleton_n=(1 << (n - 1));
    for (set_t k = 0; k < set_count; k++) {
        if (k & singleton_n) { //if j is in K
            //cout <<k << " " <<j <<endl;
            result[k] = buffer[rows[previous_row]+k] + buffer[rows[previous_row]+k ^ singleton_n];
        } else {
            result[k] = buffer[rows[previous_row]+k];
        }
    }
}

//uses buffer
template<class T>
void FastSubsetConvolution<T>::fast_mobius_inversion(T *f_mobius, T *result) {
    //todo maybe change to 2 level buffer, because only previous level is of interest
    //T* buffer=new T[n*set_count];
   // T* buffer=new T[n*set_count];
    for (int set = 0; set < set_count; ++set) {
        buffer[0 + set] = f_mobius[set];
    }

    for (set_t j = 1; j < n; j++) {
        int row = (j%2) * set_count ;
        int index = (j - 1)%2;
        int previous_row = index * set_count;
        set_t index_set = 1 << index;
        for (set_t k = 0; k < set_count; k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                buffer[row + k] = buffer[previous_row + k] - buffer[previous_row + k ^ index_set];
            } else { //j is not in K
                buffer[row + k] = buffer[previous_row + k];
            }
        }
    }
    //basically the same as the inner loop with j=n, and the final result saved i nresult
    int previous_row = ((n - 1)%2) * set_count;
    set_t index_set = 1 << (n - 1);
    for (set_t k = 0; k < set_count; k++) {
        if (k & (1 << (n - 1))) { //if j is in K
            result[k] = buffer[previous_row + k] - buffer[previous_row + k ^ index_set];
        } else { //j is not in K
            result[k] = buffer[previous_row + k];
        }
    }
   // delete[] buffer; //if own buffer for each function is used
}


template<class T>
void FastSubsetConvolution<T>::ranked_convolute(T *f, T *g, T *result) {
    for (int k = 0; k <= n; k++) {
        for (set_t subset = 0; subset < set_count; subset++) {
            result[rows[k] + subset] = 0;
            for (int j = 0; j <= k; j++) {
                result[rows[k] + subset] =result[rows[k] + subset] + f[rows[j] + subset] * g[rows[k - j] + subset]; //alternative += but needs to be implkement for min sum ring
            }
        }
    }
}

template<class T>
void FastSubsetConvolution<T>::ranked_mobius_inversion(T *f, T *result) {
   // int size=(n+1)*set_count;

    for (int k = 0; k < n + 1; ++k) {
        fast_mobius_inversion(&f[rows[k]], &buffer[rows[k]]);
        int subset_count=nChoosek(n,k);
        set_t* s=new set_t[subset_count]; //vll einfach einmal max size allocaten oben, aber soltle nicht so teuer sein
        generate_sets_of_size_k(k,n,s);
        for (int i=0;i<subset_count;++i) {
            set_t subset=s[i];
            result[subset] = buffer[rows[k]+ subset];
        }
        delete[] s;
    }
;
}


template<class T>
FastSubsetConvolution<T>::~FastSubsetConvolution() {
    delete[] rows;
    delete [] buffer;
}

