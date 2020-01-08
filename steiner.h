#ifndef STEINER_H_INCLUDED
#define STEINER_H_INCLUDED
#include <cstdlib>
#include <queue>
#include "boost/multi_array.hpp"
//#include <limits>
#include <iostream>
#include "common.h"
#include "MinSumRingEmbedd.h"


weight_matrix compute_ap_shortest_path(weight_matrix adjancy, int size);
int classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K);
int classic_dreyfuss_wagner(weight_matrix &graph_adj,weight_matrix &pair_wise_dist, int size, set_t K);
int mobius_dreyfuss(weight_matrix &graph_adj,weight_matrix &pair_wise_dist, int size, set_t K,int input_range);
int mobius_dreyfuss(weight_matrix &graph_adj,int size, set_t K,int input_range);

#endif // STEINER_H_INCLUDED
