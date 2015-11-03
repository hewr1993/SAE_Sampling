#include "average_path_length_sampling.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <map>
#include <vector>
#include <utility>

using namespace std;
using namespace sae::io;
const infi1 = 1e9+7;
average_path_length_sampling::average_path_length_sampling(MappedGraph *graph):
Solver(graph)
{}

average_path_length_sampling::~average_path_length_sampling(){}

map<vid_t , size_t> node_corre;//节点在边表中位置
vector<vector<vid_t>> Node_vec;//边表
vector<vid_t> Row;
vector<vid_t> Name;
vector<vid_t> que[2];//当前节点、下一次遍历节点
set<vid_t> exist;//该节点是否存在

double sampling_dfs(vid_t a , vid_t b){//实际上实现为bfs
		que[0].clear() , que[1].clear();
		exist.clear();
		exist.insert(a);
		que[0].push_back(a);
		int flag = 1;
		for (int round = 1; ; round++){
			que[flag].clear();
			flag = 1- flag;
			if (que[flag].size() == 0) break;
			for (int i = 0; i < que[flag].size(); ++ i){
				size_t x = node_corre[que[flag][i]];
				for (int j = 0; j < Node_vec[x].size(); ++ j )
					if (exist.count(Node_vec[x][j]) == 0){
							exist.insert(Node_vec[x][j]);
							que[1-flag].push_back(Node_vec[x][j]);
							if (Node_vec[x][j] == b)
								return round;
						}
			}
		}
		return 1901;
}


double average_path_length_sampling::solve(double p , double q){
	set<pair<vid_t , vid_t>> edges;
	node_corre.clear();
	Node_vec.clear();
	Name.clear();
	srand(time(NULL));
	for (auto iter = graph->Edges(); iter -> Alive(); iter->Next()){
		vid_t a = iter -> Source() -> GlobalId();
		vid_t b = iter -> Target() -> GlobalId();
		if (a == b) continue;
		if (a > b) swap(a , b);
		auto key = make_pair(a , b);
		if (edges.find(key) != edges.end()) continue;
		edges.insert(key);
		double r = q;
		int has_a = (node_corre.find(a) != node_corre.end());
		int has_b = (node_corre.find(b) != node_corre.end());
		double coin = rand() * 1.0 / RAND_MAX;
		if (coin <= r){
			if (has_a == 0){
				node_corre[a] = Node_vec.size();
				Name.push_back(a);
				Row.clear();
				Row.push_back(b);
				Node_vec.push_back(Row);
			}	else
				Node_vec[node_corre[a]].push_back(b);

			if (has_b == 0){
				node_corre[b] = Node_vec.size();
				Name.push_back(b);
				Row.clear();
				Row.push_back(a);
				Node_vec.push_back(Row);
			}	else
				Node_vec[node_corre[b]].push_back(a);
		}
	}
	double Ans = 0;
	int run_time = 1000;
	for (int i = 0; i < run_time; ++ i)
	{
		size_t x = 1 , y = 1;
		while (x == y){
			x = rand() % Name.size();
			y = rand() % Name.size();
		}
		int len = sampling_dfs(Name[x] , Name[y]);
		if (len == infi1){ //防止两点之间不存在路径
			i --;
			continue;
		}
		Ans += len;
	}
	return Ans/run_time;
}

double average_path_length_sampling::solve(double p){
	return solve(p , p);
}
