//
// Created by dominik on 7/4/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H
#define FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H

#include "common.h"
#include "Tree.h"
#include <vector>
#include "utility.h"
#include "FastSubsetConvolution.h"

class Path_Embedding {
public:
    //Path_Embedding();
    int embedd_mobius(Tree &tree, std::vector<int> &path_lengths);

    int embedd_naive(Tree &tree, std::vector<int> &path_lengths);

    static void generateFullTrees(int depth, Tree &t);

    static void two_children_propagate_mobius(FastSubsetConvolution<int8_t> &conv, int8_t *table_l, int8_t *table_r,
                                              vector<int> &path_lengths, int depth, int8_t *table_out);

    static void
    two_child_propagate_direct(int8_t *table_l, int8_t *table_r, vector<int> &path_lengths, int8_t *table_out);
};


#endif //FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H
