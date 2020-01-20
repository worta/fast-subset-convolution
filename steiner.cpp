#include <cstdlib>
#include <queue>
#include "boost/multi_array.hpp"
#include <limits>
#include <iostream>
#include "steiner.h"
#include "utility.h"
#include "common.h"
#include "mobius.h"
#include "functions.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <deque>
#include <set>
#include <array>
#include "MinSumRingEmbedd.h"
#include <cassert>
#include "FastSubsetConvolution.h"

typedef boost::multi_array<int, 2> weight_matrix;
typedef weight_matrix::index index;

#define STEINER_SET_MAX_SIZE 10000
//more than enough for maximal steiner size

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

//bool set_size_cmp (int i,int j) { return (__builtin_popcount(i)<__builtin_popcount(j)); }

inline set_t to_byte_repr(vector<int> &indices) {
    set_t set_repr = 0;
    for (int elem:indices) {
        set_repr += 1 << (elem - 1);
    }
    return set_repr;
}

int classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    return classic_dreyfuss_wagner(graph_adj,pair_wise_dist, size, K);
}

int classic_dreyfuss_wagner(weight_matrix &graph_adj, weight_matrix &pair_wise_dist, int size, set_t K) {
    //weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    vector<int> indices = get_element_indices(K);
    int k = __builtin_popcount(K);
    if (k == 2) { //return the shortest path
        return pair_wise_dist[indices[0] - 1][indices[1] - 1];
    }
    vector <unordered_map<set_t, int>> s(size, unordered_map<set_t, int>());

    //relabel
    int relabel[size];
    for (int i = 0; i < indices.size(); ++i) {
        relabel[i] = indices[i] - 1;
    }
    int new_index = indices.size();
    for (int i = 0; i < size; ++i) {
        if (std::find(indices.begin(), indices.end(), i + 1) == indices.end()) { //works for n>32 if k is below 32
            relabel[new_index] = i;
            new_index++;
        }
    }
    set_t relabeld_K = 0;
    for (int i = 0; i < indices.size(); i++) { //relabeld_K=1<<indices.size -1
        relabeld_K = relabeld_K | (1 << i);
    }
    //choose q:
    int original_q =
            indices[indices.size() - 1] - 1;  //this is the q that corresponds to the highest bit in the relabeld k
    int q = 1 << (k - 1);
    set_t C = relabeld_K xor q;
    vector<int> new_indices = get_element_indices(C);
    for (int i = 0; i < size; ++i) {
        for (int p = 0; p < k; ++p) {
            s[i][1 << p] = pair_wise_dist[relabel[i]][relabel[p]];
        }
    }

    for (int m = 2; m < k - 1; ++m) { //|K|-1 is equal to |C|
        vector <set_t> Ds = generate_subsets_of_size_k(C, m, k - 1);
        for (set_t D:Ds) {
            for (int i = 0; i < size; ++i) {
                s[i][D] = 100000;//TODO max finden
            }
            for (int j = 0; j < size; ++j) {
                int u = 100000;
                vector <set_t> Es = get_subsets(D);
                set_t D_1 = 1 << (__builtin_ffs(D) - 1);
                for (set_t e:Es) {
                    if ((D_1 bitand e) and (e != D)) { //is D_1 in e and e real subset of D
                        int val = s[j][e] + s[j][D xor e]; //D xor e is D-e (D\e) if e is subset of D
                        if (val < u) {
                            u = val;
                        }
                    }
                }
                for (int i = 0; i < size; ++i) {
                    int val = pair_wise_dist[relabel[i]][relabel[j]] + u;
                    if ((s[i].find(D) == s[i].end()) or (val < s[i][D])) {
                        s[i][D] = val;
                    }
                }
            }
        }
    }
    int result = 100000;
    for (int j = 0; j < size; ++j) {
        int u = 10000000;
        vector <set_t> Es = get_subsets(C);
        set_t C_1 = 1 << (__builtin_ffs(C) - 1);
        for (set_t e:Es) {
            if ((C_1 bitand e) and (e != C)) {
                int val = s[j][e] + s[j][C xor e]; //D xor e is D-e (D\e) if e is subset of D
                if (val < u) {
                    u = val;
                }
            }
        }
        int value = pair_wise_dist[relabel[j]][original_q] + u;
        if (value < result) {
            result = value;
        }
    }
    //cout<<result<<endl;
    return result;
}


class Function_p
        : public Function<int> {

private:
    int level;
    vector <vector<int>> W;
    int p;
public:
    Function_p(vector <vector<int>> &W_, int p_) :
            W(W_), p(p_) {
    };

    int operator()(set_t s) override {
        return W[p][s];
    }

};

class Function_Embedd : public Function<MinSumRingEmbedd> {
    Function_p &f_p;
public:
    explicit Function_Embedd(Function_p &f) :
            f_p(f) {
    }


    MinSumRingEmbedd operator()(set_t s) override {
        return MinSumRingEmbedd(f_p(s));
    }
};


int mobius_dreyfuss(weight_matrix &graph_adj, int n, set_t K,int input_range) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, n);
    return mobius_dreyfuss(graph_adj, pair_wise_dist, n,K,input_range);
}

int mobius_dreyfuss(weight_matrix &graph_adj, weight_matrix &pair_wise_dist, int n, set_t K, int input_range) {
    int k = __builtin_popcount(K);
    vector<int> indices = get_element_indices(K);
    if (k == 2) { //just shortest path
        return pair_wise_dist[indices[0] - 1][indices[1] - 1];
    }
    int max_value = (n - 1) * input_range + 1;
    vector<vector<int> > W(n, vector<int>((int) pow(2, k), max_value));
    vector<vector<MinSumRingEmbedd> > g(n, vector<MinSumRingEmbedd>((int) pow(2, k)));

    //relabel
    int relabel[n];
    for (int i = 0; i < indices.size(); ++i) {
        relabel[i] = indices[i] - 1;
    }
    int new_index = indices.size();
    for (int i = 0; i < n; ++i) {
        //if (!(K bitand (1 << i))) { //is i in K
        if(std::find(indices.begin(), indices.end(), i+1) == indices.end()){ //works for n>32 if k is below 32
            relabel[new_index] = i;
            new_index++;
        }
    }
    //Subsets can now be treated as 000..011..1
    set_t relabeld_K = 0;
    for (int i = 0; i < indices.size(); i++) {
        relabeld_K = relabeld_K | (1 << i);
    }

    //init for W for l=2
    for (int q = 0; q < n; ++q) {
        for (int p = 0; p < k; p++) {
            W[q][1 << p] = pair_wise_dist[relabel[q]][relabel[p]];
        }
    }

    //init for Y=1;
    for (int q = 0; q < n; ++q) {
        W[q][0] = 0;
    }

    //levelwise computation
    for (int l = 2; l < k; ++l) {
        vector<set_t> Xs = generate_subsets_of_size_k(relabeld_K, l,
                                                      k);
        for (int p = 0; p < n; ++p) {
            Function_p f_p(W, p);
            Function_Embedd f(f_p);
            g[p] = advanced_convolute<MinSumRingEmbedd>(f,k);
        }
        for (unsigned int q = 0; q < k; ++q) {
            for (set_t X:Xs) {
                if ((X bitand (1 << q)) == 0) { //for all X with q not in X
                    int min_value = STEINER_SET_MAX_SIZE;
                    for (int p = 0; p < n; ++p) {
                        int value = pair_wise_dist[relabel[q]][relabel[p]] + g[p][X].min();
                        if (value < min_value) {
                            min_value = value;
                        }
                    }

                    W[q][X] = min_value;
                }
            }
        }

    }
    
    int result=STEINER_SET_MAX_SIZE;
    for(int q=0;q<k;++q){
        int value=W[q][relabeld_K xor (1<<q)];
        if(value<result){
            result=value;
        }
    }

    return result;
}
