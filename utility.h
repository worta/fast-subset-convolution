#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED
#include <vector>
#include <cstdint>
#include "common.h"
using namespace std;
void getSubsets(set_t superset,vector<set_t > &results);
vector<set_t> get_subsets_it(set_t superset);
vector<int> get_element_indices(set_t set) ;
unsigned nChoosek( unsigned n, unsigned k );
void output_set(set_t setRep, int n);
vector<set_t> generate_subsets_of_size_k(set_t K, int subset_size, int bitcount);
#endif // UTILITY_H_INCLUDED
