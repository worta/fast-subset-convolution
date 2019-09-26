//
// Created by dominik on 6/26/19.
//

#include "benchmark_fsconv.h"
#include "common.h"
#include "steiner.h"
#include <chrono>
#include "benchmark_writer.h"
#include "string"
#include "functions.h"
#include "mobius.h"
#include "FastSubsetConvolution.h"
#include <random>

using namespace std;
using namespace std::chrono;
void benchmark_fsconv::random_func(int max_n) {
    std::string name="fsconv_benchmark";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("Nodes");
    b.write("Duration(ms)");
    b.writeln("");
    //ConstFunction<int> f(1);
    std::vector<int8_t> f_vec;
    f_vec.reserve(1<<max_n);
    std::default_random_engine generator;
    std::uniform_int_distribution<int8_t> distribution(0,10);
    for(int i=0;i<(1<<max_n);++i){
        f_vec[i]=distribution(generator);
    }
    VectFunction<int8_t> f(f_vec);
    for(int i=10; i<=max_n; ++i)
    {
        int n=i;
        //naive
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        vector<int8_t> d=naive_convolute(f,f,n);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Classic");
        b.write(n);
        b.write(duration);
        b.writeln("");
        //mobius
        t1 = high_resolution_clock::now();
        FastSubsetConvolution<int8_t> con(i);
        int8_t* result=new int8_t[con.set_count];
        con.advanced_convolute(f,result);
        //advanced_convolute<int>(f,n);
        t2 = high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(n);
        b.write(duration);
        b.writeln("");
        //myfile<<" "<<duration<<"\n";
        cout<<"Finished "<<i<<"\\"<<max_n<<endl;
//        cout<<result[0];
        delete[] result;
    }
    b.close();

}
