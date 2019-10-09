#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED
#include <vector>
#include <cstdint>
#include "common.h"
using namespace std;
//void getSubsets(set_t superset,vector<set_t > &results);
vector<set_t> get_subsets(set_t superset);
int get_subsets(set_t superset, set_t* subsets);
vector<int> get_element_indices(set_t set) ;
unsigned nChoosek( unsigned n, unsigned k );
void output_set(set_t setRep, int n);
vector<set_t> generate_subsets_of_size_k(set_t K, int subset_size, int bitcount);
void generate_sets_of_size_k(int subset_size, int bitcount,set_t *result);
int64_t ipow(int64_t base, uint8_t exp);
#endif // UTILITY_H_INCLUDED
