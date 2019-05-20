#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
// Abstract base class
using namespace std;
#include <vector>
template <class T>
class Function
{
public:
    Function() {};
    virtual T operator() (int subset)=0;
};

template <class T>
class RankedFunction
{
public:
    RankedFunction() {};
    virtual T operator() (int ran,int subset)=0;
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
    T operator() (int s)
    {
        return value;
    }
};

template <typename T>
class VectFunction : public Function<T>
{
    vector<T> v;
public:
    VectFunction(vector<T> &vect) //TODO: Reference?
    {
        v=vect;
    };
    T operator() (int s)
    {
        return v[s];
    }
};

template <typename T>
class RankedVectFunction : public RankedFunction<T>
{
    vector<vector<T> > v;
public:
    RankedVectFunction(vector<vector<T> > &vect)
    {
        v=vect;
    };
    T operator() (int ran,int subset)
    {
        return v[ran][subset];
    }
};

#endif // FUNCTIONS_H_INCLUDED
