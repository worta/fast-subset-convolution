//
// Created by dominik on 1/8/20.
//

#include "test_steiner.h"
#include "boost/multi_array.hpp"
#include "steiner.h"
#include <iostream>
typedef boost::multi_array<int, 2> weight_matrix;
typedef weight_matrix::index index;

using namespace std;


void test_steiner() {

    //GRAPH 1

    /*  a --  1 --  b -- 1 --   d
     *     --2      |2
     *         --   c
     */
    std::cout << "------------------" << endl;
    std::cout << "Test Steiner: ";
    std::cout << "Graph 1: \n";
    weight_matrix graph(boost::extents[4][4]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            graph[i][j] = -1;
            if (i == j) {
                graph[i][i] = 0;
            }
        }
    }
    graph[0][1] = 1;
    graph[1][0] = 1;
    graph[0][2] = 2;
    graph[2][0] = 2;
    graph[1][2] = 2;
    graph[2][1] = 2;
    graph[1][3] = 1;
    graph[3][1] = 1;

    int resultc = 0;
    int resultm = 1;
    cout << "Testing {b,d,a}, expected result:2\n";
    resultc = classic_dreyfuss_wagner(graph, 4, 0b1011);
    resultm = mobius_dreyfuss(graph, 4, 0b1011,2);
   // assert(resultm == 2);
   // assert(resultc == resultm);
    cout << "Classic: " << resultc << endl;
  //  cout << "ADVANCED RESULT:" << resultm << endl;


    /*  a --  5 --  b -- 1 --   d
    *     --2      |2
    *         --   c-----1------e
    *                -1-    -3-
    *                    f
    */
    std::cout << "Test Steiner: ";
    std::cout << "Graph 2: \n";
    weight_matrix graph2(boost::extents[6][6]);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            graph2[i][j] = -1;
            if (i == j) {
                graph2[i][i] = 0;
            }
        }
    }
    graph2[0][1] = 5;
    graph2[1][0] = 5;
    graph2[0][2] = 2;
    graph2[2][0] = 2;
    graph2[1][2] = 2;
    graph2[2][1] = 2;
    graph2[1][3] = 1;
    graph2[3][1] = 1;
    graph2[2][4] = 1;
    graph2[4][2] = 1;
    graph2[2][5] = 1;
    graph2[5][2] = 1;
    graph2[4][5] = 3;
    graph2[5][4] = 3;


    cout << "Test with: {d,e,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b111000);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111000,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 5);
    assert(resultc == resultm);

    cout << "Test with: {a,b,c,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b100111);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b100111,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 5);
    assert(resultc == resultm);

    cout << "Test with: {a,b,d,f},Expected Value: 6\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b101011);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b101011,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 6);
    assert(resultc == resultm);

    cout << "Test with: {a,b},Expected Value: 4\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b000011);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b000011,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 4);
    assert(resultc == resultm);

    cout << "Test with: {b,c,d,e,f},Expected Value: 5\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b111110);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111110,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 5);
    assert(resultc == resultm);


    cout << "Test with: {a,b,c,d,e,f},Expected Value: 7\n";
    resultc = classic_dreyfuss_wagner(graph2, 6, 0b111111);
    cout << "Classic RESULT:" << resultc << endl;
    resultm = mobius_dreyfuss(graph2, 6, 0b111111,5);
    cout << "ADVANCED RESULT:" << resultm << endl;
    assert(resultm == 7);
    assert(resultc == resultm);
}


void test_ring_embedd() {
    MinSumRingEmbedd a = MinSumRingEmbedd(0) + MinSumRingEmbedd(3) - (MinSumRingEmbedd(1));
    MinSumRingEmbedd b = MinSumRingEmbedd(1) + MinSumRingEmbedd(0) - MinSumRingEmbedd(3);
    MinSumRingEmbedd c = MinSumRingEmbedd(3) + MinSumRingEmbedd(3) - MinSumRingEmbedd(4);

    MinSumRingEmbedd dist1 = a * (b + c);
    MinSumRingEmbedd dist2 = a * b + a * c;


    cout << "-------------" << endl;
    cout << "Test: Ring Embedding \n";
    cout << "Example 1\n";
    //check equal
    if (dist1.mset == dist2.mset) {
        cout << "a*(b+c)=ab+ac : Valid\n";
    } else {
        cout << "ERROR:a*(b+c)!=ab+ac\n";
    }
    dist1 = (a + b) * c;
    dist2 = a * c + b * c;

    //check equal
    if (dist1.mset == dist2.mset) {
        cout << "(a+b)*c=ac+bc : Valid\n";
    } else {
        cout << "ERROR:(a+b)*c!=ac+bc\n";
    }

}


void test_dijkstra() {
    /*  a --  1 --  b -- 1 --   d
     *     --2      |2
     *         --   c
     */
    std::cout << "Test Dijkstra: \n";
    weight_matrix graph(boost::extents[4][4]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            graph[i][j] = -1;
            if (i == j) {
                graph[i][i] = 0;
            }
        }
    }
    graph[0][1] = 1;
    graph[1][0] = 1;
    graph[0][2] = 2;
    graph[2][0] = 2;
    graph[1][2] = 2;
    graph[2][1] = 2;
    graph[1][3] = 1;
    graph[3][1] = 1;
    weight_matrix distances = compute_ap_shortest_path(graph, 4);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << distances[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "should be\n";
    std::cout << "0 1 2 2\n1 0 2 1\n2 2 0 3\n2 1 3 0\n";

}