#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "length2_sampling.h"
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <utility>

using namespace std;
using namespace sae::io;

typedef pair<vid_t , vid_t> PAVT;
length2_sampling::length2_sampling(MappedGraph *graph)
	:Solver(graph)
{
}

length2_sampling::~length2_sampling()
{
}
double Random1()
{
    return 1.0 * rand() / RAND_MAX;
}

double length2_sampling::solve(double p , double q){
    map<pair<vid_t, vid_t>, bool> edges;
    for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
        vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
        if (x == y) continue;
	if (x > y) swap(x , y);
	if (edges.find(make_pair(x, y)) == edges.end()) {
                edges[make_pair(x, y)] = true;
        }
    }
    srand(time(0));
    double Ans = 0;
    set<vid_t> node_set;
    vector<double> same_node_sum;
    map<vid_t , size_t> node_map;
    same_node_sum.clear();
    int sum = 0;
    for (auto itr = edges.begin(); itr != edges.end(); ++itr) {
        auto edge = itr -> first;
        vid_t a = edge.first;
        vid_t b = edge.second;
        if (a >= b)  continue;
        edge = make_pair(a, b);
        double r = q;
        int has_a = 1;
        int has_b = 1;
   
            has_a = node_set.count(a);
            has_b = node_set.count(b);
            if (has_a + has_b == 0)
                r = p;
        
        double coin = Random1();
	if (coin <= r ){
	    sum ++;
            if (has_a == 0){
                node_set.insert(a);
                node_map.insert(make_pair( a , same_node_sum.size()));
                same_node_sum.push_back(1.0/r);
            }   else {
                Ans += (1.0 / r ) * same_node_sum[node_map[a]];
                same_node_sum[node_map[a]] += 1.0/r;
            }
            if (has_b == 0){
                node_set.insert(b);
                node_map.insert(make_pair(b ,  same_node_sum.size()));
                same_node_sum.push_back(1.0/r);
            }   else {
                Ans += (1.0 / r) * same_node_sum[node_map[b]];
                same_node_sum[node_map[b]] += 1.0/r;
            }
        }   
 }
    printf("%d edges have been sampled.\n" , sum);
    return Ans;
}

double length2_sampling::solve(double q){
    return solve(q , q);
}



