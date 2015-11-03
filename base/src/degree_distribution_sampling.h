#include "storage/mgraph.h"
#include "solver/solver.h"

class degree_distribution_sampling:public sae::Solver<int>{
public:
	degree_distribution_sampling(sae::io::MappedGraph * graph);
	~degree_distribution_sampling();
	std::vector<std::pair<double, double>>  solve(double p , double q );
	std::vector<std::pair<double,double>> solve(double p);
};
