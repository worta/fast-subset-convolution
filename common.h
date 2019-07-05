//
// Created by dominik on 5/14/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_COMMON_H
#define FAST_SUBSET_CONVOLUTION_COMMON_H
#include <cstdint>
#include "boost/multi_array.hpp"
typedef boost::multi_array<int, 2> adjancy_matrix;
typedef adjancy_matrix::index index;
typedef  uint_fast64_t set_t;
typedef boost::multi_array<int, 2> weight_matrix;
typedef weight_matrix::index index;

#endif //FAST_SUBSET_CONVOLUTION_COMMON_H
