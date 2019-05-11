#include "utility.h"
#include <cstdint>
#include <cmath>
void getSubsets(int superset,vector<int> &results)
{
    int index= __builtin_ffs(superset); //Returns one plus the index of the least significant 1-bit of x, or if x is zero, returns zero.
    if(index==0)
    {
        results.push_back(0);
        return;
    }
    else
    {
        int element=1<<(index-1);
        int new_set=superset xor element;
        getSubsets(new_set,results);
        int current_elems=results.size();
        for(int i=0; i<current_elems; ++i)
        {
            results.push_back(results[i] | element);
        }
    }

}

void getSubsetsIt(int superset,vector<int> &results){
    int limit=(int)pow(2,__builtin_popcount(superset));
    for(uint32_t i=0;i<limit;++i){
        
    }
}

