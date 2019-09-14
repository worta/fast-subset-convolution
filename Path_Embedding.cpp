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
void one_child_propagate(int *table_in, vector<int> &path_lengths, int *table_out) {
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    for (int path_set = 0; path_set < table_size; ++path_set) {
        if (table_in[path_set] >= 0) { //L(v,P)>=0
            table_out[path_set] = table_in[path_set] + 1;
        } else { //L(v,P)=-inf
            table_out[path_set] = -1000; //TODO new higher minus
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

void two_children_propagate_mobius(int *table_l, int *table_r, vector<int> &path_lengths,int depth ,int *table_out){
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    int v1[table_size];
    int v2[table_size];

    //Create tables v1 and v2
    one_child_propagate(table_l, path_lengths, v1); //TODO das einfach schon vorher machen, in dem aufruf und davor überprüfen ob das schonmal gemacht wurxe
    one_child_propagate(table_r, path_lengths, v2);

    //merge two children mobius like
    ThresholdFunction<int> f_1(v1,0);
    ThresholdFunction<int> f_2(v2,0);

    for(int i=0;i<table_size;++i){
        table_out[i]=-100;
    }

    FastSubsetConvolution<int> conv(path_count); //Todo an zentraler stelle initialisieren
    //neben depth könnte auch noch die summe der untergebrachten pfadlängen - der verfügbaren edges interessant sein
    int* temp1=new int[table_size];
    int* temp2=new int[table_size];
    for(int i=0;i<=depth;++i){
        ThresholdFunction<int> g_1(v1,i);
        ThresholdFunction<int> g_2(v2,i);

        conv.advanced_convolute(f_1,g_2,temp1);
        conv.advanced_convolute(f_1,g_2,temp2);

        for(set_t j=0;j<table_size;++j){
            if(temp1[j]+temp2[j]>0){
                table_out[j]=i;
            }
        }
    }
    /*for(int i=0;i<table_size;++i){
        if(table_out[i]<0){ //Todo maybe <=0?
            //TODO add subset convolution with subsets

        }
    }*/
    //second case nto yet done with convolution but naively
    for (set_t P = 0; P < table_size; ++P) {
        if (table_out[P] < 0) {
            for (int i = 0; i < path_count; ++i) {
                if ((1 << i) & P) {
                    set_t P_without_i = P xor(1 << i);
                    vector<set_t> subsets = get_subsets_it(P);
                    for (set_t P_1:subsets) {
                        if (v1[P_1] + v2[P xor P_1] >=
                            path_lengths[i]) { //either one could be -1, so maybe replace -1 by higher -value , i.e. tree depth
                            table_out[P] = 0;
                            break;
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
void two_child_propagate_direct(int *table_l, int *table_r, vector<int> &path_lengths, int *table_out) {
    int path_count = path_lengths.size();
    int table_size = 1 << path_count;
    int v1[table_size];
    int v2[table_size];


    for (int i = 0; i < table_size; ++i) {
        table_out[i] = -1000;//-path_count; //TODO einfach richtiges minus unendlich ,i.e. ein sehr negativer wert
    }

    //Create tables v1 and v2
    one_child_propagate(table_l, path_lengths, v1); //TODO das einfach schon vorher machen, in dem aufruf und davor überprüfen ob das schonmal gemacht wurxe
    one_child_propagate(table_r, path_lengths, v2);


    //merge the tables in the "naive"/direct way
    //checking every subset S of P if L(v_1,P),L(v_2,S\P)>=0 and max of these=r
    for (set_t S = 0; S < table_size; ++S) {
        vector<set_t> subsets = get_subsets_it(S);
        for (set_t P_1:subsets) {
            if (v1[P_1] >= 0 and v2[S xor P_1] >= 0) {
                table_out[S] = max(table_out[S], max(v1[P_1], v2[S xor P_1]));
            }
        }
    }
    //check the remaining 0 and impossible ones if it can be packed by using v-u1 and v-u2
    for (set_t P = 0; P < table_size; ++P) {
        if (table_out[P] < 0) { //changed to from <=0 to <0, is that correct?
            for (int i = 0; i < path_count; ++i) {
                if ((1 << i) & P) {
                    set_t P_without_i = P xor(1 << i);
                    vector<set_t> subsets = get_subsets_it(P);
                    for (set_t P_1:subsets) {
                        if (v1[P_1] + v2[P xor P_1] >=
                            path_lengths[i]) { //either one could be -1, so maybe replace -1 by higher -value , i.e. tree depth
                            table_out[P] = 0;
                            break;
                        }
                    }
                }
            }
        }

    }

}

void test_child_propagation() {
    int in_table[] = {0, -1};
    int *out_table = new int[2];
    vector<int> path = {1};
    int in_table2a[] = {0, -3, -3, -3}; //p1=4,p2=2, just a leaf
    int in_table2b[] = {2, 0, 2, -3};//p1=4,p2=2, full tree with depth 2
    vector<int> path2 = {4,2};
    one_child_propagate(in_table, path, out_table);
    in_table[0] = 0;
    in_table[1] = -1;
    two_child_propagate_direct(in_table, in_table, path, out_table);
    two_child_propagate_direct(in_table2a, in_table2b, path2, out_table); //result should be {3,1,3,0} and it is


}


int Path_Embedding::embedd_naive(Tree &tree, std::vector<int> &path_lengths) {
    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number
   // test_child_propagation();
    int table_size = 1 << path_lengths.size();
    unordered_map<int, int *> tree_table_map;
    vector<vector<Tree> > nodes_by_depth = get_nodes_by_depth(tree);
    unordered_map<int, int> id_to_tree_number = get_value_id_map(nodes_by_depth);
    int depth = nodes_by_depth.size();
    //initialize for leaf, i.e. tree number 1
    int *leaf_table = new int[table_size];
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
                int *v = new int[table_size];
                if (child_count == 1) {
                    shared_ptr<Tree> child = node.left;
                    if (child == 0) {
                        child = node.right;
                    }
                    int child_tree = id_to_tree_number[child->id];
                    int *child_table = tree_table_map[child_tree];
                    one_child_propagate(child_table, path_lengths, v);
                    tree_table_map[id_to_tree_number[node.id]] = v;
                } else { //child_count==2
                    int *child_table1 = tree_table_map[id_to_tree_number[node.left->id]];
                    int *child_table2 = tree_table_map[id_to_tree_number[node.right->id]];
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
    unordered_map<int, int *> tree_table_map;
    vector<vector<Tree> > nodes_by_depth = get_nodes_by_depth(tree);
    unordered_map<int, int> id_to_tree_number = get_value_id_map(nodes_by_depth);
    int depth = nodes_by_depth.size();
    //initialize for leaf, i.e. tree number 1
    int *leaf_table = new int[table_size];
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
                int *v = new int[table_size];
                if (child_count == 1) {
                    shared_ptr<Tree> child = node.left;
                    if (child == 0) {
                        child = node.right;
                    }
                    int child_tree = id_to_tree_number[child->id];
                    int *child_table = tree_table_map[child_tree];
                    one_child_propagate(child_table, path_lengths, v);
                    tree_table_map[id_to_tree_number[node.id]] = v;
                } else { //child_count==2
                    int *child_table1 = tree_table_map[id_to_tree_number[node.left->id]];
                    int *child_table2 = tree_table_map[id_to_tree_number[node.right->id]];
                    two_children_propagate_mobius(child_table1, child_table2, path_lengths,depth-level,v);
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
