//
// Created by dominik on 6/24/19.
//

#include "benchmark_writer.h"
#include <iostream>
Benchmark_writer::Benchmark_writer(std::string  &filename){
    time_t rawtime;
    time (&rawtime);
    stdout_str="";
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
    stdout_str=stdout_str+line;
    bfile<<line<<"\n";
    std::flush(bfile);
    std::cout<<stdout_str<<"\n";
    stdout_str="";
}

void Benchmark_writer::write(std::string word) {
    stdout_str=stdout_str+" "+word;
    bfile<<word<<"\t";
    std::flush(bfile);
}

void Benchmark_writer::write(int word) {
    stdout_str=stdout_str+" "+std::to_string(word);
    bfile<<word<<"\t";
    std::flush(bfile);
}

void Benchmark_writer::write(std::chrono::duration<double> word) {
    stdout_str=stdout_str+" "+std::to_string(word.count());
    bfile<<word.count()<<"\t";
    std::flush(bfile);
}
