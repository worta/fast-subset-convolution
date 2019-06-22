//
// Created by dominik on 6/14/19.
//

#include "benchmark_steiner.h"
#include "GraphGenerator.h"
#include "common.h"
#include "steiner.h"
//TODO add file handler to write benchmarks
//Test with complete graphs of increasing size
//maybe return time
void benchmark_steiner::complete_graphs(int max_size){
    int k=3;
    for(int nodes=4;nodes<max_size;++nodes){
        adjancy_matrix graph=GraphGenerator::generate_complete_graph_with_uniform_weights(nodes,1);
        int test_set=0b1111;
        //Test naive

        int class_result=classic_dreyfuss_wagner(graph,nodes,test_set);



        //test mobius
        int mob_result=mobius_dreyfuss(graph,nodes,test_set,1);
    }


}