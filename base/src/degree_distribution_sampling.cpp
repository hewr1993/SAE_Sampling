#include <iostream>
#include "degree_distribution_sampling.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <utility>


using namespace std;
using namespace sae::io;

degree_distribution_sampling::degree_distribution_sampling(MappedGraph *graph):
Solver(graph)
{}

degree_distribution_sampling::~degree_distribution_sampling(){}

bool cmp(double p , double q){
	return fabs(p - q) < 1e-5;
}

vector<pair<double , double>> degree_distribution_sampling::solve(double p , double q){
    	vector<double> node_degree;
	map<vid_t , size_t> node_flag;
	set<pair<vid_t , vid_t>> edges;
	for (auto iter = graph->Edges(); iter->Alive(); iter -> Next())
	{	
		vid_t a = iter -> Source() ->GlobalId();
		vid_t b = iter -> Target() ->GlobalId();
		if (a == b) continue;
		if (a > b) swap(a , b);
		auto key = make_pair(a , b);
		double r = q;
		if (edges.find(key) != edges.end()) continue;
		edges.insert(key);
		int has_a = (node_flag.find(a) != node_flag.end());
		int has_b = (node_flag.find(b) != node_flag.end());
		if (has_a + has_b == 0) 
			r = p;
		double coin = 1.0 * rand()/RAND_MAX;
		if (coin <= r){
			if (has_a == 0){
				node_flag.insert(make_pair(a , node_degree.size()));
				node_degree.push_back(1.0 / r);
			}	else node_degree[node_flag[a]] += 1.0/r;
			if (has_b == 0){
				node_flag.insert(make_pair(b , node_degree.size()));
				node_degree.push_back(1.0 / r);
			}	else node_degree[node_flag[b]] += 1.0/r;
		}
	} 
	sort(node_degree.begin() , node_degree.end());
	vector<pair<double , double > > Ans;
	for (size_t start = 0; start < node_degree.size(); ){
		size_t end = start + 1;
		for (; end < node_degree.size() && cmp(node_degree[end], node_degree[start]); ++ end);
		Ans.push_back(make_pair(node_degree[start] , 1.0* (end-start)/node_degree.size())); 
		start = end;
	}
	return Ans;
}

vector<pair<double, double>> degree_distribution_sampling::solve(double p){
	return solve(p , p);
}
