#ifndef __OC__
#define __OC__

#include <petsc.h>

class TopOpt;
class Filter;

class OC {
private:
	Vec tmp1, tmp2; // Temporary stuff
	Vec xcur;       // Current value of design variables
	Vec dv;         // Filtered gradient of the volume constraints

public:
	~OC();

	// Init internal vectors
	PetscErrorCode Init(TopOpt *opt);

	// Set and solve a subproblem: return new xval
	PetscErrorCode Update(TopOpt *opt, Filter *filter);
};

#endif