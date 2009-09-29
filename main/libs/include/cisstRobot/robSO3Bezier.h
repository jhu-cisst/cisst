#ifndef _robSO3Bezier_h
#define _robSO3Bezier_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robSLERP.h>

#include <cisstVector/vctTransformationTypes.h>

namespace cisstRobot{

  class robSO3Bezier : public robFunction {

  private:

    int nctrlpoints;
    Quaternion q1;
    Quaternion q2;
    Quaternion qa;
    Quaternion qb;

    Real ti;
    SO3 Rwi;

    Real tmin;
    Real tmax;

    Quaternion SLERP( const Quaternion &q1, const Quaternion &q2, Real t);

  public:

    robSO3Bezier( Real t1,
		  Real t2,
		  const SO3& Rw1, 
		  const R3&  w1, 
		  const SO3& Rw2, 
		  const R3&  w2,
		  Real wmax );

    robSO3Bezier( const SO3& Rw0, 
		  const SO3& Rw1, 
		  const SO3& Rw2, 
		  const SO3& Rw3 );

    robDomainAttribute IsDefinedFor( const robDOF& input ) const ;
    robError Evaluate( const robDOF& input, robDOF& output );
  };

}

#endif
