#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include "pagerank_direct_sampling.h" 

using namespace std;
using namespace sae::io;

pagerank_direct_sampling::pagerank_direct_sampling(MappedGraph *graph):Solver(graph)
{}

pagerank_direct_sampling::~pagerank_direct_sampling(){}

vector<double> d_row;

void direct_matrix_mult(vector<double> & a , const vector<vector<double>> & b){
	double Ans;
	double q = 0.8;
	d_row.clear();
	int N = b.size();
	for (int i = 0; i < N; ++ i){
		Ans = (1-q)/N;
		for (int j = 0; j < N; ++j)
			Ans +=q * b[j][i] * a[j];
		d_row.push_back(Ans);
	}
	a = d_row;
}

vector<double> direct_matrix_run(vector<vector<double> >& Matr , int a){
	vector<double> Ans;
	for (int i = 0; i < Matr.size(); ++ i)
		Ans.push_back(1.0/Matr.size());
	for (; a--; )	direct_matrix_mult(Ans , Matr);
	return Ans;
}

bool pr_direct_cmp(const pair<vid_t , double> &a , const pair<vid_t , double> & b){
	return a.second < b. second;
}

vector<pair<vid_t , double>> pagerank_direct_sampling::solve(double p , double q){
	
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
    auto value =  direct_matrix_run(Matr , 200);
    
    vector<pair<vid_t , double>> ans;
    for (auto iter = edg_map.begin(); iter != edg_map.end(); ++ iter){
		ans.push_back(make_pair(iter->first , value[iter->second]));
	}
    sort(ans.begin() , ans.end() , pr_direct_cmp);
    return ans;
}
