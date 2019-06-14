//
// Created by dominik on 6/14/19.
//

#include "GraphGenerator.h"
adjancy_matrix GraphGenerator::generate_complete_graph_with_uniform_weights(int n, int weight){
    adjancy_matrix graph(boost::extents[n][n]);
    for(int i=0;i<n;++i){
        for(int j=0;j<n;++j){
            if(i==j){
                graph[i][j]=0;
            }
            else{
                graph[i][j]=weight;
            }

        }
    }
    return graph;

}