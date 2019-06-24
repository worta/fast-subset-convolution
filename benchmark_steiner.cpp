//
// Created by dominik on 6/14/19.
//

#include "benchmark_steiner.h"
#include "GraphGenerator.h"
#include "common.h"
#include "steiner.h"
#include <chrono>
#include "benchmark_writer.h"

//Test with complete graphs of increasing size
//maybe return time
void benchmark_steiner::complete_graphs(int max_size){
    int k=3;
    for(int nodes=4;nodes<max_size;++nodes){
        adjancy_matrix graph=GraphGenerator::generate_complete_graph_with_uniform_weights(nodes,1);
        int test_set=0b1111;
        //Test naive
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        int class_result=classic_dreyfuss_wagner(graph,nodes,test_set);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        //test mobius
        int mob_result=mobius_dreyfuss(graph,nodes,test_set,1);
        std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

    }


}