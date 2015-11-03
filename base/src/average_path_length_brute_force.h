#include "storage/mgraph.h"
#include "solver/solver.h"

class average_path_length_brute_force:public sae::Solver<int>{
public:
	average_path_length_brute_force(sae::io::MappedGraph *graph);
	~average_path_length_brute_force();
	double solve(std::string str);
};
