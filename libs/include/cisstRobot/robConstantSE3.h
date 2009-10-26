#ifndef _robConstantSE3_h
#define _robConstantSE3_h

#include <cisstRobot/robFunction.h>

class robConstantSE3 : public robFunction {
protected:
  
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt;
  vctDynamicVector<double> xmin, xmax;
  
public:
  
  //! hack...
  virtual double Duration() const { return xmax.at(0)-xmin.at(0); }
  
  
  //! Create an SE3 constant function (M:t->SE3)
  /**
     Creates a time-valued SE3 constant function
     \param y The value of the constant
     \param x1 The lower value of the domain (default -infinity)
     \param x2 The upper value of the domain (default infinity)
  */
  robConstantSE3( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
		  double xstart=FLT_MIN, double xend=FLT_MAX);
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );
  
  };

#endif
