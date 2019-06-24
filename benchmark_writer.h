//
// Created by dominik on 6/24/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
#define FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
#include <string>
#include <fstream>
class Benchmark_writer {
    std::ofstream bfile;
public:
    explicit Benchmark_writer(std::string &filename);
    void close();
    void writeln(std::string &line);
    void write(std::string &word);
};


#endif //FAST_SUBSET_CONVOLUTION_BENCHMARK_WRITER_H
