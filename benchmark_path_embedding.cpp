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
    for(int i=1;i<=path_count_max;++i){
        path_lengths.push_back(1); //add path of length one to paths
       // if(i==7){
            int result=p.embedd_naive(*root,path_lengths);
            std::cout <<"Done "<<i<<"/"<<path_count_max<<" with result:"<<result<<std::endl;
            result=p.embedd(*root,path_lengths);
            std::cout <<"Done "<<i<<"/"<<path_count_max<<" with result:"<<result<<std::endl;
  //      }

    }

}
