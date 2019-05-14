#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED
#include <vector>
#include <cstdint>
#include "common.h"
using namespace std;
void getSubsets(set_t superset,vector<set_t > &results);
vector<set_t> getSubsetsIt(set_t superset);
#endif // UTILITY_H_INCLUDED
