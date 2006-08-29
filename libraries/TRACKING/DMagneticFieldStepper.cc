
#include <iostream>
#include <iomanip>
using namespace std;

#include "DMagneticFieldMap.h"
#include "DMagneticFieldStepper.h"

//-----------------------
// DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::DMagneticFieldStepper(const DMagneticFieldMap *bfield)
{
	this->bfield = bfield;
	q = 1.0;
	start_pos = pos = TVector3(0.0,0.0,0.0);
	start_mom = mom = TVector3(0.0,0.0,1.0);
	stepsize = 1.0; // in cm
	CalcDirs();
}

//-----------------------
// DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::DMagneticFieldStepper(const DMagneticFieldMap *bfield, double q, TVector3 *x, TVector3 *p)
{
	this->bfield = bfield;
	this->q = q;
	start_pos = pos = *x;
	start_mom = mom = *p;
	stepsize = 1.0; // in cm
}

//-----------------------
// ~DMagneticFieldStepper
//-----------------------
DMagneticFieldStepper::~DMagneticFieldStepper()
{

}
	
//-----------------------
// SetStartingParams
//-----------------------
jerror_t DMagneticFieldStepper::SetStartingParams(double q, TVector3 *x, TVector3 *p)
{
	this->q = q;
	start_pos = pos = *x;
	start_mom = mom = *p;
	CalcDirs();

	return NOERROR;
}

//-----------------------
// SetMagneticFieldMap
//-----------------------
jerror_t DMagneticFieldStepper::SetMagneticFieldMap(const DMagneticFieldMap *bfield)
{
	this->bfield = bfield;

	return NOERROR;
}

//-----------------------
// SetStepSize
//-----------------------
jerror_t DMagneticFieldStepper::SetStepSize(double step)
{
	this->stepsize = step;

	return NOERROR;
}

//-----------------------
// CalcDirs
//-----------------------
void DMagneticFieldStepper::CalcDirs(void)
{
	/// This just accesses the magnetic field map
	/// at the current position and passes it on
	/// to CalcDirs(TVector3 *B). See there for
	/// more details.

	// Get B-field
	const DBfieldPoint_t* tmp = bfield->getQuick(pos.x(), pos.y(), pos.z());
	TVector3 B(tmp->Bx, tmp->By, tmp->Bz);

	CalcDirs(&B);
}

//-----------------------
// CalcDirs
//-----------------------
void DMagneticFieldStepper::CalcDirs(TVector3 *B)
{
	/// Calculate the directions of the "natural coordinates"
	/// (aka reference trajectory coordinate system) in the
	/// lab frame using the current momentum and the specified
	/// magnetic field vector B. The results are left in the
	/// private member fields, copies of which may be obtained
	/// by a subsequent call to the GetDirs(...) method.

	// cross product of p and B (natural x-direction)
	xdir = mom.Cross(*B);
	xdir.SetMag(1.0);
	
	// cross product of B and pxB (natural y-direction)
	ydir = B->Cross(xdir);
	ydir.SetMag(1.0);
	
	// B-field is natural z-direction
	zdir = *B;
	zdir.SetMag(1.0);
}

//-----------------------
// Step
//-----------------------
jerror_t DMagneticFieldStepper::Step(TVector3 *newpos)
{
	/// Advance the track one step and return the new position
	
	// The idea here is to work in the coordinate system whose
	// axes point in directions defined in the following way:
	// z-axis is along direction of B-field
	// x-axis is in direction perpendicular to both B and p (particle momentum)
	// y-axis is then just cross product of z and x axes.
	//
	// These coordinates are referred to as the natural coordinates below.
	// The step is calculated based on moving along a perfect helix a distance
	// of "stepsize". This means that the new position will actually be
	// closer than stepsize to the current position (unless the magnetic field
	// is zero).
	
	// Get B-field
	const DBfieldPoint_t* tmp = bfield->getQuick(pos.x(), pos.y(), pos.z());
	TVector3 B(tmp->Bx, tmp->By, tmp->Bz);
//TVector3 B(0.0, 0.0,-2.0);

	// If the magnetic field is zero or the charge is zero, then our job is easy
	if(B.Mag()==0.0 || q==0.0){
		TVector3 pstep = mom;
		pstep.SetMag(stepsize);
		pos += pstep;
		if(newpos)*newpos = pos;

		return NOERROR;
	}
	
	// cosine of angle between p and B
	double theta = B.Angle(mom);
	double cos_theta = cos(theta);
	double sin_theta = sin(theta);

	// Note that xdir, ydir, and zdir should already be valid for the
	// starting point of this step. They are set via a call to CalcDirs(...)

	// delta_z is step size in z direction
	TVector3 delta_z = zdir*stepsize*cos_theta;
	
	// The ratio p/qB appears in a few places.
	double Rp = mom.Mag()/(q*B.Mag()*qBr2p); // qBr2p converts to GeV/c/cm so Rp will be in cm
	Ro = Rp*sin_theta;

	// delta_phi is angle of rotation in natural x/y plane
	double delta_phi = stepsize/Rp;

	// delta_x is magnitude of step in natural x direction
	TVector3 delta_x = Ro*(1.0-cos(delta_phi))*xdir;
	
	// delta_y is magnitude of step in natural y direction
	TVector3 delta_y = Ro*sin(delta_phi)*ydir;
	
	// Step to new position
	pos += delta_x + delta_y + delta_z;
	
	// Update momentum by rotating it by delta_phi about B
	mom.Rotate(-delta_phi, B);
	
	// Energy loss for 1.0GeV pions in Air is roughly 2.4keV/cm
//	double m = 0.13957;
//	double p = mom.Mag();
//	double E = sqrt(m*m + p*p) - 0.0000024*stepsize;
//	mom.SetMag(sqrt(E*E-m*m));
	
	// Calculate directions of natural coordinates at the new position
	CalcDirs(&B);
	
	// return new position 
	if(newpos)*newpos = pos;

	return NOERROR;
}

//-----------------------
// GetBField
//-----------------------
const DBfieldPoint_t* DMagneticFieldStepper::GetDBfieldPoint(void)
{
	return bfield->getQuick(pos.x(), pos.y(), pos.z());
}

//-----------------------
// GetDirs
//-----------------------
void DMagneticFieldStepper::GetDirs(TVector3 &xdir, TVector3 &ydir, TVector3 &zdir)
{
	xdir = this->xdir;
	ydir = this->ydir;
	zdir = this->zdir;
}

//-----------------------
// GetMomentum
//-----------------------
void DMagneticFieldStepper::GetMomentum(TVector3 &mom)
{
	mom = this->mom;
}


