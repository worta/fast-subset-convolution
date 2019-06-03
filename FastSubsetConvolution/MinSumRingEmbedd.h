//
// Created by dominik on 6/3/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
#define FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
#include <math.h>
#include <algorithm>
#include <set>
class MinSumRingEmbedd {
    std::multiset<int> mset;
    MinSumRingEmbedd (int x):
    mset(std::multiset<int>())
    {
        mset.insert(x);
    }
    MinSumRingEmbedd& operator= (const int& x) //that is not well defined
    {
        mset.clear();
        if(x!=0){
            mset.insert(x); //if it is set to 0, that is the 0 element of the ring
        }
        return *this;
    }
    MinSumRingEmbedd& operator= (const MinSumRingEmbedd& x)
    {
        mset=x.mset; //copies container content
        return *this;
    }
    MinSumRingEmbedd(std::multiset<int> &m):
    mset(m)
    {
    }
    MinSumRingEmbedd operator*(const MinSumRingEmbedd& rhs)
    {
        std::multiset<int> newS=std::multiset<int>();
        for(int node_rhs:rhs.mset){
            for(int node_lhs:mset){
                if(node_rhs>=0 && node_lhs>=0){ //TODO cant add -0
                    newS.insert(node_lhs+node_rhs);
                }
            }
        }
        return MinSumRingEmbedd(newS);
    }
    MinSumRingEmbedd operator+(const MinSumRingEmbedd& rhs)
    {
        std::multiset<int> newS=std::multiset<int>();
        newS.insert(mset.begin(),mset.end());
        for(int node:rhs.mset){
            auto it=newS.find(-node);//was it removed before
            if(it==newS.end()){
                newS.insert(node);
            }else{
                newS.erase(it);
            }
        }
        return MinSumRingEmbedd(newS);
    }
    MinSumRingEmbedd operator-(const MinSumRingEmbedd& rhs) //+ and minus could easily be implemented with third function to avoid code duplication and nverted node
    {
        //extract, wenn niemand drin ist muss entweder eine struct das zählen oder es gibt ein weiteres multiset mit
        //entfernten nodes
        std::multiset<int> newS=std::multiset<int>();
        newS.insert(mset.begin(),mset.end());
        for(int node:rhs.mset){
            auto it=newS.find(node);//was it removed before
            if(it==newS.end()){
                newS.insert(-node);
            }else{
                newS.erase(it);
            }
        }
        return MinSumRingEmbedd(newS);
    }

};


#endif //FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
