#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "degree_distribution_brute_force.h"
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <algorithm>
using namespace std;
using namespace sae::io;

degree_distribution_brute_force::degree_distribution_brute_force(MappedGraph *graph):
Solver(graph)
{}

degree_distribution_brute_force::~degree_distribution_brute_force(){}

bool equal(double p , double q ){
	return fabs(p - q) < 1e-5;
}

vector<pair<double , double>> degree_distribution_brute_force::solve(string str){
	set<pair<vid_t , vid_t>> edges;
	map<vid_t , size_t> node_flag;
	vector<double> node_degree;
	for (auto iter = graph->Edges(); iter->Alive(); iter->Next()){
	     vid_t a = iter -> Source() -> GlobalId();
	     vid_t b = iter -> Target() -> GlobalId();
	     if (a == b) continue;
	     if (a > b) swap(a , b);
	     if (edges.find(make_pair(a , b)) != edges.end()) continue;
	     edges.insert(make_pair(a , b));
	     if (node_flag.find(a) == node_flag.end()) {
			node_flag[a] = node_degree.size();
			node_degree.push_back(1.0);
		}	else node_degree[node_flag[a]] += 1.0;
	     if (node_flag.find(b) == node_flag.end()) {
			node_flag[b] = node_degree.size();
			node_degree.push_back(1.0);
		}	else node_degree[node_flag[b]] += 1.0;
	}
	vector<pair<double , double> > Ans; //first为度，second为节点百分比
	sort(node_degree.begin() , node_degree.end() );
	for (size_t start = 0; start < node_degree.size(); ){
		size_t end = start + 1;
		for ( ; end < node_degree.size() && equal(node_degree[end],node_degree[start]) ; ++ end);
		Ans.push_back(make_pair(1.0*node_degree[start] ,1.0* (end - start)/ node_degree.size() ));
		start = end;
	}
	return Ans;

}
