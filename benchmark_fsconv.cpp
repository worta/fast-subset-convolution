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

using namespace std;
using namespace std::chrono;
void benchmark_fsconv::constant_func(int max_n) {
    std::string name="fsconv_benchmark";
    Benchmark_writer b=Benchmark_writer(name);
    b.write("Type");
    b.write("Nodes");
    b.write("Duration(ms)");
    b.writeln("");
    ConstFunction<int> f(1);
    for(int i=6; i<21; ++i)
    {
        if(i%5==0){
            cout<<"Finished "<<i<<"\\"<<max_n<<endl;
        }
        int n=i;
        //naive
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        naive_convolute(f,f,n);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Classic");
        b.write(n);
        b.write(duration);
        b.writeln("");
        //mobius
        t1 = high_resolution_clock::now();
        advanced_convolute<int>(f,n);
        t2 = high_resolution_clock::now();
        duration=std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
        b.write("Mobius");
        b.write(n);
        b.write(duration);
        b.writeln("");
        //myfile<<" "<<duration<<"\n";
    }
    b.close();

}
