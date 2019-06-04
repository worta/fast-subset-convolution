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

inline set_t to_byte_repr(vector<int> indices) {
    set_t set_repr = 0;
    for (int elem:indices) {
        set_repr += 1 << (elem - 1);
    }
    return set_repr;
}


int eval_g(weight_matrix &pair_wise_dist, vector<int> &W, int &nodes, set_t set_repr, int p, intd2_arr &g);

//Both eval W and eval g should keep track of a list of optimal choices
int eval_W(weight_matrix &pair_wise_dist, set_t set_repr, vector<int> &W, intd2_arr &g, int &nodes) {
    int ele_count = __builtin_popcount(set_repr);
    int value;
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
    set_repr = set_repr xor (1 << q); //remove q
    if (W[set_repr] >= 0) {
        return W[set_repr];
    } else {
        for (int i = 0; i < nodes; ++i) {
            value = pair_wise_dist[q - 1][i] + eval_g(pair_wise_dist, W, nodes, set_repr, i, g);
            if (value < min) {
                min = value;
            }
        }
    }
    W[set_repr] = min;
    return min;
}


int eval_g(weight_matrix &pair_wise_dist, vector<int> &W, int &nodes, set_t set_repr, int p,
           intd2_arr &g) { //TODO add g as buffer for results
    if (g[p][set_repr] >= 0) {
        return g[p][set_repr];
    }
    vector<set_t> sets = get_subsets_it(set_repr);
    int min = INT_MAX;
    int value;
    for (set_t set:sets) {
        if (set != 0 && set != set_repr) { //not empty and not all of X
            value = eval_W(pair_wise_dist, set or (1 << p), W, g, nodes) +
                    eval_W(pair_wise_dist, set_repr xor set, W, g, nodes);
            if (value < min) {
                min = value;
            }
        }
    }
    g[p][set_repr] = min;
    return min;
}


//K is a subset of the nodes of the graph, called the terminals in the Steiner Tree problem. The set is
//represented as bit mask, e.g. the set {1,3} would be 0...0101, so the first and third bit are set.
int classic_dreyfuss_wagner(weight_matrix &graph_adj, int size, set_t K) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, size);
    int subset_count = (int) pow(2, __builtin_popcount(K));
    vector<int> W(subset_count, -1);
    intd2_arr g(boost::extents[size][subset_count]);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < subset_count; ++j) {
            g[i][j] = -1;
        }
    }
    int weight = eval_W(pair_wise_dist, K, W, g, size);
    return weight;
}
//TODO: Es sollte noch die Indizes in einen Vector gepackt werden und das subset entsprechend auf minimale Darstellung
//TODO: reduziert werden, i.e. die nötigen Knoten werden gerelabeld

bool cmp_setsize(set_t i, set_t j) { return (__builtin_popcount(i) < __builtin_popcount(j)); }


class Function_p
        : public Function<int> { //TODO ADD MAP SO THAT RELEVANT SUBSET ARE ALWAYS THE LOWEST BITS IN SET REPR

private:
    int level;
    vector<vector<int> > W;
    set_t p;
    int max_value;
public:
    Function_p(vector<vector<int> > &W_, int level_, int p_, int max_value_) :
            W(W_), level(level_), p(p_), max_value(max_value_) {
    };

    int operator()(set_t s) { //das if und so kann ich mri vermutlich sparenw enn ich w am anfang apassend befülle
        int x_size = __builtin_popcount(s);
        if (x_size < level && x_size >= 1) {
            return W[p][s];
        }
        return max_value;
    }

};

class Function_Embedd:public Function<MinSumRingEmbedd>{
    Function_p &f_p;
public:
    Function_Embedd(Function_p &f):
    f_p(f)
    {
    }


    MinSumRingEmbedd operator()(set_t s){
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

int mobius_dreyfuss(weight_matrix &graph_adj, int n, set_t K, int input_range) {
    weight_matrix pair_wise_dist = compute_ap_shortest_path(graph_adj, n);
    //vector<vector <int> > W; //W[q][X] with q in V\K and X subset of K (maybe q=0 signals q in K)
    int k = __builtin_popcount(K);
    vector<int> indices = get_element_indices(K);
    int highest_index = 0;
    for (int index:indices) { //inefficient but does not matter
        if (index > highest_index) {
            highest_index = index;
        }
    }

    vector<vector <int> > W(n, vector<int>((int) pow(2, k),(n-1)*input_range+1));
    for(int i=0;i<W.size();++i)
    {
        for(int j=0;j<W[i].size();++j){
            cout<<"i:"<<i<<" j:"<<j<<" W:"<<W[i][j]<<" ";
        }
        cout<<endl;
    }
    vector<vector <MinSumRingEmbedd> > g(n, vector<MinSumRingEmbedd>((int) pow(2, k)));

    //SUBSET GENERATION
    //vector<set_t> subsets = get_subsets_it(K);
    //subsets.reserve(((int) pow(2, k)) * (n - k + 1));


    vector<set_t> V_without_K_singletons; //should contain all nodes in V/K represented as singleton sets, i.e. a
    // set_t with exactly 1 bit set

    //relabel
    int relabel[n];
    for(int i=0;i<indices.size();++i){
        relabel[i]=indices[i]-1;
    }
    int new_index=indices.size();
    for (int i = 0; i < n; ++i) {
        if (!(K bitand (1 << i))) {
            V_without_K_singletons.push_back(i << 1);
            relabel[new_index]=i;
            new_index++;
        }
    }
    //Subsets can now be treated as 000..011..1
    set_t relabeld_K=0;
    for(int i=0;i<indices.size();i++){
        relabeld_K=relabeld_K|(1<<i);
    }

//TODO change all constructors from type a=type() to type a();

#if 0
    /*int current_size=subsets.size();
    for(int i=0;i<current_size;++i){
        for(set_t singleton:V_without_K_singletons){
            subsets.push_back(subsets[i] bitor singleton);
        }
    }*/
    //subsets now contains all subsets of the form X u {q} with X subset of K and q element of V without K
    //std::sort(subsets.begin(),subsets.end(),cmp_setsize);
#endif
    //init for W for l=2
    for (int q = 0; q < n; q++) {
        for (int p = 0; p < k; p++) {
            W[q][1 << p] = pair_wise_dist[relabel[q]][relabel[p]];
        }
    }

    //levelwise computation
    int max_value = (n - 1) * input_range + 1;
    for (int l = 2; l < k; ++l) {
        for (int p = 0; p < n; ++p) {
            Function_p f_p(W, l, p, max_value);
            Function_Embedd f(f_p);
            //g[p]=advanced_convolute<MinSumRingEmbedd>(f, f, k);
            g[p]=naive_convolute<MinSumRingEmbedd>(f, f, k);
            cout<<"Level: "<<l<<" p:"<<p<<endl;
          /*  cout<<"---------------------------"<<endl;
            for(set_t z;z<g[p].size();++z){
                cout<<"set:";
                output_set(z,k);
                cout<<" "<<g[p][z].min();
            }
            cout<<"---------------------------"<<endl;*/
            //g[p][subset element of Xs]=convolute fp and fp
        }
        vector<set_t> Xs = generate_subsets_of_size_k(relabeld_K, l, k);
        for (int q = 0; q < n; ++q) {
            for (set_t X:Xs) {
                int min_value=INT_MAX;
                for(int p=0;p<n;++p){
                    int value= pair_wise_dist[relabel[q]][relabel[p]]+g[p][X].min();
                    if(value<min_value){
                        min_value=value;
                    }
                }
                W[q][X]=min_value;
            }
        }

    }
    cout<<"relabeld K:"<<relabeld_K<<endl;
    cout<<"k:"<<k<<endl;
    /*for(int q=0;q<n;++q){

        vector<set_t> Xs = generate_subsets_of_size_k(relabeld_K, k-1,k);
        for(set_t X:Xs){
            cout<<"q="<<relabel[q]<<" X:"<<X<<" value:"<<W[q][X]<<endl;
        }
    }*/


    for (set_t j = 0; j <k; ++j) {
        cout<<g[j][relabeld_K xor (1<<j)].min()<<endl;

    }

    //calculate best
    int result=INT_MAX;
    for(int i=0;i<n;++i){
        int v=pair_wise_dist[relabel[0]][relabel[i]]+g[i][relabeld_K-1].min();
        if(v<result){
            result=v;
        }

    }




    return result;
}


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
    /*  a --  1 --  b -- 1 --   d
     *     --2      |2
     *         --   c
     */
    std::cout << "Test Steiner: \n";
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
    int result = classic_dreyfuss_wagner(graph, 4, 0b1011);
    if (result != 2) {
        cout << "ERROR:Steiner: Should be 4 but is: " << result << endl;
    } else {
        cout << "Steiner:OK" << endl;
    }
    result = mobius_dreyfuss(graph, 4, 0b1011, 3);
    cout << "ADVANCED RESULT:" << result << endl;
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
