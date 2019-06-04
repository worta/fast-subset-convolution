//
// Created by dominik on 6/3/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
#define FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H

#include <math.h>
#include <algorithm>
#include <map>
//#define NDEBUG //to disable assert
#include <assert.h>     /* assert */
#if 0
struct Element {
    int value;
    int count;

    Element(int v, int c) :
            value(v), count(c) {}
};

struct element_compare {
    bool operator()(const Element &lhs, const Element &rhs) const {
        return lhs.value < rhs.value;
    }
};
#endif


typedef std::pair<const int, int> Value_Count;
class MinSumRingEmbedd {
public:
    std::map<int,int> mset; //should use multiset, it is checked anyway if multiple are entered
    MinSumRingEmbedd(int x) :
            mset(std::map<int,int>()) {
        mset.insert(Value_Count(x, 1));
    }

    MinSumRingEmbedd(){

    }
    MinSumRingEmbedd &operator=(const int &x) //that is not well defined
    {
        mset.clear();
        if (x != 0) {
            mset.insert(Value_Count(x, 1)); //if it is set to 0, that is the 0 element of the ring, that is the empty set
        }
        return *this;
    }

    MinSumRingEmbedd &operator=(const MinSumRingEmbedd &x) {
        mset = x.mset; //copies container content
        return *this;
    }

    MinSumRingEmbedd(std::map<int,int> m) :
            mset(m) {
    }

    MinSumRingEmbedd operator*(const MinSumRingEmbedd &rhs) {
        //std::map<int,int> newS = std::map<int,int>();
        MinSumRingEmbedd newE=MinSumRingEmbedd();
        for (auto node_rhs:rhs.mset) {
            for (auto node_lhs:mset) {
                Value_Count node = Value_Count(node_lhs.first + node_rhs.first, node_lhs.second * node_rhs.second);
                assert(node.second != 0);//shouldnt happen
                newE.mset.insert(node);
            }
        }
        return newE;
    }

    MinSumRingEmbedd operator+(const MinSumRingEmbedd &rhs) {
        //std::map<Element, element_compare> newS = std::map<Element, element_compare>();
        MinSumRingEmbedd newE=MinSumRingEmbedd();
        std::pair<std::map<int,int>::iterator,bool> ret;
        newE.mset.insert(mset.begin(),mset.end());
        for(auto node:rhs.mset){
            Value_Count normal=Value_Count(node.first,node.second);
            ret=newE.mset.insert(normal);
            if(ret.second==false){ //the element exists already
                ret.first->second+=normal.second;
                /*if(ret.first->second==0){ //TODO: priciple should be optional, maybe tested whats better for performance
                    newE.mset.erase(ret);
                }*/
            }
        }

        return newE;
    }

    MinSumRingEmbedd operator-(
            const MinSumRingEmbedd &rhs)
    {
        MinSumRingEmbedd newE=MinSumRingEmbedd();
        std::pair<std::map<int,int>::iterator,bool> ret;
        newE.mset.insert(mset.begin(),mset.end());
        for(auto node:rhs.mset){
            Value_Count inverse=Value_Count(node.first,-node.second);
            ret=newE.mset.insert(inverse);
            if(ret.second==false){ //the element exists already
                ret.first->second+=inverse.second;
                /*if(ret.first->second==0){ //TODO: priciple should be optional, maybe tested whats better for performance
                    newE.mset.erase(ret);
                }*/
            }
        }
    }
    return newE;
};


#endif //FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
