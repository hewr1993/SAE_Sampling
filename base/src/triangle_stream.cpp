/*************************************************************************
	> Author: Wayne Ho
	> Purpose: triangle streaming implementation
	> Created Time: Fri Apr 24 01:09:22 2015
	> Mail: hewr2010@gmail.com 
 ************************************************************************/
#include "triangle_stream.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <map>
using namespace std;
using namespace sae::io;
#define edge_avail(x) ((x)[0] + (x)[1] > 0)
#define wedge_avail(x) ((x)[0] + (x)[1] + (x)[2] > 0)

Triangle_Stream::Triangle_Stream(int se, int sw)
	:se(se), sw(sw) {
	edge_count = 0;
	edge_res = new vid_t*[se];
	for (int i = 0; i < se; ++i) {
		edge_res[i] = new vid_t[2];
		edge_res[i][0] = edge_res[i][1] = 0;
		// make sure [0] <= [1]
	}
	isClosed = new bool[sw];
	wedge_res = new vid_t*[sw];
	for (int i = 0; i < sw; ++i) {
		isClosed[i] = false;
		wedge_res[i] = new vid_t[3];
		wedge_res[i][0] = wedge_res[i][1] = wedge_res[i][2] = 0;
		// make sure [0] <= [2]
	}
	tot_wedges = 0;
}

Triangle_Stream::~Triangle_Stream() {
	delete[] edge_res, wedge_res, isClosed;
}

int Triangle_Stream::count(vid_t x, vid_t y) {
	++edge_count;
	if (x > y) swap(x, y); // save the trouble of index combinations
	update(x, y);
	float rho = 0;
	for (int i = 0; i < sw; ++i) rho += isClosed[i];
	rho /= sw;
	float K = rho * 3;
	float T = (rho * edge_count * edge_count / (se * (se - 1))) * tot_wedges;
	cout << "\r" << "[streaming]\t" << K << " " << T << flush;
	return T;
}

void Triangle_Stream::update(vid_t x, vid_t y) {
	for (int i = 0; i < sw; ++i) if (!isClosed[i] && wedge_avail(wedge_res[i]))
		isClosed[i] = (wedge_res[i][0] == x && wedge_res[i][2] == y);
	if ((float(rand())/RAND_MAX) >= 1 - pow(1 - 1. / edge_count, se)) return;
	int idx = rand() % se;
	edge_res[idx][0] = x, edge_res[idx][1] = y;
	map<vid_t, vector<vid_t> > childs;
	for (int i = 0; i < se; ++i) {
		childs[edge_res[i][0]].push_back(edge_res[i][1]);
		childs[edge_res[i][1]].push_back(edge_res[i][0]);
	}
	tot_wedges = 0;
	for (auto itr = childs.begin(); itr != childs.end(); ++itr) {
		int n = itr->second.size();
		tot_wedges += n * (n - 1) / 2;
	}
	int new_wedges = childs[x].size() +  childs[y].size() - 2;
	float q = float(new_wedges) / tot_wedges;
	for (int i = 0; i < sw; ++i) {
		if ((float(rand())/RAND_MAX) >= q) continue;
		int w = rand() % new_wedges;
		if (w < childs[x].size() - 1) {
			vid_t z = childs[x][w];
			wedge_res[i][0] = min(y, z), wedge_res[i][1] = x, wedge_res[i][2] = max(y, z);
		} else {
			vid_t z = childs[y][w - childs[x].size() + 1];
			wedge_res[i][0] = min(x, z), wedge_res[i][1] = y, wedge_res[i][2] = max(x, z);
		}
		isClosed[i] = false;
	}
}
