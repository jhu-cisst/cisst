#ifndef _robHermite_h
#define _robHermite_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robHermite : public robFunction {
  private:
    
    real a0, a1, a2, a3, a4, a5;
    real xmin, xmax;
    
  public:
    
    robHermite( real x1, real y1, real y1d, real y1dd, 
		real x2, real y2, real y2d, real y2dd );
    
    robHermite( real x1, const R3& y1, real x2, const R3& y2 );
    
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };

}

#endif
