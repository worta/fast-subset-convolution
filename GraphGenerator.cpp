//
// Created by dominik on 6/14/19.
//

#include "GraphGenerator.h"
#include <random>

adjancy_matrix GraphGenerator::generate_complete_graph_with_random_weights(int n){
    adjancy_matrix graph(boost::extents[n][n]);
    std::default_random_engine generator;
    std::uniform_int_distribution<int8_t> distribution(1,10);
    for(int i=0;i<n;++i){
        for(int j=0;j<n;++j){
            if(i==j){
                graph[i][j]=0;
            }
            else{
                graph[i][j]=distribution(generator);
            }

        }
    }
    return graph;
    //the resulting steiner tree should probably always just contain the nodes of K with weight (n-1)*weight
}