#include "pagerank_one_step_sampling.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
using namespace std;
using namespace sae::io;

pagerank_one_step_sampling::pagerank_one_step_sampling(MappedGraph *graph):
Solver(graph){}

pagerank_one_step_sampling::~pagerank_one_step_sampling(){}

vector<double> o_row;

void one_step_matrix_mult(vector<double> & a , const vector<vector<double>> & b){
	double Ans;
	double q = 0.8;
	o_row.clear();
	int N = b.size();
	for (int i = 0; i < N; ++ i){
		Ans = (1-q)/N;
		for (int j = 0; j < N; ++j)
			Ans +=q * b[j][i] * a[j];
		o_row.push_back(Ans);
	}
	a = o_row;
}

vector<double> one_step_matrix_run(vector<vector<double> >& Matr , int a){
	vector<double> Ans;
	for (int i = 0; i < Matr.size(); ++ i)
		Ans.push_back(1.0/Matr.size());
	for (; a--; )	one_step_matrix_mult(Ans , Matr);
	return Ans;
}

bool one_step_cmp(const pair<vid_t , double> &a , const pair<vid_t , double> & b){
	return a.second < b. second;
}


vector<pair<vid_t , double>> one_bfs(map<vid_t , size_t>& edg_map ,vector<double> &ans , MappedGraph *graph)
{
	set<pair<vid_t , vid_t>> edges;
	vector<vector<vid_t>> edge_list;
	map<vid_t , size_t> edge_pos;
	vector<vid_t> row_vec;
	for (auto iter = graph->Edges(); iter ->Alive(); iter ->Next()){
		vid_t x = iter -> Source() -> GlobalId();
		vid_t y = iter -> Target() -> GlobalId();
		if (x == y) continue;
		if (x > y) swap(x , y);
		if (edges.find(make_pair(x , y)) != edges.end()) continue;
		edges.insert(make_pair(x , y));
		if (edge_pos.find(x) == edge_pos.end()) {
			edge_pos[x] = edge_list.size();
			row_vec.clear();
			row_vec.push_back(y);
			edge_list.push_back(row_vec);
		}	else edge_list[edge_pos[x]].push_back(y); 
		if (edge_pos.find(y) == edge_pos.end()){
			edge_pos[y] = edge_list.size();
			row_vec.clear();
			row_vec.push_back(x);
			edge_list.push_back(row_vec);
		}       else edge_list[edge_pos[y]].push_back(x);
	}
	map<vid_t , double > value_map;
	set<vid_t> visited;
	for (auto iter = edg_map.begin(); iter != edg_map.end(); ++ iter){
		visited.insert(iter->first);
		value_map[iter->first] = ans[iter->second];
	}
	for (auto iter = edg_map.begin(); iter != edg_map.end(); ++ iter){
		vid_t x = iter->first;
		for (int j = 0; j < edge_list[edge_pos[x]].size(); ++ j){
			if (visited.find(edge_list[edge_pos[x]][j]) == visited.end()){
				vid_t y = edge_list[edge_pos[x]][j];
				if (value_map.find(y) == value_map.end()) {
					value_map[y] = value_map[x]/edge_list[edge_pos[x]].size();
				}	else value_map[y] += value_map[x]/edge_list[edge_pos[x]].size();
			}
		}
	}
	vector<pair<vid_t , double>> ans_pair;
	for (auto iter = value_map.begin(); iter != value_map.end(); ++ iter){
			ans_pair.push_back(make_pair(iter->first,iter->second));
		}
	sort(ans_pair.begin() , ans_pair.end() , one_step_cmp);
	return ans_pair;
}

vector<pair<vid_t , double>> pagerank_one_step_sampling::solve(double p , double q){

    map<pair<vid_t, vid_t>, bool> edges;
    for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
        vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
        if (edges.find(make_pair(x, y)) == edges.end()) {
                edges[make_pair(x, y)] = true;
                edges[make_pair(y, x)] = true;
        }
    }
    vector<vid_t> row_vec;
    vector<vector<vid_t> > edge_list;
    edge_list.clear();
    map<vid_t , size_t> edg_map;
    srand(time(0));
  int cnt = 0;
    for (auto iter = edges.begin(); iter != edges.end(); ++ iter){
        auto edge = iter->first;
        vid_t a = edge.first;
        vid_t b = edge.second;
        if (a >= b) continue;
        double r = p;
        
        int has_a = (edg_map.find(a) != edg_map.end());
        int has_b = (edg_map.find(b) != edg_map.end());
        if (has_a + has_b == 0)
                r = q;
       
        double coin = 1.0 * rand()/RAND_MAX;
        if (coin <= r){
	    if (has_a) edge_list[edg_map[a]].push_back(b);
                else {
		    cnt ++;
                    edg_map[a] = edge_list.size();
                    row_vec.clear();
                    row_vec.push_back(b);
                    edge_list.push_back(row_vec);
                }
            if (has_b) edge_list[edg_map[b]].push_back(a);
                else {
		    cnt ++;
                    edg_map[b] = edge_list.size();
                    row_vec.clear();
                    row_vec.push_back(a);
                    edge_list.push_back(row_vec);
                }
        }
    }
    cout << cnt  << endl;
    vector<vector<double> > Matr;
    Matr.clear();
    
    vector<double> matr_row_vec;
    for (size_t j = 0; j < edge_list.size(); ++ j)
            matr_row_vec.push_back(0.0);
    for (size_t i = 0; i < edge_list.size(); ++ i)
    {
        Matr.push_back(matr_row_vec);
        for (size_t j = 0; j < edge_list[i].size(); ++ j)
            Matr[i][edg_map[edge_list[i][j]]] = 1.0 / edge_list[i].size() ;
    }

    vector<double> Ans;
    Ans = one_step_matrix_run(Matr , 50); 
    return one_bfs(edg_map , Ans , graph);

}

vector<pair<vid_t , double >> pagerank_one_step_sampling::solve(double p){
	return solve(p , p);
}
