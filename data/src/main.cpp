/*************************************************************************
	> Author: Wayne Ho
	> Purpose: TODO
	> Created Time: Wed 03 Jun 2015 11:01:35 AM CST
	> Mail: hewr2010@gmail.com 
 ************************************************************************/
#include "storage/graph_builder.h"
#include <iostream>
#include <cstdio>
#include <fstream>
using namespace std;
using namespace sae::io;

int main(int argc, char **argv) {
    GraphBuilder<int> graph;
    ifstream fin("./resource/amazon0312.txt");
    string buf;
    for (int i = 0; i < 4; ++i) getline(fin, buf);
    int v_cnt(-1);
    while (1) {
        int x, y;
        if (!(fin >> x >> y)) break;
        int z = max(max(v_cnt, x), y);
        while (v_cnt < z) graph.AddVertex(++v_cnt, 0);
        graph.AddEdge(x, y, 0);
    }
    cout << graph.VertexCount() << " " << graph.EdgeCount() << endl;
    graph.Save("./data/graph");
    return 0;
}

