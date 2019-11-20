//
// Created by dominik on 10/27/19.
//

#include "benchmark_dominating_set.h"
#include "DominatingSet.h"
#include "benchmark_writer.h"
#include <chrono>
#include <random>
#include "utility.h"
void benchmark_dominating_set::two_child_propagate(int treewidth) {

    std::string name="dominating_set";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("Treewidth");
    b.write("Duration(ms)");
    b.writeln("");
    std::default_random_engine generator;



    for(int i=5;i<treewidth;++i){
        int table_size=ipow(3,treewidth);
        int previous_min=3;
        std::uniform_int_distribution<int8_t> distribution(1,10);
        int8_t* previous_table1=new int8_t[table_size];
        int8_t* previous_table2=new int8_t[table_size];
        for(int j=0;j<table_size;++j){
            previous_table1[j]=distribution(generator);
            previous_table2[j]=distribution(generator);
        }
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        DominatingSet::naive_join_node(previous_table1,previous_table2,i,previous_table1);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Classic");
        b.write(i);
        b.write(duration);
        b.writeln("");


        t1 = std::chrono::high_resolution_clock::now();
        DominatingSet::mobius_join_node(previous_table1,previous_table2,previous_min,previous_min,i,previous_table1);
        t2 = std::chrono::high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(i);
        b.write(duration);
        b.writeln("");
        delete[] previous_table1;
        delete[] previous_table2;

    }


}
