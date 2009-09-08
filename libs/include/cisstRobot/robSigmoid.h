#ifndef _robSigmoid_h
#define _robSigmoid_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robSigmoid : public robFunction {
  protected:
    
    Real x1, x2, x3;  // parameters scale, offset, spread
    Real ts;          // time shift
    
    Real xmin, xmax;
    
  public:

    //! hack...
    Real Duration() const { return xmax-xmin; }

    //! Create a 1D->1D sigmoid mapping
    /**
       Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
       The sigmoid is fits (scale, offset) the desired values
       \param x1 The lower value of the domain
       \param y1 The value \f$y1=sigmoid(x1)\f$
       \param x2 The upper value of the domain
       \param y2 The value \f$y2=sigmoid(x2)\f$
    */
    robSigmoid( Real x1, Real y1, Real x2, Real y2 );
    
    //! Create a 1D->1D sigmoid mapping
    /**
       Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
       The sigmoid is fits (scale, offset) the desired values
       \param x1 The lower value of the domain
       \param x2 The upper value of the domain
       \param ydmax The maximum velocity at the saddle point
    */
    robSigmoid( Real x1, Real x2, Real ydmax );

    //! Return true if the input is between x1 and x2
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 

    //! Evaluate the sigmoid
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };
}

#endif
