//
// Created by dominik on 8/14/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_BENCHMARK_PATH_EMBEDDING_H
#define FAST_SUBSET_CONVOLUTION_BENCHMARK_PATH_EMBEDDING_H


class benchmark_path_embedding {
public:
    static void embedd_random_length_paths(int path_count_max);
    static void two_child_propagate(int path_count_max);
    static void test_child_propagation();
};


#endif //FAST_SUBSET_CONVOLUTION_BENCHMARK_PATH_EMBEDDING_H
