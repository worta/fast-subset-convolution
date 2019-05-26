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