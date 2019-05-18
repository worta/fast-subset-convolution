#ifndef STEINER_H_INCLUDED
#define STEINER_H_INCLUDED
#include <cstdlib>
#include <queue>
#include "boost/multi_array.hpp"
#include <limits>
#include <iostream>


typedef boost::multi_array<int, 2> weight_matrix;
typedef weight_matrix::index index;
weight_matrix compute_ap_shortest_path(weight_matrix adjancy, int size);
void test_dijkstra();
void test_steiner();
void naive_dreyfuss_wagner();
#endif // STEINER_H_INCLUDED
