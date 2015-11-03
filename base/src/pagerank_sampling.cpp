#include "pagerank_sampling.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
using namespace sae::io;
typedef pair<vid_t , double> PAIR;

pagerank_sampling::pagerank_sampling(MappedGraph *graph)
    :Solver(graph)
    {
    }
pagerank_sampling::~pagerank_sampling()
{
}
vector<vector<double>> c;
vector<double> c_row;

void matrix_mult(vector<double> & a , const vector<vector<double>> & b){
	double Ans;
	double q = 0.8;
	c_row.clear();
	int N = b.size();
	for (int i = 0; i < N; ++ i){
		Ans = (1-q)/N;
		for (int j = 0; j < N; ++j)
			Ans +=q * b[j][i] * a[j];
		c_row.push_back(Ans);
	}
	a = c_row;
}

vector<double>  matrix_run(vector<vector<double> >& Matr , int a){
	vector<double> Ans;
	for (int i = 0; i < Matr.size(); ++ i)
		Ans.push_back(1.0/Matr.size());
	for (; a--; )	matrix_mult(Ans , Matr);
	return Ans;
}

bool pr_cmp(const PAIR &a , const PAIR & b){
	return a.second < b. second;
}

vector<pair<vid_t , double>> bfs(map<vid_t , size_t> & edg_map , vector<double> & ans , MappedGraph *graph){
	set<vid_t> visited;
	map<pair<vid_t , vid_t> , bool> edges;
	map<vid_t , size_t> node_map;
	vector<vid_t> row_vec;
	vector<vector<vid_t>> node_vec;
	for (auto iter = graph->Edges(); iter->Alive(); iter -> Next()){
		vid_t x = iter -> Source() -> GlobalId();
		vid_t y = iter -> Target() -> GlobalId();
		if (x == y) continue;
		if (x > y) swap(x , y);
		if (edges.find(make_pair(x , y)) == edges.end()){
			edges[make_pair(x , y)] = true;
			if (node_map.find(x) == node_map.end()){
				node_map[x] = node_vec.size();
				row_vec.clear();
				row_vec.push_back(y);
				node_vec.push_back(row_vec);	
			}	else  node_vec[node_map[x]].push_back(y);
			if (node_map.find(y) == node_map.end()){
				node_map[y] = node_vec.size();
				row_vec.clear();
				row_vec.push_back(x);
				node_vec.push_back(row_vec);
			}	else node_vec[node_map[y]].push_back(x);
				
		}
	}
	map<vid_t , double> node_pr_value;
	vector<vid_t> deq[2];
	visited.clear();
	deq[0].clear() , deq[1].clear();
	for (auto iter = edg_map.begin(); iter != edg_map.end(); iter ++){
		deq[0].push_back(iter->first);
		visited.insert(iter->first);
		node_pr_value.insert(make_pair(iter->first , ans[iter->second]));
	}
	int flag = 0;
	while (deq[flag].size() != 0){
		deq[1-flag].clear();
		for (int i = 0; i < deq[flag].size(); ++ i){
			vid_t x = deq[flag][i];
			for (int j = 0; j < node_vec[node_map[x]].size(); ++ j)
				{
				vid_t y = node_vec[node_map[x]][j];
				if (visited.find(y) == visited.end())
					{
						if (node_pr_value.find(y) != node_pr_value.end()){
							deq[1-flag].push_back(y);
							node_pr_value[y] = node_pr_value[x] * 1.0 / node_vec[node_map[x]].size();
						}	else node_pr_value[y] += node_pr_value[x] * 1.0 / node_vec[node_map[x]].size();
						
					}
				}
		}
	       flag = 1 - flag;
	       for (int i = 0 ; i < deq[flag].size(); ++ i){
			visited.insert(deq[flag][i]);
		}
	}
	vector<pair<vid_t , double>> node_ans;
	for (auto iter = node_pr_value.begin(); iter != node_pr_value.end(); ++ iter){
		node_ans.push_back(make_pair(iter->first , iter -> second));
	}
	sort(node_ans.begin() , node_ans.end() , pr_cmp);
	return node_ans;
}



vector<pair<vid_t, double>> pagerank_sampling::solve(double p , double q){
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
            Matr[i][edg_map[edge_list[i][j]]] = 1.0/edge_list[i].size() ;
    }

    vector<double> Ans;
    Ans = matrix_run(Matr , 50);
   return bfs(edg_map , Ans , graph);
}


vector<pair<vid_t, double> > pagerank_sampling::solve(double p){
    return solve(p , p);
}
