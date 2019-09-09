#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Abstract base class
using namespace std;
#include <vector>
#include "utility.h"
template <class T>
class Function
{
public:
    Function() {};
    virtual T operator() (set_t subset)=0;
};

template <class T>
class RankedFunction
{
public:
    RankedFunction() {};
    virtual T operator() (int ran,set_t subset)=0;
};
template <typename T>
class ConstFunction : public Function<T>
{
    T value;
public:
    ConstFunction(T v)
    {
        value=v;
    }
    T operator() (set_t s)
    {
        return value;
    }
};

template <typename T>
class VectFunction : public Function<T>
{
    vector<T> &v;
public:
    VectFunction(vector<T> &vect):
    v(vect)
    {
    };
    T operator() (set_t s)
    {
        return v[s];
    }
};

template <typename T>
class ThresholdFunction : public Function<T>
{
    T* v;
    int t;
public:
    ThresholdFunction(T* arr,int threshold):
            v(arr),t(threshold)
    {
    };
    T operator() (set_t s)
    {
        return v[s]>=t;
    }
};


template <typename T>
class RankedVectFunction : public RankedFunction<T>
{
    vector<vector<T> > &v;
public:
    RankedVectFunction(vector<vector<T> > &vect):
    v(vect)
    {
    };
    T operator() (int ran,set_t subset)
    {
        return v[ran][subset];
    }
};

#endif // FUNCTIONS_H_INCLUDED
