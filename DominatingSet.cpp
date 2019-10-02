//
// Created by dominik on 10/1/19.
//

#include "DominatingSet.h"
#include "utility.h"
#include <unordered_map>
struct Coloring{
    uint32_t dominating_set; //1 if in dominating set,0 otherwise
    uint32_t not_dominated; //1 if not dominated, 0 otherwise
    //valid combinations are therefore (0,0),(1,0),(0,1)
};

static int naive_join_node(){
    //input two tables with 3 ^n values
    int node_count=5;
    set_t node_mask=(1<<5)-1;
    int n=100;
    for(int i=0;i<n;++i){
        //todo: get coloring c
        Coloring c; //just example so it works
        c.dominating_set=0;
        c.not_dominated=1;

        vector<set_t> divide=get_subsets_it(c.not_dominated); //get all sets with c_t=0 and c_t' either 0_0 or 0_1
        int min=1000;
        for(set_t not_d_1: divide){
            set_t not_d_2= ((~not_d_1)&node_mask) xor c.dominating_set;
            //((~not_d_1)&node_mask) results in all bits flipped up to the ith bit, where i is the node count
            //xor c.dominating_set prevents any bit from dominating set being set

            //find min in table with c'=(c.dominating_set, not_d_1) and c'' correspondingly
            //probably calculate the positon (0,0)=0; (1,0)=2, (0,1)=1;


        }
        //enter min

    }




    //output one table with 3^n values
}
