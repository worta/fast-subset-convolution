//
// Created by dominik on 10/1/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H
#define FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H

#include "utility.h"
class DominatingSet {



    static void naive_join_node(int8_t* child1,int8_t* child2,int node_count);
    static void mobius_join_node(int8_t* child1,int8_t* child2,int node_count);
};


#endif //FAST_SUBSET_CONVOLUTION_DOMINATINGSET_H
