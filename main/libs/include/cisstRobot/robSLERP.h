#ifndef _robSLERP_h
#define _robSLERP_h

#include <cisstRobot/robFunction.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <iostream>
using namespace std;
namespace cisstRobot{

  //! Define a spherical linear interpolation function 
  class robSLERP : public robFunction {

  protected:
    
    Quaternion qinitial;
    Quaternion qfinal;
    R3 w;               
    Real xmin, xmax;

  public:

    //! hack...
    Real Duration() const { return xmax-xmin; }
    
    //! Create a SLERP between ti and tf
    robSLERP( Real ti, const SE3& Ri, Real tf, const SE3& Rf);
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    //! Evaluate the function
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };

}

#endif
