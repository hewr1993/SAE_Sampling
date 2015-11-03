#include "storage/mgraph.h"
#include "solver/solver.h"

class pagerank_sampling:public sae::Solver<int> {
public:
	pagerank_sampling(sae::io::MappedGraph *graph);
	~pagerank_sampling();
    std::vector<std::pair<sae::io::vid_t , double> > solve(double p, double q);
    std::vector<std::pair<sae::io::vid_t , double> > solve(double q);
};
