//
// Created by dominik on 6/14/19.
//

#include "benchmark_steiner.h"
#include "GraphGenerator.h"
#include "common.h"
#include "steiner.h"
#include <chrono>
#include "benchmark_writer.h"
#include "string"

//Test with complete graphs of increasing size
//maybe return time
void benchmark_steiner::complete_graphs(int y_max,int max_size){
    std::string name="steiner_benchmark";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("|Y|");
    b.write("Duration(ms)");
    b.write("Nodes");
    b.writeln("");

    adjancy_matrix graph= GraphGenerator::generate_complete_graph_with_random_weights(max_size);
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph,max_size);
    for(int k=5;k<=y_max;++k){
        int test_set=((int)pow(2,k))-1;
        //Test naive
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        classic_dreyfuss_wagner(graph,pair_wise_dist, max_size, test_set);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        b.write("Classic");
        b.write(k);
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write(duration);
        b.write(max_size);
        b.writeln("");
        //test mobius
        t1 = std::chrono::high_resolution_clock::now();
        mobius_dreyfuss(graph,pair_wise_dist,max_size,test_set,10);
        t2 = std::chrono::high_resolution_clock::now();
        b.write("Mobius");
        b.write(k);
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write(duration);
        b.write(max_size);
        b.writeln("");
    }
    b.close();

}