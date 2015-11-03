#include "average_degree_brute_force.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <utility>
#include <set>

using namespace std;
using namespace sae::io;

average_degree_brute_force::average_degree_brute_force(MappedGraph * graph):
Solver(graph)
{}

average_degree_brute_force::~average_degree_brute_force(){}

double average_degree_brute_force::solve(string str){
	set<vid_t> nodes;
	set<pair<vid_t , vid_t>> edges;
	double edge_sum = 0 , node_sum = 0; //平均度 = 边数 edge_sum *2 / 点数node_sum
	for (auto iter = graph->Edges(); iter->Alive(); iter->Next())
	{
		vid_t a = iter-> Source() -> GlobalId();
		vid_t b = iter-> Target() -> GlobalId();
		if (a == b) continue;
		if (a > b) swap(a , b);
		auto key = make_pair(a,b);
		if (edges.find(key) == edges.end()){
			edges.insert(key);
			edge_sum ++;
			if (nodes.count(a) == 0) {
					node_sum ++;
					nodes.insert(a);
				}
			if (nodes.count(b) == 0){
					node_sum ++;
					nodes.insert(b);
				}
		}
	}
	return 2.0 * edge_sum /node_sum;
}
