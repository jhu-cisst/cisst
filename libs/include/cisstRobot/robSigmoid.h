#ifndef _robSigmoid_h
#define _robSigmoid_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robSigmoid : public robFunction {
  protected:
    
    real x1, x2, x3;  // parameters scale, offset, spread
    real ts;          // time shift
    
    real xmin, xmax;
    
  public:

    //! Create a 1D->1D sigmoid mapping
    /**
       Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
       The sigmoid is fits (scale, offset) the desired values
       \param x1 The lower value of the domain
       \param y1 The value \f$y1=sigmoid(x1)\f$
       \param x2 The upper value of the domain
       \param y2 The value \f$y2=sigmoid(x2)\f$
    */
    robSigmoid( real x1, real y1, real x2, real y2 );
    
    //! Create a 1D->1D sigmoid mapping
    /**
       Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
       The sigmoid is fits (scale, offset) the desired values
       \param x1 The lower value of the domain
       \param x2 The upper value of the domain
       \param ydmax The maximum velocity at the saddle point
    */
    robSigmoid( real x1, real x2, real ydmax );

    //! Return true if the input is between x1 and x2
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 

    //! Evaluate the sigmoid
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };
}

#endif
