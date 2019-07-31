//
// Created by dominik on 7/4/19.
//

#include "Path_Embedding.h"
#include "Tree.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
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

//maps each ndoe to the canonical tree number
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

