//
// Created by dominik on 7/4/19.
//

#include "Path_Embedding.h"
#include "Tree.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "utility.h"
using namespace std;
void Path_Embedding::embedd(Tree& tree, std::vector<int> path_lengths) {
    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number

}

vector<vector <Tree> > get_nodes_by_depth(Tree& tree){
    vector<vector<Tree> > tree_nodes; //saves trees by depth
    vector<Tree> current_level;
    vector<Tree> next_level;
    next_level.push_back(tree);
    int depth=0;
    while(! next_level.empty()){
        tree_nodes.push_back(vector<Tree>());
        swap(current_level,next_level);
        //next_level.clear();
        for(Tree t:current_level){
           tree_nodes[depth].push_back(t);
           next_level.push_back(*t.left);
           next_level.push_back(*t.right);
        }
        depth++;
        current_level.clear();
    }
    return tree_nodes;
}

//maps each node to the canonical tree number
unordered_map<int,int> get_value_id_map(vector<vector<Tree> > by_depth){
    unordered_map<int,int>result;
    for(int depth=by_depth.size();depth>=0;--depth){
        for(Tree t:by_depth[depth]){
            if(!t.left && !t.right){ //in case of leaf
                result[t.value]=1;
            }
            else{
                result[t.value]=0;
                if(t.left){ //the child exists
                    result[t.value]+=result[t.left->value]*2;
                }
                if(t.right){ //the child exists
                    result[t.value]+=(result[t.right->value])*2;
                }
            }
        }
    }
    return result;
}

//Here it expects the table for each v
void one_child_propagate(int* table_in,vector<int> &path_lengths,int* table_out){
    int path_count=path_lengths.size();
    int table_size=1<<path_count;
    for(int path_set=0;path_set<table_size;++path_set){
        if(table_in[path_set]>=0){ //L(v,P)>=0
            table_out[path_set]=table_in[path_set]+1;
        }
        else{ //L(v,P)=-inf
            table_out[path_set]=-1; //TODO new higher minus
            for(int path=0;path<path_count;++path){
                if((1<<path)&path_set and table_in[path_set xor (1<<path)]>=path_lengths[path]-1) //das vll sogar precomputen (1<<path) für alle path, table size ist sehr groß
                {
                    table_out[path_set]=0;
                    continue;
                }
             }

        }
    }
}

//Here it expects the table for each v
void two_child_propagate_direct(int* table_l,int* table_r,vector<int> &path_lengths,int* table_out){
    int path_count=path_lengths.size();
    int table_size=1<<path_count;
    int v1[table_size];
    int v2[table_size];

    //Create trees v1 and v2
    one_child_propagate(table_l,path_lengths,v1);
    one_child_propagate(table_r,path_lengths,v1);

    //merge the tables in the "naive"/direct way
    //checking every subset S of P if L(v_1,S),L(v_2,P\S)>=0 and max of these=r
    for(set_t S=0;S<table_size;++S){
        vector<set_t> subsets=get_subsets_it(S);
        for(set_t P_1:subsets){
            if(v1[S]>=0 and v2[S xor P_1]>=0){
                if(max(v1[S],v2[S])>0){
                    table_out[S]=max(table_out[S],max(v1[S],v2[S])); //TODO initizalize table out to -1
                }

            }
        }
    }
    //check the remaining 0 and impossible ones if it can be packed by using v-u1 and v-u2
    for(set_t P=0;P<table_size;++P){
        if(table_out[P]<=0){ //todo think about removing the if in the r check above and just set to -1
            for(int i=0;i<path_count;++i){
                if((1<<i) & P){
                    set_t P_without_i=P xor (1<<i);
                    vector<set_t> subsets=get_subsets_it(P);
                    for(set_t P_1:subsets){
                        if(v1[P_1]+v2[P xor P_1]>=path_lengths[i]){ //either one could be -1, so maybe replace -1 by higher -value , i.e. tree depth
                            table_out[P]=0;
                            break;
                        }
                    }
                }
            }
        }

    }

}


void Path_Embedding::embedd_naive(Tree& tree, std::vector<int> path_lengths) {
    // go thhrough the tree from the bottom to the top
    //remember all results coded with associated tree number





}


void Path_Embedding::generateFullTrees(int depth,Tree& root) {
    if(depth<=1){
        return;
    }
    Tree left(root.value*2+1); //0->1->3
    root.add_left(&left);      //>   >4
    Tree right(root.value*2+2);// 2 > 5
    root.add_right(&right);      //  >6
    generateFullTrees(depth-1,left);
    generateFullTrees(depth-1,right);
}

