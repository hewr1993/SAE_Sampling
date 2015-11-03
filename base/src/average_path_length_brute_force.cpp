#include "average_path_length_brute_force.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <utility>

using namespace std;
using namespace sae::io;

average_path_length_brute_force::average_path_length_brute_force(MappedGraph *graph):
Solver(graph)
{}

average_path_length_brute_force::~average_path_length_brute_force(){}
const int infi = 1e7+9;
map<vid_t , size_t> node_flag;
vector<vector<vid_t>> node_vec;//边表
vector<vid_t> row_vec;
vector<vid_t> node_name;
set<vid_t> visited;//是否访问
vector<vid_t> deq[2];//当前节点，下一次访问节点
double dfs(vid_t a , vid_t b){  //名字写成了dfs，实际做法为bfs
	visited.clear();
	visited.insert(a);
	deq[0].clear() , deq[1].clear();
	int flag = 1;
	deq[0].push_back(a);
	for (int round = 1;round <= 1900; round ++){
		deq[flag].clear();
		flag = 1 - flag;
		if (deq[flag].size() == 0) break;
		for (size_t i = 0; i < deq[flag].size(); ++i )
			{
				size_t x = node_flag[deq[flag][i]];
				for (size_t j = 0; j < node_vec[x].size(); ++ j){
					if (visited.count(node_vec[x][j]) == 0){
						visited.insert(node_vec[x][j]);
						if (node_vec[x][j] == b) return round;
						deq[1-flag].push_back(node_vec[x][j]);
					}
				}
			}
	}
	return infi;
}

double average_path_length_brute_force::solve(string str){
	set<pair<vid_t , vid_t>> edges;
	node_flag.clear();
	node_vec.clear();
	node_name.clear();
	for (auto iter = graph->Edges(); iter->Alive(); iter->Next()){
		vid_t a = iter -> Source() -> GlobalId();
		vid_t b = iter -> Target() -> GlobalId();
		if (a == b) continue;
		if (a > b) swap(a , b);
		auto key = make_pair(a , b);
		if (edges.find(key) != edges.end()) continue;
		edges.insert(key);
		if (node_flag.find(a) == node_flag.end() ){
			node_flag.insert(make_pair(a , node_vec.size()));
			row_vec.clear();
			row_vec.push_back(b);
			node_vec.push_back(row_vec);
			node_name.push_back(a);
		} else node_vec[node_flag[a]].push_back(b);
		if (node_flag.find(b) == node_flag.end() ){
			node_flag.insert(make_pair(b , node_vec.size()));
			row_vec.clear();
			row_vec.push_back(a);
			node_name.push_back(b);
			node_vec.push_back(row_vec);
		} else node_vec[node_flag[b]].push_back(a);

	}
	srand(time(NULL));
	int run_time = 1000;//运算次数
	double Ans = 0; //距离之和
	for (int i = 0; i < run_time; ++ i){
		size_t x = 1, y = 1;
		while (x == y){
			x = rand() % node_name.size();
			y = rand() % node_name.size();
		}

		int tx =  dfs(node_name[x] , node_name[y]);
		if (tx == infi) {
			i --;
			continue;
		}
		Ans += tx;
	}
	return Ans / run_time;
}
