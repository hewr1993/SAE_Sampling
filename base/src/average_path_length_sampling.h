#include "storage/mgraph.h"
#include "solver/solver.h"

class average_path_length_sampling:public sae::Solver<int> {
public:
	average_path_length_sampling(sae::io::MappedGraph *graph);
	~average_path_length_sampling();
	double solve(double p , double q);
	double solve(double p);
};
