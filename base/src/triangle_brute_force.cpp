#include "triangle_brute_force.h"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
using namespace sae::io;

Triangle_Brute_Force::Triangle_Brute_Force(MappedGraph *graph)
	:Solver(graph) {
}

Triangle_Brute_Force::~Triangle_Brute_Force() {
}

int Triangle_Brute_Force::solve() {
	// remove duplicate edges
	map<pair<vid_t, vid_t>, bool> edges;
	for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
		vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
		if (edges.find(make_pair(x, y)) == edges.end()) {
			edges[make_pair(x, y)] = true;
			edges[make_pair(y, x)] = true;
		}
	}
	// restore list of edges
	vector<pair<vid_t, vid_t> > edge_list;
	for (auto itr = edges.begin(); itr != edges.end(); ++itr) {
		auto edge = itr->first;
		if (edge.first < edge.second) {
			edge_list.push_back(make_pair(edge.first, edge.second));
		}
	}
	sort(edge_list.begin(), edge_list.end());
	// count triangles
	int cnt(0);
	for (int start = 0; start < edge_list.size(); ) {
		int end = start;
		while (end < edge_list.size() && edge_list[end].first == edge_list[start].first) ++end;
		for (int i = start; i != end; ++i) for (int j = i + 1; j != end; ++j) {
			auto key = make_pair(edge_list[i].second, edge_list[j].second);
			if (edges.find(key) != edges.end()) ++cnt;
		}
		start = end;
	}
	return cnt;
}

bool cmp(pair<pair<vid_t, vid_t>, int> a, pair<pair<vid_t, vid_t>, int> b)
{
    if (a.first.first == b.first.first)
        return a.second > b.second;
    return a.first.first < b.first.first;
}

int Triangle_Brute_Force::solve(double k) {
	// remove duplicate edges
	map<pair<vid_t, vid_t>, bool> edges;
	for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
		vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
		if (edges.find(make_pair(x, y)) == edges.end()) {
			edges[make_pair(x, y)] = true;
			edges[make_pair(y, x)] = true;
		}
	}
	// restore list of edges
	vector<pair<pair<vid_t, vid_t>, int> > edge_list;
    vector<int> degree_list;
    for (auto itr = edges.begin(); itr != edges.end(); ++itr) {
        auto edge = itr -> first;
        int a = edge.first;
        int b = edge.second;
        for (unsigned int i = degree_list.size(); i <= a || i <= b; i ++)
            degree_list.push_back(0);
        degree_list[a] ++;
        degree_list[b] ++;
    }
	for (auto itr = edges.begin(); itr != edges.end(); ++itr) {
		auto edge = itr->first;
		if (edge.first < edge.second) {
			edge_list.push_back(make_pair(make_pair(edge.first, edge.second), degree_list[edge.first]));
		}
	}
	sort(edge_list.begin(), edge_list.end(), cmp);
	// count triangles
	int cnt(0);
	for (int start = 0; start < edge_list.size() * k; ) {
		int end = start;
		while (end < edge_list.size() * k && edge_list[end].first.first == edge_list[start].first.first) ++end;
		for (int i = start; i != end; ++i) for (int j = i + 1; j != end; ++j) {
			auto key = make_pair(edge_list[i].first.second, edge_list[j].first.second);
			if (edges.find(key) != edges.end()) ++cnt;
		}
		start = end;
	}
	return cnt;
}
