#ifndef _robR3Blender_h
#define _robR3Blender_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robHermite.h>

namespace cisstRobot{

  class robR3Blender : public robFunction {
  private:
    
    robHermite* blendx;
    robHermite* blendy;
    robHermite* blendz;
    
    real xmin, xmax;

  public:
    
    robR3Blender( real x1, const R3& ti, const R3& vi, const R3& vid,
		  real x2, const R3& tf, const R3& vf, const R3& vif );
    
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };

}

#endif
