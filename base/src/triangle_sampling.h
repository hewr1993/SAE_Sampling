#include "storage/mgraph.h"
#include "solver/solver.h"

class Triangle_Sampling:public sae::Solver<int> {
public:
	Triangle_Sampling(sae::io::MappedGraph *graph);
	~Triangle_Sampling();
    int solve(double p, double q);
    int solve(double q);
private:
};
