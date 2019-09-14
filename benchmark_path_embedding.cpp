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
void benchmark_path_embedding::embedd_length_1_paths(int path_count_max) {
    std::vector<int> path_lengths=std::vector<int>();
    std::shared_ptr<Tree> root(new Tree(0));
    Path_Embedding::generateFullTrees(2,*root);
    Path_Embedding p;
    std::string name="path_emb_full_tree";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("TDepth");
    b.write("Paths");
    b.write("Duration(ms)");
    b.writeln("");
    for(int i=1;i<=path_count_max;++i){
        path_lengths.push_back(1); //add path of length one to paths
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
        result= p.embedd_mobius(*root, path_lengths);
        t2 = std::chrono::high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(2);
        b.write(path_lengths.size());
        b.write(duration);
        b.writeln("");
        std::cout <<"Done "<<i<<"/"<<path_count_max<<" with result:"<<result<<std::endl;
    }
    b.close();
}
