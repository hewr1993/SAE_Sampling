#include "storage/mgraph.h"
#include "solver/solver.h"

class length2_brute_force:public sae::Solver<int> {
public:
	length2_brute_force(sae::io::MappedGraph *graph);
	~length2_brute_force();
	double solve(std::string str);
};


