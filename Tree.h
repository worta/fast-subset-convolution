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
    int value;

    Tree(int v):value(v){
        value=v;
        right=0;
        left=0;
        father=0;
        childCount=0;
    }
    void AddLeft(Tree *l){
        left=l;
        childCount++;
        l->father=this;
    }
    void AddRight(Tree *r){
        right=r;
        childCount++;
        r->father=this;
    }
};


#endif //FAST_SUBSET_CONVOLUTION_TREE_H
