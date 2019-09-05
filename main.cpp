#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>

#include "functions.h"
#include "mobius.h"
#include "steiner.h"
#include "common.h"
#include "utility.h"
#include "benchmark_steiner.h"
#include "benchmark_fsconv.h"
#include "benchmark_path_embedding.h"
#include "FastSubsetConvolution.h"

#include "MinSumRingEmbeddBigInt.h"

using namespace std;


/*class int_max
{
public:
    int value;
    int_max ()
    {
        value=0;
    }
    int_max (int x)
    {
        value=x;
    }
    int_max& operator= (const int& x)
    {
        value=pow(2,x)+1;
        return *this;
    }
    int_max& operator= (const int_max& x)
    {
        value=x.value;
        return *this;
    }
    int_max operator*(const int_max& rhs)
    {
        return int_max(value + rhs.value);
    }
    int_max operator+(const int_max& rhs)
    {
        return int_max();
    }
    int_max operator-(const int_max& rhs) //thats just wrong
    {
        return int_max(value-rhs.value);
    }

} ;*/



/*inline int bitCount(int i) { //counts bit set to 1 for 32 bit number
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    i = (i + (i >> 4)) & 0x0f0f0f0f;
    i = i + (i >> 8);
    i = i + (i >> 16);
    return i & 0x3f;
}*/


void test_subset_gen()
{
    cout <<"Test subset generation"<<endl;
    set_t test_set=26; //<1,2>
    cout <<"Testing subset:";
    output_set(test_set,5);
    cout<<endl;
    vector<set_t> test_vec {0,2,8,16,10,18,24,26};
    vector<set_t > results;
    //results.reserve(2);
    //getSubsets(test_set,results);
    results= get_subsets_it(test_set);
    if(results.size()==test_vec.size())
    {
        cout<<"Size as expected"<<endl;
    }
    else{
        cout<<"Error: Wrong subset size"<<endl;
    }
    for(set_t i=0; i<results.size(); ++i)
    {
        output_set(results[i],5);
        test_vec.erase(std::remove(test_vec.begin(), test_vec.end(), results[i]), test_vec.end()) ;
    }
    if(test_vec.empty())
    {
        cout<<"Subsets as expected"<<endl;
    }
    else{
        cout<<"Error: Wrong elements in subset generation"<<endl;
    }
}

void test_ranked_mobius_and_convolute()
{
    int n=2; //test set={1,2}
    ConstFunction<int> f=ConstFunction<int>(1);
    FastSubsetConvolution<int> Convolute(n);
    //Test mobius transformation
    //transformed f should be:
    vector<int> f_0 {1,1,1,1};
    vector<int> f_1 {0,1,1,2};
    vector<int> f_2 {0,0,0,1};
    vector<vector<int> > results {f_0,f_1,f_2};
    int results_arr[]={1,1,1,1,0,1,1,2,0,0,0,1};
    int r_ranked_mobius[12];

    vector<vector<int> > rankedFVec(n+1);
    for(int rang=0; rang<=n; rang++)
    {
        Convolute.ranked_mobius(f,rang,&r_ranked_mobius[rang*Convolute.set_count]);
        rankedFVec[rang]=rankedMobius<int>(f,n,rang);
        if( rankedFVec[rang]==results[rang])
        {
            cout<<"Ranked mobius transform:Values for rank:" <<rang <<" OK"<<endl;

        }
        for(int i=0;i<Convolute.set_count;++i){
            if(!(r_ranked_mobius[rang*Convolute.set_count+i]==results[rang][i])){

                cout<<"ERROR: ranked mobius transform"<<endl;
                assert(false);
            }
        }
    }

    //test ranked mobius convolution
    //(transformed f)*(transformed f) should be:
    f_0= {1,1,1,1};
    f_1= {0,2,2,4};
    f_2= {0,1,1,6};
    results= {f_0,f_1,f_2};

    //RankedVectFunction<int> ranked_f=RankedVectFunction<int>(rankedFVec);
    vector< vector<int> > conv=ranked_convolute(rankedFVec,rankedFVec,n);
    int r_convoluted[12];
    Convolute.ranked_convolute(r_ranked_mobius,r_ranked_mobius,r_convoluted);
    for(int rang=0; rang<=n; rang++)
    {
        if( conv[rang]==results[rang])
        {
            cout<<"Ranked mobius convolution:Values for rank:" <<rang <<" OK"<<endl;
        }
        else
        {
            cout<<"Error: ranked mobius convolution:Values for rank:" <<rang <<" are incorrect"<<endl;
        }
        for(int i=0;i<Convolute.set_count;++i){
            if(!(r_convoluted[rang*Convolute.set_count+i]==results[rang][i])){
                cout<<"ERROR: ranked convolute"<<endl;
                assert(false);
            }
        }


    }

    //test naive convolute:
    vector<int> naive=naive_convolute(f,f,n);
    vector<int> convolute_result {1,2,2,4};
    if (naive==convolute_result)
    {
        cout<<"Naive convolute: OK"<<endl;
    }
    else
    {
        cout<<"Error: Naive convolute: Incorrect values"<<endl;
    }

    int r_inverse_simple[4];
    vector<int> normal_invert=ranked_Mobius_inversion(rankedFVec,n); //should be the same values as f
    Convolute.ranked_mobius_inversion(r_ranked_mobius,r_inverse_simple);
    if(normal_invert==vector<int> {1,1,1,1})
    {
        cout <<"Normal Mobius inversion: OK"<<endl;
    }
    else
    {
        cout<<"Error: normal invert is wrong"<<endl;
    }
    for(int i=0;i<4;++i){
        if(r_inverse_simple[i]!=1){
            cout<<"ERROR: mobius inversion, simple case is wrong"<<endl;
            assert(false);
        }
    }

    RankedVectFunction<int> ranked_conv=RankedVectFunction<int>(conv);
    vector<int> inverse=ranked_Mobius_inversion<int>(conv,n); //should be the same as f*f
    int r_inverse[4];
    Convolute.ranked_mobius_inversion(r_convoluted,r_inverse);
    if (inverse==convolute_result)
    {
        cout<<"Mobius inversion of ranked convolution: OK"<<endl;
    }
    else
    {
        cout<<"Error: Mobius inversion: Incorrect values"<<endl;
        cout<<"Should be: {1,2,2,4} but is {"<<inverse[0];
        for(set_t i=1; i<inverse.size(); ++i)
        {
            cout<<","<<inverse[i];
        }
        cout<<"}"<<endl;
    }
    for(int i=0;i<4;++i){
        if((r_inverse[i]!=convolute_result[i])){
            cout<<"ERROR: ranked mobius inversion"<<endl;
            cout<<"i: "<<i<<"Expected: "<<convolute_result[i]<<" is:"<<r_inverse[i]<<endl;
            assert(false);
        }
    }
}

void test_ring_embedd_big_int(){
    MinSumRingEmbeddBigInt::init(3,4);
    MinSumRingEmbeddBigInt a = MinSumRingEmbeddBigInt(0) + MinSumRingEmbeddBigInt(3) + MinSumRingEmbeddBigInt(1);
    MinSumRingEmbeddBigInt b = MinSumRingEmbeddBigInt(1) + MinSumRingEmbeddBigInt(0) + MinSumRingEmbeddBigInt(3);
    MinSumRingEmbeddBigInt c = MinSumRingEmbeddBigInt(3) + MinSumRingEmbeddBigInt(3) + MinSumRingEmbeddBigInt(4);

    MinSumRingEmbeddBigInt dist1 = a * (b + c);
    MinSumRingEmbeddBigInt dist2 = a * b + a * c;




    cout << "-------------" << endl;
    cout << "Test: Ring Embedding \n";
    cout << "Example 1\n";
    dist1 = (a + b) * c;
    dist2 = a * c + b * c;

    //check equal
    if (dist1.min() == dist2.min()) {
        cout << "(a+b)*c=ac+bc : Valid\n";
    } else {
        cout << "ERROR:(a+b)*c!=ac+bc\n";
    }


    MinSumRingEmbeddBigInt min_test = MinSumRingEmbeddBigInt(1) + MinSumRingEmbeddBigInt(3) + MinSumRingEmbeddBigInt(2);
    if(min_test.min()!=1){
        cout<<"ERROR: wrong min: "<<min_test.min()<<" should be 1";
        //assert(false);
    }

}

int main()
{
    //ConstFunction<int> f=ConstFunction<int>(1);
//    test_subset_gen();
    //test_ranked_mobius_and_convolute();
   // test_steiner();
/*
    test_dijkstra();
    test_ring_embedd();

    */
    //ofstream myfile;
    //myfile.open("/home/dominik/Studium/Master/fast-subset-convolution/benchmarks/results.txt");
    //myfile<<"n naive mobius\n";
    //myfile.close();
    //test_ring_embedd();
    //test_ring_embedd_big_int();
    cout<<"Starting Steiner Benchmark \n";
    //benchmark_steiner::complete_graphs(20);
    cout<<"Finished Benchmark\n";

    cout<<"Starting Fast Subset COnv Benchmark \n";
   // benchmark_fsconv::constant_func(22);
    cout<<"Finished Benchmark\n";

    benchmark_path_embedding::embedd_length_1_paths(12);
    return 0;
}
