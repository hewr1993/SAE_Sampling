
#include "storage/mgraph.h"
#include "solver/solver.h"

class length2_sampling:public sae::Solver<int>{
public:
    length2_sampling(sae::io::MappedGraph *graph);
    ~length2_sampling();
    double solve(double p , double q);
    double solve(double q);
};


