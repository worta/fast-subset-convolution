//
// Created by dominik on 7/4/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H
#define FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H
#include "common.h"
#include "Tree.h"
#include <vector>
class Path_Embedding {
    static void embedd(Tree& tree, std::vector<int> path_lengths);
    static void embedd_naive(Tree& tree, std::vector<int> path_lengths);
    static void generateFullTrees(int depth,Tree& t);
};


#endif //FAST_SUBSET_CONVOLUTION_PATH_EMBEDDING_H
