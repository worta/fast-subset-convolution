//
// Created by dominik on 6/3/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
#define FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H

#include <math.h>
#include <algorithm>
#include <set>
//#define NDEBUG //to disable assert
#include <assert.h>     /* assert */

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

class MinSumRingEmbedd {
public:
    std::set<Element, element_compare> &mset; //should use multiset, it is checked anyway if multiple are entered
    MinSumRingEmbedd(int x) :
            mset(std::set<int, element_compare>()) {
        mset.insert(Element(x, 1));
    }

    MinSumRingEmbedd &operator=(const int &x) //that is not well defined
    {
        mset.clear();
        if (x != 0) {
            mset.insert(Element(x, 1)); //if it is set to 0, that is the 0 element of the ring, that is the empty set
        }
        return *this;
    }

    MinSumRingEmbedd &operator=(const MinSumRingEmbedd &x) {
        mset = x.mset; //copies container content
        return *this;
    }

    MinSumRingEmbedd(std::set<Element, element_compare, std::allocator<Element>> m) :
            mset(m) {
    }

    MinSumRingEmbedd operator*(const MinSumRingEmbedd &rhs) {
        std::set<Element, element_compare> newS = std::set<Element, element_compare>();
        for (Element node_rhs:rhs.mset) {
            for (Element node_lhs:mset) {
                Element node = Element(node_lhs.value + node_rhs.value, node_lhs.count * node_rhs.count);
                assert(node.count != 0);//shouldnt happen
                newS.insert(node);
            }
        }
        return MinSumRingEmbedd(newS);
    }

    MinSumRingEmbedd operator+(const MinSumRingEmbedd &rhs) {
        std::set<Element, element_compare> newS = std::set<Element, element_compare>();
        newS.insert(mset.begin(), mset.end());
        for (Element node:rhs.mset) {
            auto it = newS.find(node);
            if (it == newS.end()) {
                newS.insert(node);
            } else {
                newS.erase(it);
            }
        }
        return MinSumRingEmbedd(newS);
    }

    MinSumRingEmbedd operator-(
            const MinSumRingEmbedd &rhs) //+ and minus could easily be implemented with third function to avoid code duplication and nverted node
    {
        //extract, wenn niemand drin ist muss entweder eine struct das zählen oder es gibt ein weiteres multiset mit
        //entfernten nodes
        std::set<Element,element_compare> newS = std::set<Element,element_compare>();
        newS.insert(mset.begin(), mset.end());
        for (Element node:rhs.mset) {
            auto it = newS.find(node);//was it removed before
            if (it == newS.end()) {
                newS.insert(node);
            } else {
                newS.erase(it);
            }
        }
        return MinSumRingEmbedd(newS);
    }

};


#endif //FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDD_H
