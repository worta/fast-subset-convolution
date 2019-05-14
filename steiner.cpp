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
typedef boost::multi_array<int, 2> weight_matrix;
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
bool set_size_cmp (int i,int j) { return (__builtin_popcount(i)<__builtin_popcount(j)); }



//Both eval W and eval g should return the list of optimal choices
void eval_W() {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    vector<int> W((int)pow(2,num_terminals)); //need W[X] with X subset of K
    vector<int> g((int)pow(2,num_terminals)); //need g[p][X], with p element of V and X subset of K
    eval_W(X_without_q)


}
void eval_g(){


}

//K is a subset of the nodes of the graph, called the terminals in the Steiner Tree problem. The set is
//represented as bit mask, e.g. the set {1,3} would be 0...0101, so the first and third bit are set.
void classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K, int num_terminals) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    vector<int> W((int)pow(2,num_terminals),-1); //need W[X] with X subset of K
    vector<int> g((int)pow(2,num_terminals),-1); //need g[p][X], with p element of V and X subset of K

    get_element_indices(K);

}