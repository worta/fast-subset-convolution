#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED
#include <vector>
#include <cstdint>
#include "common.h"
using namespace std;
void getSubsets(set_t superset,vector<set_t > &results);
vector<set_t> get_subsets_it(set_t superset);
vector<int> get_element_indices(set_t set) ;
#endif // UTILITY_H_INCLUDED
