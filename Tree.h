//
// Created by dominik on 7/4/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_TREE_H
#define FAST_SUBSET_CONVOLUTION_TREE_H


class Tree {
public:
    Tree* right;
    Tree* left;
    Tree* father;
    int childCount;
    int id;

    Tree(int v):id(v){
        id=v;
        right=0;
        left=0;
        father=0;
        childCount=0;
    }
    void add_left(Tree *l){
        left=l;
        childCount++;
        l->father=this;
    }
    void add_right(Tree *r){
        right=r;
        childCount++;
        r->father=this;
    }
};


#endif //FAST_SUBSET_CONVOLUTION_TREE_H
