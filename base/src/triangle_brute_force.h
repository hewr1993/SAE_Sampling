#include "storage/mgraph.h"
#include <map>

class Triangle_Brute_Force {
public:
	Triangle_Brute_Force(char* prefix);
	~Triangle_Brute_Force();
	int count(); // return exact number of triangles
private:
	sae::io::MappedGraph *graph;
};
