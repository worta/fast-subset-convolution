//
// Created by dominik on 6/12/19.
//
//Unnötig, einfach eigene beispiele
#include "stp_parser.h"
void stp_parser::parse(){


    std::string line="Test";
    //getline(myFile, line, '\n');
    bool r = phrase_parse(
            line.begin(),
    line.end(),
    qi::double_ >> *(',' >> qi::double_),
    ascii::space
    );

}
//http://steinlib.zib.de/format.php

//hier erste zeile und section comment
void stp_parser::setup(){

}
