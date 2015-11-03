#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "average_degree_sampling.h"
#include <set>
#include <map>
#include <utility>
using namespace std;
using namespace sae::io;

average_degree_sampling::average_degree_sampling(MappedGraph *graph)
:Solver(graph)
{}

average_degree_sampling::~average_degree_sampling()
{}

double average_degree_sampling::solve(double p , double  q){
	map<vid_t , size_t> node_map;
	set<pair<vid_t , vid_t>> edges;
	srand(time(NULL));
	double node_sum = 0;
	double edge_sum = 0;
	double cnt = 0;
	for (auto iter = graph->Edges(); iter->Alive(); iter -> Next()){
		vid_t a = iter-> Source() -> GlobalId();
		vid_t b = iter-> Target() -> GlobalId();

		if (a == b) continue;
		if (a > b) swap(a , b);

		auto key = make_pair(a , b);
		if (edges.find(key) != edges.end()) continue;
		int has_a = (node_map.find(a) != node_map.end());
		int has_b = (node_map.find(b) != node_map.end());
		double r = q;
		if (has_a + has_b == 0) r = p;
		double coin = 1.0 * rand() / RAND_MAX;
		if (coin <= r ){
			edges.insert(key);
			edge_sum += 1.0 / r;
			cnt ++;
			if (has_a == 0)
				{
					node_map[a] = 1;
					node_sum ++;
				}
			if (has_b == 0)
				{
					node_map[b] = 1;
					node_sum++;
				}
		}
	}
	return 2.0*cnt/node_sum;
}


double average_degree_sampling::solve(double q){
	return solve(q , q);
}
