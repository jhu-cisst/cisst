#ifndef _robQuintic_h
#define _robQuintic_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{
  
  class robQuintic : public robFunction{

  private:
    
    Real xmin;
    Real xmax;
    vctFixedSizeVector<Real, 6> b;

  public:

    robQuintic(){}
    robQuintic( Real t1, Real x1, Real v1, Real a1, 
		Real t2, Real x2, Real v2, Real a2 );

    robDomainAttribute IsDefinedFor( const robDOF& input ) const;

    robError Evaluate( const robDOF& input, robDOF& output );

  };

}

#endif

