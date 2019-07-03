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

//bool set_size_cmp (int i,int j) { return (__builtin_popcount(i)<__builtin_popcount(j)); }

inline set_t to_byte_repr(vector<int> &indices) {
    set_t set_repr = 0;
    for (int elem:indices) {
        set_repr += 1 << (elem - 1);
    }
    return set_repr;
}



int eval_g(weight_matrix &pair_wise_dist, unordered_map<set_t,int> &W, int &nodes, set_t set_repr, int p, vector<unordered_map<set_t,int> >  &g);

//Both eval W and eval g should keep track of a list of optimal choices
//int eval_W(weight_matrix &pair_wise_dist, set_t set_repr, vector<int> &W, intd2_arr &g, int &nodes) {
int eval_W(weight_matrix &pair_wise_dist, set_t set_repr, unordered_map<set_t,int> &W, vector<unordered_map<set_t,int> >  &g, int &nodes) {
    int ele_count = __builtin_popcount(set_repr);
    int min = INT_MAX;
    if (ele_count < 2) {
        return 0;
    }
    if (ele_count == 2) {
        int ele1 = __builtin_ffs(set_repr) - 1;
        set_repr = set_repr xor (1 << ele1);
        int ele2 = __builtin_ffs(set_repr) - 1;
        return pair_wise_dist[ele1][ele2];
    }
    int q = __builtin_ffs(set_repr) - 1;
    set_t without_q = set_repr xor (1 << q); //remove q
    auto ret=W.find(set_repr);
    if(ret!=W.end()){
        return  ret->second;
    } else {
        for (int i = 0; i < nodes; ++i) {
            int  value = pair_wise_dist[q ][i];
            value+= eval_g(pair_wise_dist, W, nodes, without_q, i, g);
            if (value < min) {
                min = value;
            }
        }
    }
    W[set_repr] = min; //put back q
    return min;
}


int eval_g(weight_matrix &pair_wise_dist, unordered_map<set_t,int> &W, int &nodes, set_t set_repr, int p,
           vector<unordered_map<set_t,int> > &g) {
    auto ret=g[p].find(set_repr);
    if(ret!=g[p].end()){
        return g[p][set_repr];
    }
    vector<set_t> sets = get_subsets_it(set_repr);
    int min = INT_MAX;
    int value;
    for (set_t set:sets) {
        if (set != 0 && set != set_repr) { //not empty and not all of X
            value = eval_W(pair_wise_dist, set bitor (1 << p), W, g, nodes) +
                    eval_W(pair_wise_dist, (set_repr xor set) bitor (1<<p), W, g, nodes);
            if (value < min) {
                min = value;
            }
        }
    }
    g[p][set_repr] = min;
    return min;
}


int classic_dreyfuss_wagner2(weight_matrix &graph_adj,int n, set_t K){
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, n);
    int k=__builtin_popcount(K);
    vector<unordered_map<set_t,int> >  s(n,unordered_map<set_t,int>()); //TODO: can change map to array/vector
    vector<int> indices=get_element_indices(K);
    //relabel
    int relabel[n];
    for (int i = 0; i < indices.size(); ++i) {
        relabel[i] = indices[i] - 1;
    }
    int new_index = indices.size();
    for (int i = 0; i < n; ++i) {
        if(std::find(indices.begin(), indices.end(), i+1) == indices.end()){ //works for n>32 if k is below 32
            relabel[new_index] = i;
            new_index++;
        }
    }
    set_t relabeld_K = 0;
    for (int i = 0; i < indices.size(); i++) {
        relabeld_K = relabeld_K | (1 << i);
    }
    //choose q:
    int q=1<<(__builtin_ffs(relabeld_K)-1);
    set_t C=relabeld_K xor q;
    vector<int> new_indices = get_element_indices(C);
    for(int i=0;i<n;++i){
        for(int p=0;p<k;++p){
            s[i][1<<p]=pair_wise_dist[relabel[i]][relabel[p]];
        }
    }

    for(int m=2;m<k-1;++m){ //|K|-1 is equal to |C|
        vector<set_t> Ds = generate_subsets_of_size_k(C,m,k-1);
        for(set_t D:Ds){
            for(int i=0;i<n;++i){
                s[i][D]=100000;//TODO max finden
            }
            for(int j=0;j<n;++j){
                int u=100000;
                vector<set_t> Es=get_subsets_it(D);
                set_t D_1=1<<(__builtin_ffs(D)-1);
                for(set_t e:Es){
                    if((D_1 bitand e) and (e !=D)){ //is D_1 in e and e real subset of D
                        int val=s[j][e]+s[j][D xor e]; //D xor e is D-e (D\e) if e is subset of D
                        if(val<u){
                            u=val;
                        }
                    }
                }
                for(int i=0;i<n;++i){
                    int val=pair_wise_dist[relabel[i]][relabel[j]]+u;
                    if (s[i].find(D) == s[i].end() )
                    {
                        s[i][D]=val;
                    }
                    else{
                        if(val<s[i][D]){
                            s[i][D]=val;
                        }
                    }
                }
            }
        }
    }
    int result=100000;
    for(int j=0;j<n;++j){
        int u=10000000;
        vector<set_t> Es=get_subsets_it(C);
        set_t C_1=1<<(__builtin_ffs(C)-1);
        for(set_t e:Es) {
            if ((C_1 bitand e) and (e != C)) {
                int val=s[j][e]+s[j][C xor e]; //D xor e is D-e (D\e) if e is subset of D
                if(val<u){
                    u=val;
                }
            }
        }
        int value=pair_wise_dist[relabel[j]][relabel[q]]+u;
        if(value<result){
            result=value;
        }
    }
    return result;
}


//K is a subset of the nodes of the graph, called the terminals in the Steiner Tree problem. The set is
//represented as bit mask, e.g. the set {1,3} would be 0...0101, so the first and third bit are set.
int classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    int k=__builtin_popcount(K);
    int subset_count=(int) pow(2,k);
    unordered_map<set_t,int> W(subset_count);
    vector<unordered_map<set_t,int> > g(size,unordered_map<set_t,int>(subset_count));
    int weight = eval_W(pair_wise_dist, K, W, g, size);
    return weight;
}

bool cmp_setsize(set_t i, set_t j) { return (__builtin_popcount(i) < __builtin_popcount(j)); }


class Function_p
        : public Function<int> {

private:
    int level;
    vector<vector<int> > W;
    int p;
    int max_value;
public:
    Function_p(vector<vector<int> > &W_, int level_, int p_, int max_value_) : //TODO level and max value unimprotant
            W(W_), level(level_), p(p_), max_value(max_value_) {
    };

    int operator()(set_t s)  override{ //das if und so kann ich mri vermutlich sparenw enn ich w am anfang apassend befülle
        // int x_size = __builtin_popcount(s);
        // if (x_size < level && x_size >= 1) {
        return W[p][s];
        // }
        // return max_value;
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


#if 0
class EmbeddIntoIntProduct : public Function<int> {
    Function<int> &f;
    int base;
public:
    EmbeddIntoIntProduct(Function<int> &func, int n) : f(
            func) { //because f is a reference it has to be in a initializer list
        this->base = pow(2, n) + 1;
    }

    int operator()(set_t s) {
        int value = f(s);
        if (value == INT_MAX) return INT_MAX; //reicht immer noch nicht ,hiermit wird bei convolution ja noch gferechent
        return (int) pow(base, value);
    }

};
#endif

//TODO change all constructors from type a=type() to type a();
//TODO test map instead of array
int mobius_dreyfuss(weight_matrix &graph_adj, int n, set_t K, int input_range) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, n);
    int k = __builtin_popcount(K);
    vector<int> indices = get_element_indices(K);

    if (k == 2) { //just shortest path
        return pair_wise_dist[indices[0] - 1][indices[1] - 1];
    }
    int max_value = (n - 1) * input_range + 1;
    vector<vector<int> > W(n, vector<int>((int) pow(2, k), max_value)); //,(n-1)*input_range+1) in the second brackes
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
//    int max_value = (n - 1) * input_range + 1;
    for (int l = 2; l < k; ++l) {
        for (int p = 0; p < n; ++p) {
            Function_p f_p(W, l, p, max_value);
            Function_Embedd f(f_p);
            if(l>10){ //TODO fine tune
                g[p] = advanced_convolute<MinSumRingEmbedd>(f,k); //only convolute for Xs generated below
            }
            else{
                g[p] = naive_convolute<MinSumRingEmbedd>(f,f,k);
            }
        }
        vector<set_t> Xs = generate_subsets_of_size_k(relabeld_K, l,
                                                      k); //can skip this for l=k-1 and only do for one set as done in the comments below at compute result
        for (unsigned int q = 0; q < k; ++q) { //todo think about if you could calculate less here
            for (set_t X:Xs) {
                if ((X bitand (1 << q)) == 0) { //for all X with q not in X
                    int min_value = INT_MAX;
                    for (int p = 0; p < n; ++p) {
                       int value = pair_wise_dist[relabel[q]][relabel[p]] + g[p][X].min();
                       //int value = W[p][1<<q]+g[p][X].min();
                        if (value < min_value) {
                            min_value = value;
                        }
                    }

                    W[q][X] = min_value;
                }
            }
        }

    }


   /* for (int test = 0; test < k; test++) {
        cout << " Result: " << W[test][relabeld_K xor (1 << test)]; //should be the same everywhere
    }
    cout << endl;*/ //TODO think about this below
    int result=INT_MAX;
    for(int q=0;q<k;++q){
        int value=W[q][relabeld_K xor (1<<q)];
        if(value<result){
            result=value;
        }
    }
    //int result = W[1][relabeld_K xor 2];
    //output_tree(0,relabeld_K xor (1),n,W,g,relabel);

    return result;
}

#if 0
void output_tree(int q, set_t relabeld_K, int n, vector<vector<int> > &W, vector<vector<MinSumRingEmbedd> > &g,
                 int relabeling[]) {
    if (__builtin_popcount(relabeld_K) == 1) {
        cout << relabeling[__builtin_ffs(relabeld_K) - 1];
        return;
    }
    if (relabeld_K == 0) {
        return;
    }
    set_t X = relabeld_K;
    if (X == 0) {
        return;
    }
    int min = INT_MAX;
    int corresponding_p = n + 1;
    for (int p = 0; p < n; ++p) {
        int value = W[p][1 << q] + g[p][X].min();
        if (value < min) {
            min = value;
            corresponding_p = p;
        }
    }
    cout << "path(" << relabeling[q] << "," << relabeling[corresponding_p] << ") " << endl;

    vector<set_t> sets = get_subsets_it(X);
    if (sets.size() == 2) {
        cout << "p:" << relabeling[__builtin_ffs(X) - 1];
        return;
    }
    set_t min_set = 0;
    min = INT_MAX;
    for (set_t s:sets) {
        if ((s != 0) and (s != X)) {
            int value = W[corresponding_p][s] + W[corresponding_p][X xor s];
            if (value < min) {
                min = value;
                min_set = s;
            }
        }
    }
    set_t D = min_set;
    set_t X_wo_D = X xor D;
    vector<int> d = get_element_indices(D);
    cout << "H2:";
    for (int el:d) {
        cout << relabeling[el - 1] << " ";
    }
    cout << endl;
    cout << "H2 connects:" << relabeling[corresponding_p] << " ";
    //cout<<endl;
    output_tree(corresponding_p, D, n, W, g, relabeling);
    d = get_element_indices(X_wo_D);
    cout << "H3:";
    for (int el:d) {
        cout << relabeling[el - 1] << " ";
    }
    cout << "H3 connects:" << relabeling[corresponding_p] << " ";
    //cout<<endl;
    output_tree(corresponding_p, X_wo_D, n, W, g, relabeling);

}
#endif

#if 0
int mobius_dreyfuss(weight_matrix &graph_adj, int size, set_t K, int input_range) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    int k = __builtin_popcount(K);
    int subset_count = (int) pow(2, k);

    //TODO: ALle subsets bis größe x von allen Knoten


    vector<set_t> subsets = get_subsets_it(K); //das sind auch nicht alle subsets die wir brauchen, es fehlt für jedes
    //X, Xu{p} mit p element von V\K


    std::sort(subsets.begin(), subsets.end(), cmp_setsize); //TODO: generate subsets in order->bankers code
    // subsets of size 0=1, subsets of size 1=n, subsets of size 2=n*(n-1)/2, size 3=n*(n-1)*(n-2)/6 so (n over size)
    //intd2_arr g(boost::extents[size][subset_count]);

    vector<vector<int> > g(size,vector<int>(pow(2,size)));

    //@TODO relabeling
    vector<int> indices = get_element_indices(K);



    //init W
    vector<int> W((int)pow(2,size));
    for (set_t i = 0; i < k + 1; ++i) { //subsets of size 0 and 1
        W[i] = 0;
        cout<<subsets[i]<<" ";
    }
    cout<<endl;

    for (int i = k + 1; i < k + 1 + (k * (k - 1) / 2); ++i) { //subsets of size 2
        set_t set_repr = subsets[i];
        cout<<subsets[i]<<" ";
        int ele1 = __builtin_ffs(i) - 1;
        set_repr = set_repr xor (1 << ele1); //remove first element
        int ele2 = __builtin_ffs(set_repr) - 1;
        cout<<set_repr<<endl;
        W[subsets[i]] = pair_wise_dist[ele1][ele2]; //TODO: change when relabeling is done
    }

//#if 0
    int last_index = k + 1 + (k * (k - 1) /
                                 2); //determines the end index of subsets of size x, here set to the last index of subsets of size 2
    vector<vector<int> > g_transformed;
    for (int l = 2; l < k; ++l) {

        //compute gp for |X|<l
        for (int p = 0; p < size; ++p) {
            Function_p fbase = Function_p(W, l, p);
            EmbeddIntoIntProduct f=EmbeddIntoIntProduct(f,size);
            //should add a version which expects size ob subset and a vector containing all previous results
            g[p] = advanced_convolute<int>(f, f, size);
            /*TODO: super inefficient, probably need a convolute which only calculates for subsets of size |x|
              TODO: this also goes back to implementing bankers code*/
            //transform g[p] back
            int base = (int) pow(2, size) + 1;
            for (int i = input_range * 2; i >= 0; --i) {
                int coeff = pow(base, i); //careful that this term does not exceed int
                for (int j = 0; j < g[p].size(); ++j) {
                    if(g[p][j]==INT_MAX){

                    }
                    if (g[p][j] > coeff) {
                        g_transformed[p][j] = i;//at the end the minimal i will be in here
                        g[p][j] = g[p][j] % coeff; //TODO: funktioniert das mit max
                    }
                }
            }
        }
        //compute W with set=|X|=l U {q} :\q in V

        unsigned int new_index = last_index + nChoosek(k, l);
        for (unsigned int set_ind = last_index; set_ind < new_index; ++set_ind) { //go trhough all subsets of size l
            for (int q = 0; q < size; ++q)  //go troguh all v
            {
                if(q bitand subsets[set_ind]==0){ //consider only q not in X
                    set_t new_set=subsets[set_ind] bitor q;
                    int min=INT_MAX;
                    int value;
                    for(int p=0;p<size;++q) {
                       value=pair_wise_dist[q][p]+g[p][subsets[set_ind]];
                       if(min>value){
                           value=min;
                       }
                    }
                    W[new_set]=min;
                }
            }

        }
        last_index = new_index;

    }

    return W[K];
//#endif
    return 0;
}
#endif

void test_steiner() {

    //GRAPH 1

    /*  a --  1 --  b -- 1 --   d
     *     --2      |2
     *         --   c
     */
    std::cout << "------------------" << endl;
    std::cout << "Test Steiner: ";
    std::cout << "Graph 1: \n";
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

    int resultc = 0;
    int resultm =1;
    cout << "Testing {b,d,a}, expected result:2\n";
    resultc = classic_dreyfuss_wagner2(graph, 4, 0b1011);
    resultm = mobius_dreyfuss(graph, 4, 0b1011, 3);
    assert(resultm==2);
    assert(resultc==resultm);
    cout<<"Classic: " <<resultc<<endl;
    cout << "ADVANCED RESULT:" << resultm << endl;


    /*  a --  5 --  b -- 1 --   d
    *     --2      |2
    *         --   c-----1------e
    *                -1-    -3-
    *                    f
    */
    std::cout << "Test Steiner: ";
    std::cout << "Graph 2: \n";
    weight_matrix graph2(boost::extents[6][6]);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            graph2[i][j] = -1;
            if (i == j) {
                graph2[i][i] = 0;
            }
        }
    }
    graph2[0][1] = 5;
    graph2[1][0] = 5;
    graph2[0][2] = 2;
    graph2[2][0] = 2;
    graph2[1][2] = 2;
    graph2[2][1] = 2;
    graph2[1][3] = 1;
    graph2[3][1] = 1;
    graph2[2][4] = 1;
    graph2[4][2] = 1;
    graph2[2][5] = 1;
    graph2[5][2] = 1;
    graph2[4][5] = 3;
    graph2[5][4] = 3;


    cout << "Test with: {d,e,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b111000);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111000, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==5);
    assert(resultc==resultm);

    cout << "Test with: {a,b,c,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b100111);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b100111, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==5);
    assert(resultc==resultm);

    cout << "Test with: {a,b,d,f},Expected Value: 6\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b101011);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b101011, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==6);
    assert(resultc==resultm);

    cout << "Test with: {a,b},Expected Value: 4\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b000011);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b000011, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==4);
    assert(resultc==resultm);

    cout << "Test with: {b,c,d,e,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b111110);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111110, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==5);
    assert(resultc==resultm);


    cout << "Test with: {a,b,c,d,e,f},Expected Value: 7\n";
    resultc = classic_dreyfuss_wagner2(graph2, 6, 0b111111);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111111, 5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm==7);
    assert(resultc==resultm);
}


void test_ring_embedd() {
    MinSumRingEmbedd a = MinSumRingEmbedd(0) + MinSumRingEmbedd(3) - (MinSumRingEmbedd(1));
    MinSumRingEmbedd b = MinSumRingEmbedd(1) + MinSumRingEmbedd(0) - MinSumRingEmbedd(3);
    MinSumRingEmbedd c = MinSumRingEmbedd(3) + MinSumRingEmbedd(3) - MinSumRingEmbedd(4);

    MinSumRingEmbedd dist1 = a * (b + c);
    MinSumRingEmbedd dist2 = a * b + a * c;


    cout << "-------------" << endl;
    cout << "Test: Ring Embedding \n";
    cout << "Example 1\n";
    //check equal
    if (dist1.mset == dist2.mset) {
        cout << "a*(b+c)=ab+ac : Valid\n";
    } else {
        cout << "ERROR:a*(b+c)!=ab+ac\n";
    }
    dist1 = (a + b) * c;
    dist2 = a * c + b * c;

    //check equal
    if (dist1.mset == dist2.mset) {
        cout << "(a+b)*c=ac+bc : Valid\n";
    } else {
        cout << "ERROR:(a+b)*c!=ac+bc\n";
    }

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
    std::cout << "0 1 2 2\n1 0 2 1\n2 2 0 3\n2 1 3 0\n";

}
