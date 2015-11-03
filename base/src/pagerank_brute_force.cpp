#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "pagerank_brute_force.h"
#include <algorithm>
#include <set>
#include <map>
#include <utility>
using namespace std;
using namespace sae::io;
typedef pair<vid_t , double > Pair;


pagerank_brute_force::pagerank_brute_force(MappedGraph *graph):
Solver(graph)
{}

pagerank_brute_force::~pagerank_brute_force(){
}
vector<double> bf_vec;
void bf_matrix_mult(vector<double> & a , const vector<vector<double>> & b){//向量乘矩阵
	bf_vec.clear();
	double q = 0.8;
	double sum;
	for (int i = 0; i < b.size(); ++ i){
		sum = (1-q)*1.0 / b.size();
		for (int j = 0; j < b[i].size(); ++ j )
			sum += q * b[j][i] * a[i];
		bf_vec.push_back(sum);
	}
	a = bf_vec;
}



vector<double> bf_matrix_run(vector<vector<double>> & matr , int a){//运行a次
	vector<double> pr_value;
	pr_value.clear();
	for (int i = 0; i < matr.size(); ++ i) pr_value.push_back(1.0 / matr.size());
	for (; a --; ){
		bf_matrix_mult(pr_value , matr);
	}
	return pr_value;

}
bool bf_cmp(const Pair &a , const Pair &b){
	return a.second < b.second;
}


vector<Pair> pagerank_brute_force::solve(string str){
	map<pair<vid_t , vid_t> , bool > edges;
	for (auto iter = graph->Edges(); iter -> Alive(); iter -> Next()){
		vid_t x = iter -> Source() -> GlobalId();
		vid_t y = iter -> Target() -> GlobalId();
		if (x == y) continue;
		if (x > y) swap(x , y);
		if (edges.find(make_pair(x , y)) == edges.end() ){
			edges[make_pair(x , y)] = true;
		}
	}
	map<vid_t , size_t> node_map;
	vector<vector<vid_t>> node_edge;
	vector<vid_t> row_vec;

	for (auto iter = edges.begin(); iter != edges.end(); ++ iter){
		vid_t x = iter->first.first;
		vid_t y = iter->first.second;
		if (node_map.find(x) == node_map.end()){
			node_map[x] = node_edge.size();
			row_vec.clear();
			row_vec.push_back(y);
			node_edge.push_back(row_vec);
		}  else node_edge[node_map[x]].push_back(y);
		if (node_map.find(y) == node_map.end()){
			node_map[y] = node_edge.size();
			row_vec.clear();
			row_vec.push_back(x);
			node_edge.push_back(row_vec);
		}  else node_edge[node_map[y]].push_back(x);
	}

	vector<vector<double>> Matr;
	vector<double> row;
	Matr.clear();
	row.clear();
	for (int i = 0; i < node_edge.size(); ++ i) row.push_back(0.0);
	for (int i = 0; i < node_edge.size(); ++ i){
		Matr.push_back(row);
		for (int j = 0; j < node_edge[i].size(); ++ j){
			vid_t x = node_edge[i][j];
			Matr[i][node_map[x]] = 1.0 / node_edge[i].size();
		}
	}
	auto value = bf_matrix_run(Matr , 200);
	vector<Pair> Ans;
	for (auto iter = node_map.begin(); iter != node_map.end(); ++ iter){
		Ans.push_back(make_pair(iter->first , value[iter->second]));//first为节点标号，second为pagerank值
	}
	sort(Ans.begin() , Ans.end() , bf_cmp);
	return Ans;
}
