#ifndef _robQuintic_h
#define _robQuintic_h

#include <cisstRobot/robFunction.h>

class robQuintic : public robFunction{

private:
  
  double xmin;
  double xmax;
  vctFixedSizeVector<double, 6> b;
  
public:
  
  robQuintic(){}
  robQuintic( double t1, double x1, double v1, double a1, 
	      double t2, double x2, double v2, double a2 );
  
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  robError Evaluate( const robVariables& input, robVariables& output );
  
};

#endif

