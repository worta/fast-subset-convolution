//
// Created by dominik on 6/12/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_STP_PARSER_H
#define FAST_SUBSET_CONVOLUTION_STP_PARSER_H
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <string>
#include <iostream>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
using qi::double_;
using qi::phrase_parse;
using ascii::space;

class stp_parser {
private:


    std::vector<std::vector<int> > edge_weights;
    void insert_edge();
    void setup();
public:
    void parse();
};



#endif //FAST_SUBSET_CONVOLUTION_STP_PARSER_H
