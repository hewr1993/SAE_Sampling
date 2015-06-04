#include "storage/mgraph.h"
#include "solver/solver.h"

class Triangle_Brute_Force:public sae::Solver<int> {
public:
	Triangle_Brute_Force(sae::io::MappedGraph *graph);
	~Triangle_Brute_Force();
	int solve();
    int solve(double k);
private:
};
