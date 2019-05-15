#include <cstdlib>
#include <queue>
#include "boost/multi_array.hpp"
#include <limits>
#include <iostream>
#include "steiner.h"
#include "utility.h"
#include "common.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <deque>
#include <set>
#include <array>

typedef boost::multi_array<int, 2> weight_matrix;
typedef boost::multi_array<int, 2> intd2_arr;
typedef weight_matrix::index index;



struct Node {
    int idx;
    int dist;

    Node(int i, int d) {
        idx = i;
        dist = d;
    }
};

weight_matrix compute_ap_shortest_path(weight_matrix adjancy, int size) {
    weight_matrix distances(boost::extents[size][size]);
    //bool visited [size] ;
    //initialize distance matrix
    for (index i = 0; i < size; ++i) {
        //visited[i]=false;
        for (index j = 0; j < size; ++j) {
            if (i != j) {
                distances[i][j] = std::numeric_limits<int>::max();
            } else {
                distances[i][i] = 0;
            }

        }
    }
    //Dijkstra for each node
    auto cmp = [](Node left, Node right) { return (left.dist) < (right.dist); };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> queue(cmp);
    for (int i = 0; i < size; ++i) {
        Node start = Node(i, 0);
        queue.push(start);
        while (!queue.empty()) {
            Node check = queue.top();
            queue.pop();
            //visited[check.idx]=true;
            for (int j = 0; j < size; ++j) {
                if (((adjancy[check.idx][j] > 0) and (i != j))) {
                    int new_dist = distances[i][check.idx] + adjancy[check.idx][j];
                    if (new_dist <= distances[i][j]) {
                        distances[i][j] = new_dist;
                        Node add = Node(j, new_dist);
                        queue.push(add);
                    }
                }
            }

        }

    }
    return distances;
}

void test_dijkstra() {
    /*  a --  1 --  b -- 1 --   d
     *     --2      |2
     *         --   c
     */
    std::cout << "Test Dijkstra: \n";
    weight_matrix graph(boost::extents[4][4]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            graph[i][j] = -1;
            if (i == j) {
                graph[i][i] = 0;
            }
        }
    }
    graph[0][1] = 1;
    graph[1][0] = 1;
    graph[0][2] = 2;
    graph[2][0] = 2;
    graph[1][2] = 2;
    graph[2][1] = 2;
    graph[1][3] = 1;
    graph[3][1] = 1;
    weight_matrix distances = compute_ap_shortest_path(graph, 4);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << distances[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "should be\n";
    std::cout << "0 1 2 2\n1 0 2 1\n2 2 0 3\n2 1 3 0";

}
//bool set_size_cmp (int i,int j) { return (__builtin_popcount(i)<__builtin_popcount(j)); }

inline set_t to_byte_repr(vector<int> indices){
    set_t set_repr=0;
    for(int elem:indices) {
        set_repr+=1<<(elem-1);
    }
    return set_repr;
}



int eval_g(weight_matrix &pair_wise_dist,vector<int> &W,int &nodes,set_t set_repr);
//Both eval W and eval g should keep track of a list of optimal choices
int eval_W(weight_matrix &pair_wise_dist,set_t set_repr,vector<int> &W,int &nodes) {
    int ele_count=__builtin_popcount(set_repr);
    if(ele_count<2){
        return 0;
    }
    if(ele_count==2){
        int ele1=__builtin_ffs(set_repr)-1;
        set_repr=set_repr xor (1<<ele1);
        int ele2=__builtin_ffs(set_repr)-1;
        return pair_wise_dist[ele1][ele2];
    }
    int q=__builtin_ffs(set_repr);
    set_repr=set_repr xor (1<<q);
    if(W[set_repr]>=0) {
        return W[set_repr];
    }
    else{
        int min=INT_MAX;
        int value;
        for(int i=0;i<nodes;++i) {
            value=pair_wise_dist[q-1][i]+eval_g(pair_wise_dist,W,nodes,set_repr);
            if(value<min){
                min=value;
            }
        }
    }
}


int eval_g(weight_matrix &pair_wise_dist,vector<int> &W,int &nodes,set_t set_repr){
    vector<set_t> sets=get_subsets_it(set_repr);
    int min=INT_MAX;
    int value;
    for(set_t set:sets){
        if(set!=0 && set!=set_repr){ //not empty and not all of X
            value=eval_W(pair_wise_dist,set,W,nodes)+eval_W(pair_wise_dist,set_repr xor set,W,nodes);
            if(value<min){
                min=value;
            }
        }
    }
    return value;
}

//K is a subset of the nodes of the graph, called the terminals in the Steiner Tree problem. The set is
//represented as bit mask, e.g. the set {1,3} would be 0...0101, so the first and third bit are set.
void classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    vector<int> temp=get_element_indices(K);
    set<int> k_set=set<int>(temp.begin(),temp.end());
    vector<int>  W ((int)pow(2,k_set.size()),-1);
    cout<< W[0]<<endl;
    intd2_arr  g(boost::extents[size][k_set.size()]);
}



void mobius_dreyfuss(){



}












