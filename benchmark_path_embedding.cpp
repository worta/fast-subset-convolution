//
// Created by dominik on 8/14/19.
//

#include "benchmark_path_embedding.h"
#include "common.h"
#include <chrono>
#include "benchmark_writer.h"
#include <vector>
#include "Tree.h"
#include "Path_Embedding.h"
#include <iostream>
#include <memory>
#include <random>

void benchmark_path_embedding::embedd_random_length_paths(int path_count_max) { //todo change to random
    std::vector<int> path_lengths=std::vector<int>();
    std::shared_ptr<Tree> root(new Tree(0));
    Path_Embedding::generateFullTrees(5,*root);
    Path_Embedding p;
    std::string name="path_emb_full_tree";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("TDepth");
    b.write("Paths");
    b.write("Duration(ms)");
    b.writeln("");
    std::default_random_engine generator;
    std::uniform_int_distribution<int8_t> distribution(1,2);
    for(int i=1;i<=path_count_max;++i){
        path_lengths.push_back(distribution(generator)); //add path of length one to paths
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        int result=p.embedd_naive(*root,path_lengths);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Classic");
        b.write(2);
        b.write(path_lengths.size());
        b.write(duration);
        b.writeln("");
        std::cout <<"Done "<<i<<"/"<<path_count_max<<" with result:"<<result<<std::endl;
        t1 = std::chrono::high_resolution_clock::now();
        int result2= p.embedd_mobius(*root, path_lengths);
        t2 = std::chrono::high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(2);
        b.write(path_lengths.size());
        b.write(duration);
        b.writeln("");
        std::cout <<"Done "<<i<<"/"<<path_count_max<<" with result:"<<result<<std::endl;
        assert(result==result2);
    }
    b.close();
}


void benchmark_path_embedding::two_child_propagate(int path_count_max) { //todo change to random
    std::vector<int> path_lengths=std::vector<int>();
    std::shared_ptr<Tree> root(new Tree(0));
    Path_Embedding::generateFullTrees(2,*root);
    Path_Embedding p;
    std::string name="path_emb_only_2_child_propagate";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("TDepth");
    b.write("Paths");
    b.write("Duration(ms)");
    b.writeln("");
    std::default_random_engine generator;
    std::uniform_int_distribution<int8_t> distribution_path(1,10);
    std::uniform_int_distribution<int8_t> distribution(-1,5);
    int8_t* first_child=new int8_t[1<<path_count_max];
    int8_t* second_child=new int8_t[1<<path_count_max];
    int8_t* result_table=new int8_t[1<<path_count_max];
    for(int i=0;i<1<<path_count_max;++i){
        first_child[i]=distribution(generator);
        second_child[i]=distribution(generator);
    }


    for(int i=1;i<=path_count_max;++i){
        path_lengths.push_back(distribution_path(generator));
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        //p.two_child_propagate_direct(first_child,second_child,path_lengths,result_table);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        /*b.write("Classic");
        b.write(2);
        b.write(path_lengths.size());
        b.write(duration);
        b.writeln("");*/
        //std::cout <<"Done "<<i<<"/"<<path_count_max<<std::endl;
        FastSubsetConvolution<int8_t> f(path_lengths.size(),true); //need only covering product
        t1 = std::chrono::high_resolution_clock::now();
        p.two_children_propagate_mobius(f,first_child,second_child,path_lengths,10,result_table);
        t2 = std::chrono::high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(2);
        b.write(path_lengths.size());
        b.write(duration);
        b.writeln("");
       // std::cout <<"Done "<<i<<"/"<<path_count_max<<std::endl;
    }
    b.close();
}


void benchmark_path_embedding::test_child_propagation() {
    Path_Embedding p;
    int8_t  *out_table = new int8_t [4];
    int8_t  in_table2a[] = {0, -100, -100, -100}; //p1=4,p2=2, just a leaf
    int8_t  in_table2b[] = {2, 0, 2, -100};//p1=4,p2=2, full tree with depth 2
    vector<int> path2 = {4,2};
    p.two_child_propagate_direct(in_table2a, in_table2b, path2, out_table); //result should be {1,1,3,?}
    for(int i=0;i<4;++i){
        std::cout<<std::to_string(out_table[i])<<",";
    }
    std::cout<<endl;
    FastSubsetConvolution<int8_t> f(2,true);
    p.two_children_propagate_mobius(f,in_table2a, in_table2b, path2,3,out_table); //result should be same as above
    for(int i=0;i<4;++i){
        std::cout<<std::to_string(out_table[i])<<",";
    }

    delete[] out_table;
}


