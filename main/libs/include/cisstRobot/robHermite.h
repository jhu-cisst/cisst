#ifndef _robHermite_h
#define _robHermite_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robHermite : public robFunction {
  private:
    
    Real a0, a1, a2, a3, a4, a5;
    Real xmin, xmax;
    
  public:
    
    robHermite( Real x1, Real y1, Real y1d, Real y1dd, 
		Real x2, Real y2, Real y2d, Real y2dd );
    
    robHermite( Real x1, const R3& y1, Real x2, const R3& y2 );
    
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };

}

#endif
