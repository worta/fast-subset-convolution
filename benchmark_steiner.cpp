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
void benchmark_steiner::complete_graphs(int max_size){
    int k=3;
    std::string name="steiner_benchmark";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("Nodes");
    b.write("Duration(ms)");
    b.writeln("");
    for(int nodes=4;nodes<max_size;++nodes){
        if(nodes%10==0){
            std::cout<<"Finished "<<nodes <<"\\"<<max_size<<"\n";
        }

        adjancy_matrix graph=GraphGenerator::generate_complete_graph_with_uniform_weights(nodes,1);
        int test_set=0b1111;
        //Test naive
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        int class_result=classic_dreyfuss_wagner(graph,nodes,test_set);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        b.write("Classic");
        b.write(nodes);
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write(duration);
        b.writeln("");
        //test mobius
        t1 = std::chrono::high_resolution_clock::now();
        int mob_result=mobius_dreyfuss(graph,nodes,test_set,1);
        t2 = std::chrono::high_resolution_clock::now();
        b.write("Mobius");
        b.write(nodes);
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write(duration);
        b.writeln("");
    }
    b.close();

}