#ifndef _robSE3Constant_h
#define _robSE3Constant_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robSE3Constant : public robFunction {
  protected:
    
    SE3 Rt;
    Rn xmin, xmax;
    
  public:

    //! hack...
    virtual Real Duration() const { return xmax.at(0)-xmin.at(0); }
    
    
    //! Create an SE3 constant function (M:R1->SE3)
    /**
       Creates a time-valued SE3 constant function
       \param y The value of the constant
       \param x1 The lower value of the domain (default -infinity)
       \param x2 The upper value of the domain (default infinity)
     */
    robSE3Constant( const SE3& Rt, Real xstart=FLT_MIN, Real xend=FLT_MAX);
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const;

    //! Evaluate the function
    robError Evaluate( const robDOF& input, robDOF& output );
    
  };
}

#endif
