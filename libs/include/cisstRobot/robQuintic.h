#ifndef _robQuintic_h
#define _robQuintic_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{
  
  class robQuintic : public robFunction{

  private:
    
    real xmin;
    real xmax;
    vctFixedSizeVector<real, 6> b;

  public:

    robQuintic(){}
    robQuintic( real t1, real x1, real v1, real a1, 
		real t2, real x2, real v2, real a2 );

    robDomainAttribute IsDefinedFor( const robDOF& input ) const;

    robError Evaluate( const robDOF& input, robDOF& output );

  };

}

#endif

