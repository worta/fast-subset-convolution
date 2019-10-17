//
// Created by dominik on 7/4/19.
//

#include "Path_Embedding.h"
#include "Tree.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "utility.h"
#include <memory>
#include "functions.h"
#include "FastSubsetConvolution.h"
using namespace std;


vector<vector<Tree> > get_nodes_by_depth(Tree &tree) {
    vector<vector<Tree> > tree_nodes; //saves trees by depth
    vector<Tree> current_level;
    vector<Tree> next_level;
    next_level.push_back(tree);
    int depth = 0;
    while (!next_level.empty()) {
        tree_nodes.push_back(vector<Tree>());
        swap(current_level, next_level);
        //next_level.clear();
        for (Tree t:current_level) {
            tree_nodes[depth].push_back(t);
            if (t.left) next_level.push_back(*t.left);
            if (t.right) next_level.push_back(*t.right);
        }
        depth++;
        current_level.clear();
    }
    return tree_nodes;
}

//maps each node to the canonical tree number
unordered_map<int, int> get_value_id_map(vector<vector<Tree> > &by_depth) {
    unordered_map<int, int> result;
    for (int depth = by_depth.size() - 1; depth >= 0; --depth) {
        for (Tree t:by_depth[depth]) {
            if (!t.left && !t.right) { //in case of leaf
                result[t.id] = 1;
            } else {
                result[t.id] = 0;
                if (t.left) { //the child exists
                    result[t.id] += result[t.left->id] * 2;
                }
                if (t.right) { //the child exists
                    result[t.id] += (result[t.right->id]) * 2;
                }
            }
        }
    }
    return result;
}

//Here it expects the table for each v
void Path_Embedding::one_child_propagate(int8_t  *table_in, vector<int> &path_lengths, int8_t *table_out) {
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    for (int path_set = 0; path_set < table_size; ++path_set) {
        if (table_in[path_set] >= 0) { //L(v,P)>=0
            table_out[path_set] = table_in[path_set] + 1;
        } else { //L(v,P)=-inf
            table_out[path_set] = -100; //TODO new higher minus
            for (int path = 0; path < path_count; ++path) {
                if ((1 << path) & path_set and table_in[path_set xor (1 << path)] >= path_lengths[path] -
                                                                                     1) //das vll sogar precomputen (1<<path) für alle path, table size ist sehr groß
                {
                    table_out[path_set] = 0;
                    continue;
                }
            }

        }
    }
}

void Path_Embedding::two_children_propagate_mobius(FastSubsetConvolution<int8_t> &conv,int8_t *table_l,int8_t  *table_r, vector<int> &path_lengths,int depth ,int8_t  *table_out){
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    int8_t  v1[table_size];
    int8_t v2[table_size];

    //Create tables v1 and v2
    one_child_propagate(table_l, path_lengths, v1);
    one_child_propagate(table_r, path_lengths, v2); //todo remvoe here

    //merge two children mobius like
    ThresholdFunction<int8_t> f_1(v1,0);
    ThresholdFunction<int8_t> f_2(v2,0);

    for(int i=0;i<table_size;++i){
        table_out[i]=-100;
    }

    int8_t * temp1=new int8_t [table_size];
    int8_t * temp2=new int8_t [table_size];
    vector<set_t > check_again;
    check_again.reserve(table_size/5);// guess
    for(int i=0;i<=depth;++i){

        ThresholdFunction<int8_t> g_1(v1,i);
        ThresholdFunction<int8_t> g_2(v2,i);

        //conv.advanced_convolute(f_1,g_2,temp1);
        //conv.advanced_convolute(f_2,g_1,temp2);
        conv.advanced_covering_product(f_1,g_2,temp1);
        conv.advanced_covering_product(f_2,g_1,temp2);

        if(i<depth){
            for(set_t j=0;j<table_size;++j){
                if(temp1[j]+temp2[j]>0){
                    table_out[j]=i;
                }
            }
        }
        else{
            for(set_t j=0;j<table_size;++j){
                if(temp1[j]+temp2[j]>0){
                    table_out[j]=i;
                }
                if(table_out[j]<0){
                    check_again.push_back(j);
                }
            }
        }
    }
    int path_min=100;
    int path_max=-1;
    for(int path:path_lengths){
        if(path<path_min){
            path_min=path;
        }
        if(path>path_max){
            path_max=path;
        }
    }
    for(int i=1;i<path_max;++i){
        for(int j=std::max(path_min-i,i+1);j<path_max;++j){
            ThresholdFunction<int8_t> f_i(v1,i);
            ThresholdFunction<int8_t> g_j(v2,j);
            ThresholdFunction<int8_t> f_j(v1,j);
            ThresholdFunction<int8_t> g_i(v2,i);
            //conv.advanced_convolute(f_i,g_j,temp1);
            //conv.advanced_convolute(f_j,g_i,temp2); //nees also a bigger buffer in benchmark ath embedding
            conv.advanced_covering_product(f_i,g_j,temp1);
            conv.advanced_covering_product(f_j,g_i,temp2);
            for(set_t maybe_need_both_edges:check_again){
                for(int path_nr=0;path_nr<path_lengths.size();++path_nr){
                    if(i+j>=path_lengths[path_nr]){
                        if((temp1[maybe_need_both_edges xor (1<<path_nr)]>=1) or (temp2[maybe_need_both_edges xor (1<<path_nr)]>=1) ){//xor is fine, worst case the set gets bigger{
                            table_out[maybe_need_both_edges]=0; //could tryx to remove that afterwards, but might be expensive
                        }
                    }
                }
            }

        }
    }


    delete[] temp1;
    delete[] temp2;
}


//Here it expects the table for each v
void Path_Embedding::two_child_propagate_direct(int8_t  *table_l, int8_t  *table_r, vector<int> &path_lengths, int8_t  *table_out) {
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    int8_t*  v1=new int8_t[table_size];
    int8_t*  v2=new int8_t[table_size];


    for (int i = 0; i < table_size; ++i) {
        table_out[i] = -100;//-path_count; //TODO einfach richtiges minus unendlich ,i.e. ein sehr negativer wert
    }

    //Create tables v1 and v2
    one_child_propagate(table_l, path_lengths, v1); //TODO das einfach schon vorher machen, in dem aufruf und davor überprüfen ob das schonmal gemacht wurxe
    one_child_propagate(table_r, path_lengths, v2);


    //merge the tables in the "naive"/direct way
    //checking every subset S of P if L(v_1,P),L(v_2,S\P)>=0 and max of these=r
    for (set_t S = 0; S < table_size; ++S) {
        vector<set_t> subsets = get_subsets(S);
        for (set_t P_1:subsets) {
            if (v1[P_1] >= 0 and v2[S xor P_1] >= 0) {
                table_out[S] = max(table_out[S], max(v1[P_1], v2[S xor P_1]));
            }
        }
    }
    //check the remaining impossible ones if it can be packed by using v-u1 and v-u2
    for (set_t P = 0; P < table_size; ++P) {
        if (table_out[P] < 0) {
            for (int i = 0; i < path_count; ++i) {
                if ((1 << i) & P) {
                    set_t P_without_i = P xor(1 << i);
                    vector<set_t> subsets = get_subsets(P_without_i);
                    for (set_t P_1:subsets) {
                        if (v1[P_1] + v2[P_without_i xor P_1] >=
                                path_lengths[i]) {
                            table_out[P] = 0;
                            break;
                        }
                    }
                }
            }
        }

    }
    delete[] v1;
    delete[] v2;
}



int Path_Embedding::embedd_naive(Tree &tree, std::vector<int> &path_lengths) {
    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number
   // test_child_propagation();
    int table_size = 1 << path_lengths.size();
    unordered_map<int, int8_t  *> tree_table_map;
    vector<vector<Tree> > nodes_by_depth = get_nodes_by_depth(tree);
    unordered_map<int, int> id_to_tree_number = get_value_id_map(nodes_by_depth);
    int depth = nodes_by_depth.size();
    //initialize for leaf, i.e. tree number 1
    int8_t  *leaf_table = new int8_t[table_size];
    leaf_table[0] = 0; //for the empty set 0
    for (set_t P = 1; P < table_size; ++P) {
        leaf_table[P] = -depth; //equivalent to -inf
    }
    tree_table_map[1] = leaf_table;

    //from bottom to top
    for (int level = depth - 1; level >= 0; --level) { //TODO can start from depth-1, in depth only leaves
        for (Tree node:nodes_by_depth[level]) {
            int tree_number = id_to_tree_number[node.id];
            if (tree_table_map.find(tree_number) == tree_table_map.end()) {
                //create correspondiong one
                //count childs
                int child_count = (node.left != 0) + (node.right != 0);
                // childs should be 1 or 2, 0 (i.e. a leaf) is handled above
                int8_t  *v = new int8_t[table_size];
                if (child_count == 1) {
                    shared_ptr<Tree> child = node.left;
                    if (child == 0) {
                        child = node.right;
                    }
                    int child_tree = id_to_tree_number[child->id];
                    int8_t  *child_table = tree_table_map[child_tree];
                    one_child_propagate(child_table, path_lengths, v);
                    tree_table_map[id_to_tree_number[node.id]] = v;
                } else { //child_count==2
                    int8_t  *child_table1 = tree_table_map[id_to_tree_number[node.left->id]];
                    int8_t  *child_table2 = tree_table_map[id_to_tree_number[node.right->id]];
                    two_child_propagate_direct(child_table1, child_table2, path_lengths, v);
                }

                tree_table_map[tree_number] = v;
                if (v[table_size - 1] >= 0) {
                    for (auto key_value:tree_table_map) {
                        delete[] key_value.second;
                    }

                    return 1;
                }
            }
            // otherwise done
        }
    }

    for (auto key_value:tree_table_map) {
        delete[] key_value.second;
    }
    return 0;
}


//Just the root has depth=0,
void Path_Embedding::generateFullTrees(int depth, Tree &root) {
    if (depth <= 0) {
        return;
    }
    shared_ptr<Tree> left(new Tree(root.id * 2 + 1)); //0->1->3
    root.add_left(left);      //>   >4
    shared_ptr<Tree> right(new Tree(root.id * 2 + 2));// 2 > 5
    root.add_right(right);      //  >6
    generateFullTrees(depth - 1, *left);
    generateFullTrees(depth - 1, *right);
}


int Path_Embedding::embedd_mobius(Tree &tree, std::vector<int> &path_lengths) {
    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number

    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number
    // test_child_propagation();
    int table_size = 1 << path_lengths.size();
    unordered_map<int, int8_t  *> tree_table_map;
    vector<vector<Tree> > nodes_by_depth = get_nodes_by_depth(tree);
    unordered_map<int, int> id_to_tree_number = get_value_id_map(nodes_by_depth);
    int depth = nodes_by_depth.size();
    //initialize for leaf, i.e. tree number 1
    int8_t  *leaf_table = new int8_t [table_size];
    leaf_table[0] = 0; //for the empty set 0
    for (set_t P = 1; P < table_size; ++P) {
        leaf_table[P] = -depth; //equivalent to -inf
    }

    tree_table_map[1] = leaf_table;


    FastSubsetConvolution<int8_t> conv(path_lengths.size()); //Todo an zentraler stelle initialisieren
    //from bottom to top
    for (int level = depth - 1; level >= 0; --level) { //TODO can start from depth-1, in depth only leaves
        for (Tree node:nodes_by_depth[level]) {
            int tree_number = id_to_tree_number[node.id];
            if (tree_table_map.find(tree_number) == tree_table_map.end()) {
                //create correspondiong one
                //count childs
                int child_count = (node.left != 0) + (node.right != 0);
                // childs should be 1 or 2, 0 (i.e. a leaf) is handled above
                int8_t  *v = new int8_t [table_size];
                if (child_count == 1) {
                    shared_ptr<Tree> child = node.left;
                    if (child == 0) {
                        child = node.right;
                    }
                    int child_tree = id_to_tree_number[child->id];
                    int8_t  *child_table = tree_table_map[child_tree];
                    one_child_propagate(child_table, path_lengths, v);
                    tree_table_map[id_to_tree_number[node.id]] = v;
                } else { //child_count==2
                    int8_t  *child_table1 = tree_table_map[id_to_tree_number[node.left->id]];
                    int8_t  *child_table2 = tree_table_map[id_to_tree_number[node.right->id]];
                    two_children_propagate_mobius(conv,child_table1, child_table2, path_lengths,depth-level,v);
                }

                tree_table_map[tree_number] = v;
                if (v[table_size - 1] >= 0) {
                    for (auto key_value:tree_table_map) {
                        delete[] key_value.second;
                    }

                    return 1;
                }
            }
            // otherwise done
        }
    }

    for (auto key_value:tree_table_map) {
        delete[] key_value.second;
    }
    return 0;


}
