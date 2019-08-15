//
// Created by dominik on 7/4/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_TREE_H
#define FAST_SUBSET_CONVOLUTION_TREE_H

#include <memory>
class Tree {
public:
    std::shared_ptr<Tree> right;
    std::shared_ptr<Tree> left;
    int childCount;
    int id;

    Tree(int v):id(v){
        id=v;
        right=nullptr;
        left=nullptr;
        childCount=0;
    }
    void add_left(std::shared_ptr<Tree> l){
        left=l;
        childCount++;
    }
    void add_right(std::shared_ptr<Tree> r){
        right=r;
        childCount++;
    }
};


#endif //FAST_SUBSET_CONVOLUTION_TREE_H
