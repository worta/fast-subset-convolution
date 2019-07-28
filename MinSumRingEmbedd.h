//
// Created by dominik on 6/3/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
#define FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H

#include <cmath>
#include <algorithm>
#include <map>
//#define NDEBUG //to disable assert
#include <cassert>     /* assert */



typedef std::pair<const int, int> Value_Count;
class MinSumRingEmbedd {
public:
    std::map<int,int> mset;
    explicit MinSumRingEmbedd(int x) :
            mset(std::map<int,int>()) {
        mset.insert(Value_Count(x, 1));
    }

    MinSumRingEmbedd()= default;

    //should only be used to set map to 0, i.e. to empty set
    MinSumRingEmbedd &operator=(const int &x) //that is not well defined
    {
        mset.clear();
       /* if (x != 0) {
            mset.insert(Value_Count(x, 1)); //if it is set to 0, that is the 0 element of the ring, that is the empty set
        }*/
        return *this;
    }

    MinSumRingEmbedd &operator=(const MinSumRingEmbedd &x) {
        mset = x.mset; //copies container content
        return *this;
    }

    explicit MinSumRingEmbedd(std::map<int,int> &m) :
            mset(m) {
    }

    MinSumRingEmbedd operator*(const MinSumRingEmbedd &rhs) {
        //std::map<int,int> newS = std::map<int,int>();
        MinSumRingEmbedd newE;
        std::pair<std::map<int,int>::iterator,bool> ret;
        for (auto node_rhs:rhs.mset) { //todo: hier über hint beim einfügen nachdenken
            for (auto node_lhs:mset) {
                Value_Count node = Value_Count(node_lhs.first + node_rhs.first, node_lhs.second * node_rhs.second);
               // assert(node.second != 0);//shouldnt happen in normal code, but does in tsetrs
                ret=newE.mset.insert(node);
                if(!ret.second) { //the element exists already
                    ret.first->second += node.second;
                }
            }
        }
        return newE;
    }

    MinSumRingEmbedd operator+(const MinSumRingEmbedd &rhs) {
        //std::map<Element, element_compare> newS = std::map<Element, element_compare>();
        MinSumRingEmbedd newE;
        std::pair<std::map<int,int>::iterator,bool> ret;
        newE.mset.insert(mset.begin(),mset.end());
        for(auto node:rhs.mset){
            Value_Count normal=Value_Count(node.first,node.second);
            ret=newE.mset.insert(normal);
            if(!ret.second){ //the element exists already
                ret.first->second+=normal.second;
               // if(ret.first->second==0){ //TODO: priciple should be optional, maybe tested whats better for performance
               //     newE.mset.erase(ret.second);
               // }
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
            if(!ret.second){ //the element exists already
                ret.first->second+=inverse.second;
               // if(ret.first->second==0){ //TODO: priciple should be optional, maybe tested whats better for performance
                //    newE.mset.erase(ret.second);
               // }
            }
        }
        return newE;
    }

    int min(){
        for(auto pairIt:mset){
            if(pairIt.second!=0){
                return pairIt.first;
            }
        }
        assert(false);
        return 100;
    }

};


#endif //FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
