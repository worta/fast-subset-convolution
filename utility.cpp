#include "utility.h"
#include <cstdint>
#include <cmath>
#include <deque>

void getSubsets(uint32_t superset, vector<uint32_t> &results) {
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

vector<uint32_t> getSubsetsIt(uint32_t superset) {
    int limit = (int) pow(2, __builtin_popcount(superset));
    vector<uint32_t> r; //vector needs! to be empty at the beginning, i.e. size=0
    r.reserve(limit);
    std::deque<uint32_t> d;
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
        uint32_t element = (1 << (index - 1));
        for (int i = 0; i < currSize; ++i) {
            r.push_back(r[i] xor element);
        }
    }
    return r;
}

