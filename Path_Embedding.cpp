//
// Created by dominik on 7/4/19.
//

#include "Path_Embedding.h"
#include "Tree.h"
void Path_Embedding::embedd() {



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

