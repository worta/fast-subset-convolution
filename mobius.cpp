#include <vector>
#include <cmath>
#include <algorithm>
#include "functions.h"
#include "utility.h"
#include <cstdint>
#include "common.h"


using namespace std;

template<typename T>
vector<T> fastMobius(Function<T> &f, int n) {
    vector<vector<int> > d(n + 1, vector<int>((int) pow(2, n)));
    for (int k = 0; k < pow(2, n); k++) {
        d[0][k] = f(k);
    }
    for (set_t j = 1; j < n + 1; j++) {
        for (set_t k = 0; k < (int) pow(2, n); k++) {
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
    vector<vector<int> > d(n + 1, vector<int>((int) pow(2, n)));
    for (int k = 0; k < pow(2, n); k++) {
        d[0][k] = f_mobius(k);
    }
    for (set_t j = 1; j < n + 1; j++) {
        for (set_t k = 0; k < (int) pow(2, n); k++) {
            if (k & (1 << (j - 1))) { //if j is in K
                d[j][k] = d[j - 1][k] - d[j - 1][k ^ (1 << (j - 1))];
            } else { //j is not in K
                d[j][k] = d[j - 1][k] ;
            }
        }
    }
    return d[n];
}


template<typename T>
vector<T> rankedMobius(Function<T> &f, int n, int subsetRank) //only difference to fastMobius is the initalization
{
    vector<vector<T> > d(n + 1, vector<T>((int) pow(2, n)));//hier muss eig noch t hin
    for (int k = 0; k < pow(2, n); k++) {
        if (__builtin_popcount(k) ==
            subsetRank)  //__builtin_popcount counts bits set to 1, on hardware level if possible
        {
            d[0][k] = f(k);
        } else {
            d[0][k] = 0; //Klassen müssen eine Umwandlung von int/0 zum Ringelement erlauben
        }
    }

    for (int j = 1; j < n + 1; j++) {
        for (int k = 0; k < (int) pow(2, n); k++) {
            if (k & (1 << (j - 1)))   //is j in subset k?
            {
                d[j][k] = d[j - 1][k] + d[j - 1][k ^ (1 << (j - 1))];
            } else {
                d[j][k] = d[j - 1][k];
            }
        }
    }
    return d[n];
}



template<typename T>
vector<T> naive_convolute(Function<T> &f, Function<T> &g, int n) {
    vector<T> conv((int) pow(2, n));
    for (int k = 0; k < pow(2, n); ++k) {
        vector<set_t > subsets;
        subsets.reserve((int) pow(2, n));
        getSubsets(k, subsets);
        conv[k] = 0;
        for (set_t i = 0; i < pow(2, __builtin_popcount(k)); ++i) {
            conv[k] = conv[k] + f(i) * g(k xor i);
        }

    }
    return conv;

}


template<typename T>
vector<vector<T> > ranked_convolute(RankedFunction<T> &f, RankedFunction<T> &g, int n) {
    //Create result vector
    vector<vector<T> > conv(n + 1, vector<T>((int) pow(2, n)));
    int subset_count = (int) pow(2, n);
    for (int k = 0; k <= n; k++) {
        for (int subset = 0; subset < subset_count; subset++) {

            conv[k][subset] = 0;
            for (int j = 0; j <= k; j++) {
                conv[k][subset] = conv[k][subset] + f(j, subset) * g(k - j, subset);
                /*cout<<"k:"<<k<<" j:"<<j<<" subset:";
                output_set(subset,n);
                cout<<" "<<conv[k][subset]<<endl;*/
            }


        }
    }
    return conv;
}

template<typename T>
vector<T> ranked_Mobius_inversion(RankedFunction<T> &f, int n) {
    vector<T> original_f((int) pow(2, n));
    for (set_t s = 0; s < pow(2, n); ++s) {
        vector<set_t > subsets;
        subsets.reserve((int) pow(2, n));
        getSubsets(s, subsets);
        original_f[s] = 0;
        int element_count = __builtin_popcount(s);
        //for(int i=0;i<pow(2,__builtin_popcount(s));++i){
        for (set_t i = 0; i < subsets.size(); ++i) {
            set_t s_without_x = s xor subsets[i];
            //int factor=1-2*(__builtin_popcount(s_without_x)%2);
            if ((__builtin_popcount(s_without_x) % 2) == 0) {
                original_f[s] = original_f[s] + f(element_count, subsets[i]);
            } else {
                original_f[s] = original_f[s] - f(element_count, subsets[i]);
            }

        }
    }
    return original_f;
}




template<typename T>
vector<T> advanced_convolute(Function<T> &f,Function<T> &g, int n) { //TODO ADD SINGLE FUNCTION VERSION, saves 2^n by saving here
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
vector<T> advanced_covering_product(Function<T> &f,Function<T> &g, int n) {
    vector<T> mobius_f=fastMobius<T>(f,n);
    vector<T> mobius_g=fastMobius<T>(g,n);

    //element wise product
    vector<T> hadamard;
    hadamard.reserve(mobius_f.size());
    for(int i=0;i<mobius_f.size();++i){
        hadamard[i]=mobius_f[i]*mobius_g[i];
    }
    VectFunction<T> hadF=VectFunction<T>(hadamard);
    vector<T> result=fastMobiusInversion(hadF,n);
    return result;
}
