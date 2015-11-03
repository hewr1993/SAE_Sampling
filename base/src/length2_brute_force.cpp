#include <iostream>
#include "length2_brute_force.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cstring>

using namespace std;
using namespace sae::io;
typedef long long LL;
typedef pair<vid_t , vid_t> PAVT;
length2_brute_force::length2_brute_force(MappedGraph *graph):Solver(graph)
{

}

length2_brute_force::~length2_brute_force()
{

}

double length2_brute_force::solve(std::string str){
    vector<double> same_node_sum;
    same_node_sum.clear();
    map<vid_t , size_t > node_map;
    set<PAVT> edg_set;
    double Ans = 0;
    for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
		vid_t x = itr->Source()->GlobalId();
		vid_t y = itr->Target()->GlobalId();
		if (x == y) continue;
		if (x > y) swap(x , y);
		if ( edg_set.find(make_pair(x , y)) != edg_set.end() ) continue;
		edg_set.insert(make_pair(x , y));

		if (node_map.find(x) == node_map.end()){
	            node_map.insert(make_pair(x , same_node_sum.size()));
       		     same_node_sum.push_back(1.0);
		}   else {
         		   size_t cnt = node_map[x];
           		   Ans += same_node_sum[cnt];
           		   same_node_sum[cnt] ++;
		     	}

		if (node_map.find(y) == node_map.end()){
           	 node_map.insert(make_pair(y ,  same_node_sum.size()));
           	 same_node_sum.push_back(1.0);
		}   else {
         		   size_t cnt = node_map[y];
         		   Ans += same_node_sum[cnt];
                   same_node_sum[cnt] ++;
	         	}
    }
    return Ans;
    

}
