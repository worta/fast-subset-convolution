//
// Created by dominik on 6/24/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
#define FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
class Benchmark_writer {
    std::ofstream bfile;
    std::string path="./Benchmarks/";
    std::string stdout_str;
public:
    explicit Benchmark_writer(std::string &filename);
    void close();
    void writeln(std::string line);
    void write(std::string word);
    void write(int word);
    void write(std::chrono::duration<double> word);

};


#endif //FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
