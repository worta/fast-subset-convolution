//
// Created by dominik on 10/1/19.
//

#include "DominatingSet.h"
#include "utility.h"
#include <unordered_map>
#include "FastSubsetConvolution.h"
#include "functions.h"

struct Coloring {
    uint32_t dominating_set; //1 if in dominating set,0 otherwise
    uint32_t not_dominated; //1 if not dominated, 0 otherwise
    //valid combinations are therefore (0,0),(1,0),(0,1), with (0,0) dominated but not in dominating set
};

static int calculate_index(uint32_t dom_set, uint32_t not_dom, int node_count) {
    //values in array correspond to 00,01,10,11
    static int8_t dominated_value[] = {0, 2, 6, 8}; //if in dominating set ->2 in terniary
    static int8_t not_dom_value[] = {0, 1, 3, 4}; //if not dominated ->1 in terniary otherwise 0
    int index = 0;
    int level = 1;
    for (int i = 0; i < node_count / 2 + 1; ++i) {
        index = index + (dominated_value[dom_set & 3] + not_dom_value[not_dom & 3]) * level; //3= 0b11
        dom_set = dom_set >> 2;
        not_dom = not_dom >> 2;
        level = level * 9;
    }

    return index;
}

//calculate only the index part for dom set
static int calculate_dom_index(uint32_t dom_set, int node_count) {
    //values in array correspond to 00,01,10,11
    static int8_t dominated_value[] = {0, 2, 6, 8}; //if in dominating set ->2 in terniary
    int index = 0;
    int level = 1;
    for (int i = 0; i < node_count / 2 + 1; ++i) {
        index = index + dominated_value[dom_set & 3] * level; //3= 0b11
        dom_set = dom_set >> 2;
        level = level * 9;
    }
    return index;
}

//calculate only the index part for not_dom set, needs dom set though to differentitate between dominating set and domianted
static int calculate_not_dom_index(uint32_t dom_set, uint32_t not_dom, int node_count) {
    //values in array correspond to 00,01,10,11
    static int8_t not_dom_value[] = {0, 1, 3, 4}; //if not dominated ->1 in terniary otherwise 0
    static int8_t dom_value[] = {0, -1, -3, -4}; //subtract wrongly added dominated vertices
    int index = 0;
    int level = 1;
    for (int i = 0; i < node_count / 2 + 1; ++i) {
        index = index + (dom_value[dom_set & 3] + not_dom_value[not_dom & 3]) * level; //3= 0b11
        not_dom = not_dom >> 2;
        dom_set = dom_set >> 2;
        level = level * 9;
    }
    return index;
}

void DominatingSet::naive_join_node(int8_t *child1, int8_t *child2, int node_count, int8_t *result) {
    //input two tables with 3 ^n values
    set_t node_mask = (1 << node_count) - 1;
    //int n = ipow(3, node_count);
    for (set_t dom_set = 0; dom_set < (1 << node_count); ++dom_set) {
        set_t valid_not_dom_superset = (~dom_set) & node_mask;
        int one_count = __builtin_popcount(dom_set);
        vector<set_t> not_dominated_sets = get_subsets(valid_not_dom_superset);
        int ind1dom = calculate_dom_index(dom_set, node_count);
        int ind2dom = calculate_dom_index(dom_set, node_count);
        for (set_t not_dominated:not_dominated_sets) {
            Coloring c; //just example so it works
            // c.dominating_set=0;
            //  c.not_dominated=1;
            //loop through colors by looping through dominating set by 0...2^n
            //and not_dominated by generating subsets of ~dominating_set
            vector<set_t> divide = get_subsets(dom_set); //get all sets with c_t=0 and c_t' either 0_0 or 0_1
            int min = 1000;

            for (set_t not_d_1: divide) {
                set_t not_d_2 = ((~not_d_1) & node_mask) bitand not_dominated;
                //((~not_d_1)&node_mask) results in all bits flipped up to the ith bit, where i is the node count
                //xor c.not_dominated prevents any bit with state "dominated" being set to 1, i.e. being set to nto dominated
                //find min in table with c'=(c.dominating_set, not_d_1) and c'' correspondingly
                //probably calculate the positon (0,0)=0; (1,0)=2, (0,1)=1;
                // int index1 = calculate_index(dom_set, not_d_1,
                //                               node_count); //can be made faster by calculating dominating set und not_d2 part separatly
                // int index2 = calculate_index(dom_set, not_d_2, node_count);
                int index1 = ind1dom + calculate_not_dom_index(dom_set, not_d_1, node_count);
                int index2 = ind2dom + calculate_not_dom_index(dom_set, not_d_2, node_count);
                if (min > child1[index1] + child2[index2] - one_count) {
                    min = child1[index1] + child2[index2] - one_count;
                }
            }
            int index = calculate_index(dom_set, not_dominated, node_count);
            result[index] = min;
        }
        //enter min
    }
    //output one table with 3^n values
}


void DominatingSet::mobius_join_node(int8_t *child1, int8_t *child2, int minc_1, int minc_2, int node_count,
                                     int8_t *result) {
    //input two tables with 3 ^n values
    set_t node_mask = (1 << node_count) - 1; //has the form 00111..1 with #1= #nodes
    int n = ipow(3, node_count);
    int min_possible = min(minc_1 + minc_2 - (node_count ),0); //can be less than minc1+minc2, because nodes might be counted double
    //see de fluiter property, has to be in range (e_x+e_y-(k+1))
    int max_possible = minc_1 + minc_2 + node_count + 1; //((e_x+e_y+k+
    int value_range = max_possible - min_possible;
    int subset_count = 1 << node_count;
    set_t *subset_and_idx_buffer = new set_t[subset_count];

    int max_set_count = subset_count;
    int8_t *t_1 = new int8_t[max_set_count];
    int8_t *t_2 = new int8_t[max_set_count];
    int8_t *temp_result_buffer = new int8_t[max_set_count];

    for (set_t dom_set = 0; dom_set < subset_count; ++dom_set) {
        set_t valid_not_dom_superset = (~dom_set) & node_mask;
        int zero_count = __builtin_popcount(valid_not_dom_superset);
        int dom_index = calculate_dom_index(dom_set, node_count);
        //vector<set_t> not_dominated_sets = get_subsets(valid_not_dom_superset);
        int set_count = get_subsets(valid_not_dom_superset, subset_and_idx_buffer);
#if 0
        //create tables
        // int set_count=not_dominated_sets.size();
        /*int8_t *t_1 = new int8_t[set_count * value_range]; //i.e t1 points to [possuble values][sets]
        int8_t *t_2 = new int8_t[set_count * value_range];
        int set_counter = 0;
        for (int set_idx = 0; set_idx < set_count; ++set_idx) {
            int not_dom_index = calculate_not_dom_index(dom_set, subset_and_idx_buffer[set_idx], node_count);
            int index = not_dom_index + dom_index;
            for (int i = 0; i < value_range; ++i) {
                t_1[i * set_count + set_counter] = 0;
                t_2[i * set_count + set_counter] = 0;
                if (child1[index] == i + min_possible) {
                    t_1[i * set_count + set_counter] = 1;
                }
                if (child2[index] == i + min_possible) {
                    t_2[i * set_count + set_counter] = 1;
                }
            }
            ++set_counter;
            subset_and_idx_buffer[set_idx] = index; //replace set by set index, the bit representation is not needed anymore
        }*/
#endif
        //int set_counter = 0;
        for (int set_idx = 0; set_idx < set_count; ++set_idx) {
            int not_dom_index = calculate_not_dom_index(dom_set, subset_and_idx_buffer[set_idx], node_count);
            int index = not_dom_index + dom_index;
            t_1[set_idx]= child1[index];
            t_2[set_idx]= child2[index];
            subset_and_idx_buffer[set_idx] = index; //replace set by set index, the bit representation is not needed anymore
        }
        FastSubsetConvolution<int8_t> f(zero_count, true); //node_count-one_count equals the amount of 0s

        for (int i = min_possible; i < max_possible; ++i) {
            for (int j = min_possible; j < max_possible; ++j) {
                //fastr subset convolute t1[i*set_count] and t2[j*set_count]
               /* ArrayFunction<int8_t> a(&t_1[i * set_count]);
                ArrayFunction<int8_t> b(&t_2[j * set_count]); */
                EqualFunction<int8_t> a(t_1,i);
                EqualFunction<int8_t> b(t_2,j);
                //f.advanced_convolute(a,b,temp_result_buffer); //needs f to be constructed with only covering=false
                f.advanced_covering_product(a, b, temp_result_buffer); //this should be faster
                for (int temp_result_idx = 0; temp_result_idx < set_count; ++temp_result_idx) {
                    if (temp_result_buffer[temp_result_idx] > 0) {
                        if (result[subset_and_idx_buffer[temp_result_idx]] > i + j) {
                            result[subset_and_idx_buffer[temp_result_idx]] = i + j;
                        }
                    }
                }
            }
        }


    }
    delete[] temp_result_buffer; //todo maybe just allocate max memory and delte at the end
    delete[] t_1;
    delete[] t_2;
    delete[] subset_and_idx_buffer;
}