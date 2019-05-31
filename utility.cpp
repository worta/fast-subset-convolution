#include "steiner.h"
#include "mobius.h"
#include "functions.h"
#include <fstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <iostream>
#include "utility.h"
#include <cstdint>
#include <cmath>
#include <deque>
#include "common.h"
void getSubsets(set_t superset, vector<set_t> &results) {
    int index = __builtin_ffs(
            superset); //Returns one plus the index of the least significant 1-bit of x, or if x is zero, returns zero.
    if (index == 0) {
        results.push_back(0);
        return;
    } else {
        int element = 1 << (index - 1);
        int new_set = superset xor element;
        getSubsets(new_set, results);
        int current_elems = results.size();
        for (int i = 0; i < current_elems; ++i) {
            results.push_back(results[i] | element);
        }
    }

}

vector<set_t> get_subsets_it(set_t superset) {
    int limit = (int) pow(2, __builtin_popcount(superset));
    vector<set_t> r; //vector needs! to be empty at the beginning, i.e. size=0
    r.reserve(limit);
    std::deque<set_t> d;
    while (superset != 0) {
        int index = __builtin_ffs(superset);
        d.push_back(index);
        superset = superset xor (1 << (index - 1));
    }
    r.push_back(0); //empty set
    while (!d.empty()) {
        int index = d.front();
        d.pop_front();
        int currSize = r.size();
        set_t element = (1 << (index - 1));
        for (int i = 0; i < currSize; ++i) {
            r.push_back(r[i] xor element);
        }
    }
    return r;
}

vector<int> get_element_indices(set_t set) {
    vector<int> d;
    while (set != 0) {
        int index = __builtin_ffs(set);
        d.push_back(index);
        set = set xor (1 << (index - 1));
    }
    return d;
}

//taken from https://stackoverflow.com/questions/9330915/number-of-combinations-n-choose-r-in-c
unsigned nChoosek( unsigned n, unsigned k )
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

void output_set(set_t setRep, int n)
{
    cout<<"< ";
    for(set_t i=0; i<=n; i++)
    {
        if(setRep&(1<<(i-1)))
        {
            cout<<i<<" ";
        }
    }
    cout<<">";
}



#if 0
// find next k-combination "Gopher's hack"
bool next_combination(unsigned long& x) // assume x has form x'01^a10^b in binary
{
    unsigned long u = x & -x; // extract rightmost bit 1; u =  0'00^a10^b
    unsigned long v = u + x; // set last non-trailing bit 0, and clear to the right; v=x'10^a00^b
    if (v==0) // then overflow in v, or x==0
        return false; // signal that next k-combination cannot be represented
    x = v +(((v^x)/u)>>2); // v^x = 0'11^a10^b, (v^x)/u = 0'0^b1^{a+2}, and x ← x'100^b1^a
    return true; // successful completion
}
#endif
//http://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation



//K is the superset, subset_size is the desired size of the generated subsets, k is the size of K
vector<set_t> generate_subsets_of_size_k(set_t K, int subset_size, int n){
    unsigned int current_perm=(int)pow(2,subset_size)-1; // sets the first subset_size bits to 1
    unsigned int next_perm; // next permutation of bits
    unsigned check=1<<(n+1);
    if (n==0){
        return vector<set_t>(1,0); //return just the empty set
    }

    while(!(current_perm & check)){ //as long as the k+1 bit is not set

        unsigned int t = current_perm | (current_perm - 1); // t gets v's least significant 0 bits set to 1
        // Next set to 1 the most significant bit to change,
        // set to 0 the least significant ones, and add the necessary 1 bits.
        current_perm = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(current_perm) + 1));

        //now check current_perm in k?

    }

}
