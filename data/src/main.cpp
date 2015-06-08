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
#include <map>
using namespace std;
using namespace sae::io;
    
map<string, int> nodeMap;

int GetOrInsert(const string& key)
{
    map<string, int>::iterator it = nodeMap.find(key);
    if (it != nodeMap.end())
        return it -> second;
    int id = (int) nodeMap.size();
    nodeMap.insert(make_pair(key, id));
    return id;
}

int main(int argc, char **argv) {
    GraphBuilder<int> graph;
    //ifstream fin("./resource/amazon0312.txt");
    ifstream fin("./resource/twitter_combined.txt");
    string buf;
    //for (int i = 0; i < 4; ++i) getline(fin, buf);
    int v_cnt(-1);
    map<string, int> nodeMap;
    while (1) {
        string x, y;
        if (!(fin >> x >> y)) break;
        int a = GetOrInsert(x);
        int b = GetOrInsert(y);
        int c = max(max(v_cnt, a), b);
        while (v_cnt < c) graph.AddVertex(++v_cnt, 0);
        graph.AddEdge(a, b, 0);
    }
    cout << graph.VertexCount() << " " << graph.EdgeCount() << endl;
    graph.Save("./data/twitter");
    return 0;
}

