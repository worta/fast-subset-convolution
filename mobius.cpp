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
    vector<vector<int> > d(n + 1, vector<int>(subset_count));
    for (int k = 0; k < subset_count; k++) {
        d[0][k] = f_mobius(k);
    }
    for (set_t j = 1; j < n + 1; j++) {
        for (set_t k = 0; k < subset_count; k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                d[j][k] = d[j - 1][k] - d[j - 1][k ^ (1 << (j - 1))];
            } else { //j is not in K
                d[j][k] = d[j - 1][k];
            }
        }
    }
    return d[n];
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
        for (int k = 0; k < (1<<n); k++) {
            if (k & (1 << (index)))   //is j in subset k?
            {
                d[j][k] = d[index][k] + d[index][k ^ (1 << (index))];
            } else {
                d[j][k] = d[index][k];
            }
        }
    }
    return d[n];
}


template<typename T>
vector<T> naive_convolute(Function<T> &f, Function<T> &g, int n) {
    vector<T> conv(1<<n);
    //set_t N = ((set_t) 1 << n) - 1;
    for (int subset = 0; subset < (1<<n); ++subset) {
        conv[subset] = 0;
        vector<set_t> subsets=get_subsets_it(subset);
        for(set_t i:subsets){
            conv[subset] = conv[subset] + f(i) * g(subset xor i);
        }

    }
    return conv;
}


template<typename T>
vector<vector<T> > ranked_convolute(RankedFunction<T> &f, RankedFunction<T> &g, int n,vector<set_t> &subsets) {
    //Create result vector
    vector<vector<T> > conv(n + 1, vector<T>((int) pow(2, n)));
    for (int k = 0; k <= n; k++) {
        for (set_t subset:subsets) {
            conv[k][subset] = 0;
            for (int j = 0; j <= k; j++) {
                conv[k][subset] = conv[k][subset] + f(j, subset) * g(k - j, subset);
            }
        }
    }
    return conv;
}


template<typename T>
vector<vector<T> > ranked_convolute(RankedFunction<T> &f, RankedFunction<T> &g, int n) {
    //Create result vector
    vector<vector<T> > conv(n + 1, vector<T>(1<<n));
    int subset_count = 1<<n;
    for (int k = 0; k <= n; k++) {
        for (int subset = 0; subset < subset_count; subset++) {
            conv[k][subset] = 0;
            for (int j = 0; j <= k; j++) {
                conv[k][subset] = conv[k][subset] + f(j, subset) * g(k - j, subset);
            }
        }
    }
    return conv;
}

template<typename T>
vector<T> ranked_Mobius_inversion(RankedFunction<T> &f, int n) {
    int subset_count=1<<n;
    //T val(0);
    vector<T> original_f(subset_count);
    for (set_t s = 0; s < subset_count; ++s) {
        vector<set_t> subsets=get_subsets_it(s);
        //original_f[s] = 0;
        int element_count = __builtin_popcount(s);
        for (set_t subset :subsets) {
            set_t s_without_x = s xor subset;
            if ((__builtin_popcount(s_without_x) % 2) ) {
                original_f[s] = original_f[s] - f(element_count, subset);
            } else {
                original_f[s] = original_f[s] + f(element_count, subset);
            }

        }
    }
    return original_f;
}



template<typename T>
vector<T> ranked_Mobius_inversion2(RankedFunction<T> &f, int n, int subset_rank) {
    int subset_count=1<<n;
    //T val(0);
    vector<T> original_f();
    original_f.reserve(subset_count);


    return original_f;
}



template<typename T>
vector<T> advanced_convolute(Function<T> &f, int n) {
    vector<vector<T> > fast_ranked_transform_f(n + 1);
    for (int i = 0; i <= n; i++) {
        fast_ranked_transform_f[i] = rankedMobius(f, n, i);
    }
    RankedVectFunction<T> f_r_func = RankedVectFunction<T>(fast_ranked_transform_f);
    vector<vector<T> > convoluted = ranked_convolute<T>(f_r_func, f_r_func, n);
    RankedVectFunction<T> conv_f = RankedVectFunction<T>(convoluted);
    vector<T> result_advanced = ranked_Mobius_inversion<T>(conv_f, n);
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
    RankedVectFunction<T> conv_f = RankedVectFunction<T>(convoluted);
    vector<T> result_advanced = ranked_Mobius_inversion<T>(conv_f, n);
    return result_advanced;
}

#if 0
template<typename T>
vector<T> advanced_convolute(Function<T> &f,Function<T> &g, int n, vector<set_t> subsets) { //TODO ADD SINGLE FUNCTION VERSION, saves 2^n by saving here
    vector<vector<T> > fast_ranked_transform_f(n + 1);
    vector<vector<T> > fast_ranked_transform_g(n + 1);
    for (int i = 0; i <= n; i++) {
        fast_ranked_transform_f[i] = rankedMobius(f, n, i);
        fast_ranked_transform_g[i] = rankedMobius(g, n,i);
    }
    RankedVectFunction<T> f_r_func = RankedVectFunction<T>(fast_ranked_transform_f);
    RankedVectFunction<T> g_r_func = RankedVectFunction<T>(fast_ranked_transform_g);
    vector<vector<T> > convoluted = ranked_convolute<T>(f_r_func, g_r_func, n);
    RankedVectFunction<T> conv_f = RankedVectFunction<T>(convoluted);
    vector<T> result_advanced = ranked_Mobius_inversion<T>(conv_f, n);
    return result_advanced;
}
#endif


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
