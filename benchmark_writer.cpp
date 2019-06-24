//
// Created by dominik on 6/24/19.
//

#include "benchmark_writer.h"

Benchmark_writer::Benchmark_writer(std::string  &filename){
    time_t rawtime;
    time (&rawtime);
    struct tm * timeinfo;
    char buffer [8];
    timeinfo = localtime (&rawtime);
    strftime(buffer,80,"%d%m%y",timeinfo);
    bfile.open(path+buffer+filename+".csv");
}


void Benchmark_writer::close(){
    bfile.close();
}

void Benchmark_writer::writeln(std::string line) {
    bfile<<line<<"\n";
}

void Benchmark_writer::write(std::string word) {
    bfile<<word<<"\t";
}

void Benchmark_writer::write(int word) {
    bfile<<word<<"\t";
}

void Benchmark_writer::write(std::chrono::duration<double> word) {
    bfile<<word.count()<<"\t";
}
