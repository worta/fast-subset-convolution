#include <vector>
#include <cmath>
#include <algorithm>
#include "functions.h"
#include "utility.h"
#include <cstdint>
#include "common.h"
#include <array>

using namespace std;

template<typename T>
vector<T> fastMobius(Function<T> &f, int n) {
    int subset_count=1<<n;
    vector<vector<int> > d(n + 1, vector<int>(subset_count));
    for (int subset = 0; subset < subset_count; subset++) {
        d[0][subset] = f(subset);
    }
    for (set_t j = 1; j < n + 1; j++) {
        for (set_t k = 0; k < subset_count; k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                //cout <<k << " " <<j <<endl;
                d[j][k] = d[j - 1][k] + d[j - 1][k ^ (1 << (j - 1))];
            } else {
                d[j][k] = d[j - 1][k];
            }
        }
    }
    return d[n];
}


template<typename T>
vector<T> fastMobiusInversion(Function<T> &f_mobius, int n) {
    int subset_count=1<<n;
    vector<vector<T> > d(n + 1, vector<T>(subset_count));
    for (int k = 0; k < subset_count; k++) {
        d[0][k] = f_mobius(k);
    }
    for (set_t j = 1; j < n + 1; j++) {
        int j_1=j-1;
        set_t ele_set=1<<(j_1);
        for (set_t k = 0; k < subset_count; k++) {
            if (k & (ele_set)) { //if j is in K
                d[j][k] = d[j_1][k] - d[j_1][k ^ (ele_set)];
            } else { //j is not in K
                d[j][k] = d[j_1][k];
            }
        }
    }
    return d[n];
}



template<typename T>
void fastMobiusInversion(Function<T> &f_mobius, int n,vector<T> &result) {
    int subset_count=1<<n;
    vector<vector<T> > d(n, vector<T>(subset_count));
    for (int k = 0; k < subset_count; k++) {
        d[0][k] = f_mobius(k);
    }
    for (set_t j = 1; j < n; j++) {
        for (set_t k = 0; k < subset_count; k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                d[j][k] = d[j - 1][k] - d[j - 1][k ^ (1 << (j - 1))];
            } else { //j is not in K
                d[j][k] = d[j - 1][k];
            }
        }
    }
    for (set_t k = 0; k < subset_count; k++) {
        if (k & (1 << (n - 1))) { //if j is in K
            result[k] = d[n - 1][k] - d[n - 1][k ^ (1 << (n - 1))];
        } else { //j is not in K
            result[k] = d[n - 1][k];
        }
    }
}


template<typename T>
vector<T> rankedMobius(Function<T> &f, int n, int subsetRank) //only difference to fastMobius is the initalization
{
    int subset_count=1<<n;
    //T nullValue=0;
    vector<vector<T> > d(n + 1, vector<T>(subset_count));//vector wird mit 0 / standard constructor gebaut
    vector<set_t> subsets=generate_subsets_of_size_k(0,subsetRank,n);
    for(set_t subset:subsets){
        d[0][subset]=f(subset);
    }
#if 0 /*  for (int k = 0; k < subset_count; k++) {
        if (__builtin_popcount(k) ==
            subsetRank)  //__builtin_popcount counts bits set to 1, on hardware level if possible
        {
            d[0][k] = f(k);
        } else {
            d[0][k] = 0; //Klassen müssen eine Umwandlung von int/0 zum Ringelement erlauben
        }
    }*/
#endif
    for (int j = 1; j < n + 1; j++) {
        int index=j-1;
        int index_set=1<<index;
        for (int k = 0; k < subset_count; k++) { //TODO theoiretisch kann man k erst bei einem index anfangen lassen
            //der nur die subsets enthält, die über subset rank liegen
            if (k & (1 << (index)))   //is j in subset k?
            {
                d[j][k] = d[index][k] + d[index][k ^ (index_set)];
            } else {
                d[j][k] = d[index][k];
            }
        }
    }
    return d[n];
}


template<typename T>
void rankedMobius(Function<T> &f, int n, int subsetRank, vector<vector<T> > &result)
{
    int subset_count=1<<n;
    T d[n+1][subset_count];
    for(set_t i=0;i<subset_count;++i){
        d[0][i]=0;
    }
    vector<set_t> subsets=generate_subsets_of_size_k(0,subsetRank,n);
    for(set_t subset:subsets){
        d[0][subset]=f(subset);
    }
    for (int j = 1; j < n ; j++) {
        int index=j-1;
        int index_set=1<<index;
        for (int k = 0; k < subset_count; k++) { //TODO theoiretisch kann man k erst bei einem index anfangen lassen
            //der nur die subsets enthält, die über subset rank liegen
            if (k & (1 << (index)))   //is j in subset k?
            {
                d[j][k] = d[index][k] + d[index][k ^ (index_set)];
            } else {
                d[j][k] = d[index][k];
            }
        }
    }
    int n_1=n-1;
    for (int k = 0; k < subset_count; k++) {
        //der nur die subsets enthält, die über subset rank liegen
        if (k & (1 << (n_1)))   //is j in subset k?
        {
            result[subsetRank][k] = d[n_1][k] + d[n_1][k ^ (1<<n_1)];
        } else {
            result[subsetRank][k] = d[n_1][k];
        }
    }
}

template<typename T>
vector<T> naive_convolute(Function<T> &f, Function<T> &g, int n) {
    vector<T> conv(1<<n);
    //set_t N = ((set_t) 1 << n) - 1;
    for (int subset = 0; subset < (1<<n); ++subset) {
        conv[subset] = 0;
        vector<set_t> subsets= get_subsets(subset);
        for(set_t i:subsets){
            conv[subset] = conv[subset] + f(i) * g(subset xor i);
        }

    }
    return conv;
}

template<typename T>
void naive_convolute2(Function<T> &f, Function<T> &g, int n, int8_t* result) {
    //set_t N = ((set_t) 1 << n) - 1;
    for (int subset = 0; subset < (1<<n); ++subset) {
        result[subset] = 0;
        vector<set_t> subsets= get_subsets(subset);
        for(set_t i:subsets){
            result[subset] = result[subset] + f(i) * g(subset xor i);
        }
    }
}



template<typename T>
vector<vector<T> > ranked_convolute(vector<vector<T> > &f, vector<vector<T> >  &g, int n) {
    //Create result vector
    int subset_count = 1<<n;
    vector<vector<T> > conv(n + 1, vector<T>(subset_count));
    for (int k = 0; k <= n; k++) {
        for (int subset = 0; subset < subset_count; subset++) {
          //  conv[k][subset] = 0; unnötig da vektor
            for (int j = 0; j <= k; j++) {
                conv[k][subset] = conv[k][subset] + f[j][subset] * g[k - j][subset];
            }
        }
    }
    return conv;
}

template<typename T>
vector<T> ranked_Mobius_inversion(vector<vector<T> > ranked_f,const int n) {
    int subset_count=1<<n;
    //T val(0);
    vector<T> original_f(subset_count);
    //original_f.reserve(subset_count);
    for(int k=0;k<n+1;++k){
        VectFunction<T> f(ranked_f[k]);
        vector<T> intermediary=fastMobiusInversion<T>(f,n);
        vector<set_t> s= generate_subsets_of_size_k(0,k,n);
        for(set_t subset:s){
            original_f[subset]=intermediary[subset];
        }
    }
    return original_f;

}

template<typename T>
vector<T> advanced_convolute(Function<T> &f, int n) {

//    T ranked_transform[n][1<<n];
    vector<vector<T> > fast_ranked_transform_f(n+1,vector<T>(1<<n));
    for (int i = 0; i <= n; i++) {
       // fast_ranked_transform_f[i] = rankedMobius(f, n, i);
        rankedMobius(f,n,i,fast_ranked_transform_f);
    }
    //RankedVectFunction<T> f_r_func = RankedVectFunction<T>(fast_ranked_transform_f);
    vector<vector<T> > convoluted = ranked_convolute<T>(fast_ranked_transform_f, fast_ranked_transform_f, n);
    //RankedVectFunction<T> conv_f = RankedVectFunction<T>(convoluted);
    vector<T> result_advanced = ranked_Mobius_inversion<T>(convoluted, n);
    return result_advanced;
}

template<typename T>
vector<T>
advanced_convolute(Function<T> &f, Function<T> &g, int n) {
    vector<vector<T> > fast_ranked_transform_f(n + 1);
    vector<vector<T> > fast_ranked_transform_g(n + 1);
    for (int i = 0; i <= n; i++) {
        fast_ranked_transform_f[i] = rankedMobius(f, n, i);
        fast_ranked_transform_g[i] = rankedMobius(g, n, i);
    }
    RankedVectFunction<T> f_r_func = RankedVectFunction<T>(fast_ranked_transform_f);
    RankedVectFunction<T> g_r_func = RankedVectFunction<T>(fast_ranked_transform_g);
    vector<vector<T> > convoluted = ranked_convolute<T>(f_r_func, g_r_func, n);
    //RankedVectFunction<T> conv_f = RankedVectFunction<T>(convoluted);
    vector<T> result_advanced = ranked_Mobius_inversion<T>(convoluted, n);
    return result_advanced;
}

template<typename T>
vector<T> advanced_covering_product(Function<T> &f, Function<T> &g, int n) {
    vector<T> mobius_f = fastMobius<T>(f, n);
    vector<T> mobius_g = fastMobius<T>(g, n);

    //element wise product
    vector<T> hadamard;
    hadamard.reserve(mobius_f.size());
    for (int i = 0; i < mobius_f.size(); ++i) {
        hadamard[i] = mobius_f[i] * mobius_g[i];
    }
    VectFunction<T> hadF = VectFunction<T>(hadamard);
    vector<T> result = fastMobiusInversion(hadF, n);
    return result;
}
