//
// Created by dominik on 6/24/19.
//

#include "benchmark_writer.h"
Benchmark_writer::Benchmark_writer(std::string &filename){
    bfile.open(filename);
}


void Benchmark_writer::close(){
    bfile.close();
}

void Benchmark_writer::writeln(std::string &line) {
    bfile<<line<<"\n";
}

void Benchmark_writer::write(std::string &word) {
    bfile<<word;
}
