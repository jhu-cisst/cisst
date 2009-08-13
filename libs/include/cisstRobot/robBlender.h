#ifndef _robBlender_h
#define _robBlender_h

#include <cisstRobot/robHermite.h>
#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robBlender : public robFunction {
  private:
    
    robHermite* hermite;
    //robBezier*  bezier;
    
    real xmin, xmax;

  public:
    
    robBlender( const robDOFRn&  S0, const robDOFRn&  S1 );
    robBlender( const robDOF& input, 
		const robDOFRn& outgoing, 
		const robDOFRn& incoming );
    
    robDomainAttribute IsDefinedFor( const robDOF& input ) const;
    
    robError Evaluate( const robDOF& input, robDOF& output );
    
  };
}
#endif
