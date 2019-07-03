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
#endif //FAST_SUBSET_CONVOLUTION_COMMON_H
