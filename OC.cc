#include <OC.h>
#include <TopOpt.h>
#include <Filter.h>

OC::~OC() {
	VecDestroy(&tmp1);
	VecDestroy(&tmp2);
	VecDestroy(&xcur);
	VecDestroy(&dv);
}

PetscErrorCode OC::Init(TopOpt *opt) {
	VecDuplicate(opt->x, &tmp1);
	VecDuplicate(opt->x, &tmp2);
	VecDuplicate(opt->x, &xcur);
	VecDuplicate(opt->x, &dv);
	VecSet(opt->xmax, opt->Xmax);
	VecSet(opt->xmin, opt->Xmin);

	return 0;
}

PetscErrorCode OC::Update(TopOpt *opt, Filter *filter) {
	PetscErrorCode ierr = 0;

	// Current value of design variables
	VecCopy(opt->x, xcur);

	// Gradient of the volume constraint
	VecSet(opt->x, 1.0);
	ierr = filter->FilterProject(opt); CHKERRQ(ierr);
	VecCopy(opt->xPhys, dv);
	
	// Init stuff
	PetscScalar vol;
	PetscScalar vtarget = ((PetscScalar)opt->n)*opt->volfrac;

	PetscScalar l1 = 0;
	PetscScalar l2 = 1e9;
	PetscScalar move = 0.2;

	while ((l2-l1)/(l1+l2) > 1e-3) {
		PetscScalar lmid = 0.5*(l1+l2);

		VecPointwiseDivide(tmp2, opt->dfdx, dv);
		VecScale(tmp2, -1.0/lmid);
		VecSqrtAbs(tmp2);
		VecPointwiseMult(tmp2, tmp2, xcur); // = xcur .* sqrt(-opt->dfdx. / dv / lmid)

		VecCopy(xcur, tmp1);
		VecShift(tmp1, move); // = xcur+move
		VecPointwiseMin(tmp2, tmp1, tmp2); // = min(xcur+move, ...)
		VecPointwiseMin(tmp2, opt->xmax, tmp2); // = min(xmax, ...)

		VecCopy(xcur, tmp1);
		VecShift(tmp1, -move);
		VecPointwiseMax(tmp2, tmp1, tmp2); // = max(xcur+move, ...)
		VecPointwiseMax(opt->x, opt->xmin, tmp2); // = max(xmin, ...)

		// Retrieve corresponding xPhys
		ierr = filter->FilterProject(opt); CHKERRQ(ierr);

		VecSum(opt->xPhys, &vol);
		if (vol > vtarget) {
			l1 = lmid;
		} else {
			l2 = lmid;
		}
	}

	return ierr;
}
