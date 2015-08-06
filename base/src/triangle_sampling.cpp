#include "triangle_sampling.h"
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <utility>

using namespace std;
using namespace sae::io;

Triangle_Sampling::Triangle_Sampling(MappedGraph *graph)
	:Solver(graph) 
{
}

Triangle_Sampling::~Triangle_Sampling() 
{
}

double Random()
{
    return 1.0 * rand() / RAND_MAX;
}

bool    edge_pool_cmp(pair<pair<vid_t, vid_t>, double> a, pair<pair<vid_t, vid_t>, double> b)
{
    return a.first.first < b.first.first;
}

double Triangle_Sampling::solve(double p, double q) 
{
    // remove duplicate edges
    map<pair<vid_t, vid_t>, bool> edges;
    for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
        vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
        if (edges.find(make_pair(x, y)) == edges.end()) {
                edges[make_pair(x, y)] = true;
                edges[make_pair(y, x)] = true;
        }
    }
	// sampling edges
	vector<pair<pair<vid_t, vid_t>, double> > edge_pool;
    set<vid_t> node_set;
    map<pair<vid_t, vid_t>, int> edge_pool_map;
    srand(time(0));
	//for (auto itr = graph -> Edges(); itr -> Alive(); itr -> Next()) 
    for (auto itr = edges.begin(); itr != edges.end(); ++itr) {
        //vid_t a = itr -> Source() -> GlobalId();
        //vid_t b = itr -> Target() -> GlobalId();
        auto edge = itr -> first;
        vid_t a = edge.first;
        vid_t b = edge.second;
        if (a >= b)
            continue;
            //swap(a, b);
        edge = make_pair(a, b);
        double r = q;
        int has_a = 1;
        int has_b = 1;
        if (p != q)
        {
            has_a = node_set.count(a);
            has_b = node_set.count(b);
            if (has_a + has_b == 0)
                r = p;
        }
        double coin = Random();
        if (coin <= r)// && edge_pool_map.find(edge) == edge_pool_map.end())
        {
            int idx = (int) edge_pool.size();
            edge_pool.push_back(make_pair(edge, r));
            edge_pool_map.insert(make_pair(edge, idx));
            if (has_a == 0)
                node_set.insert(a);
            if (has_b == 0)
                node_set.insert(b);
        }
	}
    printf("%d edges has been sampled.\n", (int) edge_pool.size());

    // count triangles
	sort(edge_pool.begin(), edge_pool.end(), edge_pool_cmp);
	int cnt(0);
    double res = 0;
	for (int start = 0; start < edge_pool.size(); ) 
    {
		int end = start;
		while (end < edge_pool.size() && edge_pool[end].first.first == edge_pool[start].first.first) 
            ++end;
		for (int i = start; i != end; ++i) 
        {
            for (int j = i + 1; j != end; ++j) 
            {
                int a = edge_pool[i].first.second;
                int b = edge_pool[j].first.second;
                if (a > b)
                    swap(a, b);
			    auto key = make_pair(a, b);
                map<pair<vid_t, vid_t>, int>::iterator it = edge_pool_map.find(key);
			    if (it != edge_pool_map.end()) 
                {
                    ++cnt;
                    int k = it -> second;
                    double prob = edge_pool[i].second * edge_pool[j].second * edge_pool[k].second;
                    res += 1.0 / prob;
                    //printf("%.5lf %.5lf %.5lf %.5lf %.5lf\n", edge_pool[i].second, edge_pool[j].second, edge_pool[k].second, 1.0 / prob, res);
                }
		    }
        }
		start = end;
	}
    printf("sampling triangles: %d\n", cnt);
	return res;
}

double Triangle_Sampling::solve(double q) 
{
    return solve(q, q);
}

