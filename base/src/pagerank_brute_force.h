#include "storage/mgraph.h"
#include "solver/solver.h"

class pagerank_brute_force:public sae::Solver<int>{
public:
	pagerank_brute_force(sae::io::MappedGraph *graph);
	~pagerank_brute_force();
	std::vector<std::pair<sae::io::vid_t , double>> solve(std::string str);

};
