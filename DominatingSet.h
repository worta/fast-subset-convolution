//
// Created by dominik on 10/1/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H
#define FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H

#include "utility.h"
class DominatingSet {
public:
    static void naive_join_node(int8_t* child1,int8_t* child2,int node_count,int8_t* result);
    static void mobius_join_node(int8_t *child1, int8_t *child2,int minc_1,int minc_2, int node_count,int8_t* result);
};


#endif //FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H
