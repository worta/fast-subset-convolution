//
// Created by dominik on 6/14/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_GRAPHGENERATOR_H
#define FAST_SUBSET_CONVOLUTION_GRAPHGENERATOR_H
#include "boost/multi_array.hpp"
typedef boost::multi_array<int, 2> adjancy_matrix;
typedef adjancy_matrix::index index;
class GraphGenerator {
  adjancy_matrix generate_complete_graph_with_uniform_weights(int n, int weight);
};


#endif //FAST_SUBSET_CONVOLUTION_GRAPHGENERATOR_H
