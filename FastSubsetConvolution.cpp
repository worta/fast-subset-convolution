//
// Created by dominik on 7/9/19.
//

#include <iostream>
#include "FastSubsetConvolution.h"
#include "utility.h"
#include "MinSumRingEmbedd.h"
#include "MinSumRingEmbeddBigInt.h"
template class FastSubsetConvolution<int>; //need this for every type usen in the program
template class FastSubsetConvolution<MinSumRingEmbedd>;
template class FastSubsetConvolution<MinSumRingEmbeddBigInt>;
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
void FastSubsetConvolution<T>::advanced_covering_product(Function<T> &f, T *result) {
    int hadamard[set_count];
    fast_mobius(f,hadamard);
    //element wise product= hadamard product
    for(int i=0;i<set_count;++i){
        hadamard[i]=hadamard[i]*hadamard[i]; //hadamard on the rhs is the mobius transform of f, just so thatr
        //not two buffers are needed
    }
    fast_mobius_inversion(hadamard,result);
}


template<class T>
void FastSubsetConvolution<T>::advanced_convolute(Function<T> &f, T *result) {
    T* ranked_transform=new T[(n + 1) * set_count];
    for (int i = 0; i <= n; ++i) {
        ranked_mobius(f, i, &ranked_transform[rows[i]]);
    } //bis hier sieht es korrekt aus
    T* ranked_convolution= new T[(n+1) * set_count];
    ranked_convolute(ranked_transform, ranked_transform, ranked_convolution);
    ranked_mobius_inversion(ranked_convolution, result); //TODO vll hier in vektor üpberführen und den zurückgeben
    delete[] ranked_convolution;
    delete[] ranked_transform;
}

template<class T>
void FastSubsetConvolution<T>::advanced_convolute(Function<T> &f, T &g, T *result) {

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
        buffer[0 + s] = f(s);
    }
    //finish initialization
    for (int j = 1; j < n; j++) {
        int index = j - 1;
        int index_set = 1 << index;
        for (int k = 0; k < set_count; k++) { //TODO theoiretisch kann man k erst bei einem index anfangen lassen
            //der nur die subsets enthält, die über subset rank liegen
            if (k & (1 << (index)))   //is j in subset k?
            {
                buffer[j * set_count + k] = buffer[index * set_count + k] + buffer[index * set_count + k ^ (index_set)];
            } else {
                buffer[j * set_count + k] = buffer[index * set_count + k];
            }
        }
    }

    int n_index = n - 1;
    int n_set = 1 << n_index;
    int row = n_index * set_count;
    for (int k = 0; k < set_count; k++) { //TODO theoiretisch kann man k erst bei einem index anfangen lassen
        //der nur die subsets enthält, die über subset rank liegen
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
        for (set_t k = 0; k < set_count; k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                //cout <<k << " " <<j <<endl;
                buffer[j*set_count+k] = buffer[(j - 1)*set_count+k] + buffer[(j - 1)*set_count+k ^ (1 << (j - 1))];
            } else {
                buffer[j*set_count+k] = buffer[(j - 1)*set_count+k];
            }
        }
    }
    //same as above, just with with j=n
    for (set_t k = 0; k < set_count; k++) {
        if (k & (1 << (n - 1))) { //if j is in K
            //cout <<k << " " <<j <<endl;
            result[k] = buffer[(n - 1)*set_count+k] + buffer[(n - 1)*set_count+k ^ (1 << (n - 1))];
        } else {
            result[k] = buffer[(n - 1)*set_count+k];
        }
    }
}

//uses buffer
template<class T>
void FastSubsetConvolution<T>::fast_mobius_inversion(T *f_mobius, T *result) {
    //T* buffer=new T[n*set_count];
   // T* buffer=new T[n*set_count];
    for (int set = 0; set < set_count; ++set) {
        buffer[0 + set] = f_mobius[set];
    }

    for (set_t j = 1; j < n; j++) {
        int row = j * set_count;
        int index = j - 1;
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
    int previous_row = (n - 1) * set_count;
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
    int size=(n+1)*set_count;
    //T* buffer2=new T[size];
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


 //   delete[] buffer2;
}


template<class T>
FastSubsetConvolution<T>::~FastSubsetConvolution() {
    delete[] rows;
    delete [] buffer;
}

