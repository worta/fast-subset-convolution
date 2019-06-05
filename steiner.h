#ifndef STEINER_H_INCLUDED
#define STEINER_H_INCLUDED
#include <cstdlib>
#include <queue>
#include "boost/multi_array.hpp"
#include <limits>
#include <iostream>
#include "common.h"


typedef boost::multi_array<int, 2> weight_matrix;
typedef weight_matrix::index index;
weight_matrix compute_ap_shortest_path(weight_matrix adjancy, int size);
void test_dijkstra();
int classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K);
int mobius_dreyfuss(weight_matrix &graph_adj, int size, set_t K, int input_range);
void test_steiner();
void test_ring_embedd();
#endif // STEINER_H_INCLUDED
