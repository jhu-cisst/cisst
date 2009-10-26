#ifndef _robSLERP_h
#define _robSLERP_h

#include <iostream>

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robFunction.h>

//! Define a spherical linear interpolation function 
class robSLERP : public robFunction {

protected:
    
  vctQuaternionRotation3<double> qinitial;
  vctQuaternionRotation3<double> qfinal;
  vctFixedSizeVector<double,3> w;               
  double xmin, xmax;
  
public:
  
  //! hack...
  double Duration() const { return xmax-xmin; }
  
  //! Create a SLERP between ti and tf
  robSLERP( double ti, const vctFrame4x4<double,VCT_ROW_MAJOR>& Ri, 
	    double tf, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rf);
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const; 
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );  
  
};

#endif
