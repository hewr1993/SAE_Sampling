#include "storage/mgraph.h"
#include "solver/solver.h"

class degree_distribution_brute_force:public sae::Solver<int>
{
public:
	degree_distribution_brute_force(sae::io::MappedGraph *graph);
	~degree_distribution_brute_force();
	std::vector<std::pair<double, double>> solve(std::string str);
};
