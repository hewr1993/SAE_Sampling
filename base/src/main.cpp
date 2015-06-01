/*************************************************************************
	> Author: Wayne Ho
	> Purpose: TODO
	> Created Time: Fri Apr 17 16:32:22 2015
	> Mail: hewr2010@gmail.com 
 ************************************************************************/
#include "solver/solver.h"
#include "triangle_brute_force.h"
#include "triangle_stream.h"
#include "storage/graph_builder.h"
#include "storage/mgraph.h"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <map>
#include <cstdlib>
using namespace std;
using namespace sae::io;


void makeFakeData(int numVertex=10) {
	int numEdge = rand() % (numVertex * numVertex / 3) + numVertex;
	GraphBuilder<int> graph;
	for (int i = 0; i < numVertex; ++i) graph.AddVertex(i, 0);
	map<pair<int, int>, bool> edges;
	for (int i = 0; i < numEdge; ++i) {
		pair<int, int> edge;
		do {
			int x = rand() % (numVertex - 1);
			int y = rand() % (numVertex - x - 1) + x + 1;
			edge = make_pair(x, y);
		} while (edges.find(edge) != edges.end());
		edges[edge] = true;
		graph.AddEdge(edge.first, edge.second, 0);
		//cout << edge.first << " " << edge.second << endl;
	}
	system("mkdir -p fake");
	graph.Save("./fake/graph");
}


void testTriangle(char* prefix="./fake/graph") {
	MappedGraph *graph;
	graph = MappedGraph::Open(prefix);
	// brute force
	Triangle_Brute_Force bf(graph);
	int bf_cnt = bf.solve();
	cout << "[brute force]\t" << bf_cnt << endl;
	// streaming
	Triangle_Stream stm(50, 1000);
	int stream_cnt(0);
	for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
		vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
		auto res = stm.solve(x, y);
		stream_cnt = res.second;
		cout << "\r" << "[streaming]\t" << res.first << " " << res.second << flush;
	}
	cout << endl;
	cout << "[streaming]\t" << stream_cnt << endl;
	// evaluation
	cout << "\terror " << (float(bf_cnt - stream_cnt) / bf_cnt * 100) << "%" << endl;
}


int main(int argc, char **argv) {
	srand(time(NULL));
	//makeFakeData(300);
	testTriangle();

    return 0;
}

