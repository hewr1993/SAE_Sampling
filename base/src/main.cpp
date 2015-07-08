/*************************************************************************
	> Author: Wayne Ho
	> Purpose: TODO
	> Created Time: Fri Apr 17 16:32:22 2015
	> Mail: hewr2010@gmail.com 
 ************************************************************************/
#include "argparse/macro-argparse-jquery.hh"
#include "solver/solver.h"
#include "triangle_sampling.h"
#include "eigen_triangle.h"
#include "triangle_brute_force.h"
#include "triangle_stream.h"
#include "storage/graph_builder.h"
#include "storage/mgraph.h"
#include "report/table_generator.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <map>
#include <cstdlib>

using namespace std;
using namespace sae::io;

DEF_ARGUMENT_CLASS(
	Argument,
	string,		content,	"test",		OPTIONAL,	OPT_SLH(-c, --content, "what to say"),
	//int,		number,		1,			REQUIRED,	OPT_LH(-n, "what number is it? "),
	bool,		decorate,	true,		OPTIONAL,	OPT_SLWH(-d, --decorate, true, "decoreate the output")
);


void makeFakeData(int numVertex=10, double p = 0.1) {
	//int numEdge = rand() % (numVertex * numVertex / 3) + numVertex;
    int numEdge = (numVertex * (numVertex - 1)) / 2 * p;
	GraphBuilder<int> graph;
	for (int i = 0; i < numVertex; ++i) graph.AddVertex(i, 0);
	map<pair<int, int>, bool> edges;
	for (int i = 0; i < numEdge; ++i) {
		pair<int, int> edge;
		do {
			int x = rand() % (numVertex - 1);
			int y = rand() % (numVertex - x - 1) + x + 1;
			edge = make_pair(x, y);
		} while (edges.find(edge) != edges.end());
		edges[edge] = true;
		graph.AddEdge(edge.first, edge.second, 0);
		//cout << edge.first << " " << edge.second << endl;
	}
	system("mkdir -p fake");
	graph.Save("./fake/graph");
}

void testTriangle(char* prefix="../data/data/gplus") {
    // prepare graph
    cout << "Loading data from " << prefix << endl;
	MappedGraph *graph;
	graph = MappedGraph::Open(prefix);

    // set up table
	TableGenerator table;
	string title[] = {"Algo.", "Res.", "Error (%)", "Time (sec.)"};
	table.setTitle(vector<string>(title, title + sizeof(title) / sizeof(title[0])));
    vector<string> bf_row;
    bf_row.push_back("EdgeIterator");
    vector<string> sampling_row;
    sampling_row.push_back("SamplingTriangle");
    vector<string> greedy_row;
    greedy_row.push_back("Greedy");
    vector<string> eigen_row;
    eigen_row.push_back("EigenTriangle");
    vector<string> stm_row;
    stm_row.push_back("Streaming");
    //score.push_back(toString<int>(graph->VertexCount()));
    //score.push_back(toString<int>(graph->EdgeCount()));

    cout << "===== graph information =====" << endl;
    cout << "#vertices: " << graph -> VertexCount() << endl;
    cout << "#edges: " << graph -> EdgeCount() << endl;
    cout << "=============================" << endl;
	
    // brute force
    Triangle_Brute_Force bf(graph);
    time_t bf_start_time = clock();
    int bf_cnt = 1073677742; //bf.solve();
    time_t bf_end_time = clock();
    cout << "[EdgeIteartor]\t" << bf_cnt << endl;
    double bf_error = 0.0;
    bf_row.push_back(toString<int>(bf_cnt));
    bf_row.push_back(toString<double>(bf_error));
    //bf_row.push_back(toString<double>((bf_end_time - bf_start_time + 0.0) / CLOCKS_PER_SEC));
    bf_row.push_back("6834.6");

    // sampling 
    Triangle_Sampling sampling(graph);
    double p = 0.9;
    double q = 0.9;
    time_t sampling_start_time = clock();
    double sampling_cnt = sampling.solve(p, q);
    time_t sampling_end_time = clock();
    cout << "[sampling]\t" << sampling_cnt << endl;
    double sampling_error = (bf_cnt - sampling_cnt) / bf_cnt * 100;
    sampling_row.push_back(toString<double>(sampling_cnt));
    sampling_row.push_back(toString<double>(sampling_error));
    sampling_row.push_back(toString<double>((sampling_end_time - sampling_start_time + 0.0) / CLOCKS_PER_SEC));

    // greedy
    time_t greedy_start_time = clock();
    int greedy_cnt = 0;//bf.solve(0.8);
    time_t greedy_end_time = clock();
    cout << "[greedy]\t" << greedy_cnt << endl;
    double greedy_error = double(bf_cnt - greedy_cnt) / bf_cnt * 100;
    greedy_row.push_back(toString<int>(greedy_cnt));
    greedy_row.push_back(toString<double>(greedy_error));
    greedy_row.push_back(toString<double>((greedy_end_time - greedy_start_time + 0.0) / CLOCKS_PER_SEC));

    // eigen
    //EigenTriangle et(graph);
    time_t et_start_time = clock();
    double k = 0.005;
    double tol = 10;
    double et_cnt = 0;//et.solve(k /*30 graph -> VertexCount() * k, tol*/);
    //double et_cnt = et.solve((int) (graph -> VertexCount() * k));
    time_t et_end_time = clock();
    cout << "[eigen triangle]\t" << et_cnt << endl;
    double eigen_error = double(bf_cnt - et_cnt) / bf_cnt * 100;
    eigen_row.push_back(toString<int>(int(et_cnt)));
    eigen_row.push_back(toString<double>(double(eigen_error)));
    eigen_row.push_back(toString<double>((et_end_time - et_start_time + 0.0) / CLOCKS_PER_SEC));

    // streaming
    time_t stm_start_time = clock();
	Triangle_Stream stm(10000, 10000);
	int stream_cnt(0);
	for (auto itr = graph->Edges(); itr->Alive(); itr->Next()) {
		vid_t x = itr->Source()->GlobalId(), y = itr->Target()->GlobalId();
		auto res = stm.solve(x, y);
		stream_cnt = res.second;
		cout << "\r" << "[streaming]\t" << res.first << " " << res.second << flush;
        break;
	}
    time_t stm_end_time = clock();
	cout << endl;
	cout << "[streaming]\t" << stream_cnt << endl;
    double stm_error = double(bf_cnt - stream_cnt) / bf_cnt * 100;
    stm_row.push_back(toString<int>(stream_cnt));
    stm_row.push_back(toString<double>(stm_error));
    stm_row.push_back(toString<double>((stm_end_time - stm_start_time + 0.0) / CLOCKS_PER_SEC));
    /*
    cout << "\terror " << (float(bf_cnt - stream_cnt) / bf_cnt * 100) << "%" << endl;
    cout << "\terror " << (float(bf_cnt - et_cnt) / bf_cnt * 100) << "%" << endl;
    */
    // report table
    table.addRow(bf_row);
    table.addRow(sampling_row);
    table.addRow(greedy_row);
    table.addRow(eigen_row);
    table.addRow(stm_row);
    string tableContent = table.report();
    cout << tableContent << endl;
    ofstream fout("report.md");
    fout << tableContent << endl;
    fout.close();
    graph -> Close();
}


void testTable() {
	TableGenerator table;
	string title[] = {"z", "y", "x"};
	table.setTitle(vector<string>(title, title + sizeof(title) / sizeof(title[0])));
	for (int l = 0; l < rand() % 10 + 1; ++l) {
		vector<string> content;
		content.push_back(toString(l));
		for (int i = 0; i < rand() % 10 + 1; ++i) content.push_back(toString(rand() % 1000));
		table.addRow(content);
	}
	cout << table.report() << endl;
}


int main(int argc, char **argv) {
    int vertexNum = 100;
    double edgeProb = 0.7;
	// parse arguments
	Argument args;
	if (!args.parse_args(argc, argv)) 
        return 1;
	//cout << args.content() << endl;
	
	// main process
	srand(time(NULL));
	//testTable();
	makeFakeData(vertexNum, edgeProb);
	testTriangle();
    return 0;
}

