/*************************************************************************
	> Author: Wayne Ho
	> Purpose: brute-force counting triangles
	> Created Time: Fri Jul 17 01:08:33 2015
	> Mail: hewr2010@gmail.com 
 ************************************************************************/
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

vector<pair<int, int> > edges;
map<pair<int, int>, bool> pool;
int n, m;

int main(int argc, char **argv) {
	scanf("%d%d", &n, &m);
	for (int i = 0; i < m; ++i) {
		int x, y;
		scanf("%d%d", &x, &y);
		auto key = make_pair(x, y);
		pool[key] = true;
		edges.push_back(key);
	}
	sort(edges.begin(), edges.end());
    // count triangles
    int cnt(0);
    for (int start = 0; start < edges.size(); ) {
        int end = start;
        while (end < edges.size() && edges[end].first == edges[start].first) ++end;
        for (int i = start; i != end; ++i) for (int j = i + 1; j != end; ++j) {
            auto key = make_pair(edges[i].second, edges[j].second);
            if (pool.find(key) != pool.end()) ++cnt;
        }
        start = end;
    }
	printf("%d\n", cnt);
    return 0;
}

