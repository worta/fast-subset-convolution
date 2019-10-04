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

static int calculate_index(uint32_t dom_set,uint32_t not_dom,int node_count){
    //values in array correspond to 00,01,10,11
    static int8_t dominated_value[]={0,2,6,8}; //if in dominating set ->2 in terniary
    static int8_t not_dom_value[]={0,1,3,4}; //if not dominated ->1 in terniary otherwise 0
    int index=0;
    int level=1;
    for(int i=0;i<node_count/2+1;++i){
        index=index+(dominated_value[dom_set&3]+not_dom_value[dom_set&3])*level; //3= 0b11
        dom_set=dom_set>>2;
        not_dom=not_dom>>2;
        level=level*9;
    }
    return index;
}

static int naive_join_node(int8_t* child1,int8_t* child2,int node_count){
    //input two tables with 3 ^n values
    set_t node_mask=(1<<node_count)-1;
    int n=ipow(3,node_count);
    int8_t* result=new int8_t[n];
    for(set_t dom_set=0;dom_set<(1<<node_count);++dom_set){
        set_t valid_not_dom_superset=(~dom_set)&node_mask;
        int one_count=__builtin_popcount(dom_set);
        vector<set_t> not_dominated_sets=get_subsets_it(valid_not_dom_superset);
        for(set_t not_dominated:not_dominated_sets){
            Coloring c; //just example so it works
           // c.dominating_set=0;
           //  c.not_dominated=1;
            //loop through colors by looping through dominating set by 0...2^n
            //and not_dominated by generating subsets of ~dominating_set
            vector<set_t> divide=get_subsets_it(dom_set); //get all sets with c_t=0 and c_t' either 0_0 or 0_1
            int min=1000;
            for(set_t not_d_1: divide){
                set_t not_d_2= ((~not_d_1)&node_mask) bitand not_dominated;
                //((~not_d_1)&node_mask) results in all bits flipped up to the ith bit, where i is the node count
                //xor c.not_dominated prevents any bit with state "dominated" being set to 1, i.e. being set to nto dominated
                //find min in table with c'=(c.dominating_set, not_d_1) and c'' correspondingly
                //probably calculate the positon (0,0)=0; (1,0)=2, (0,1)=1;
                int index1=calculate_index(dom_set,not_d_1,node_count); //can be made faster by calculating dominating set und not_d2 part separatly
                int index2=calculate_index(dom_set,not_d_2,node_count);
                if(min> child1[index1]+child2[index2]-one_count){
                    min=child1[index1]+child2[index2]-one_count;
                }
            }
            int index=calculate_index(dom_set,not_dominated,node_count);
            result[index]=min;
        }
        //enter min
    }
    //output one table with 3^n values
}
