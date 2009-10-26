#ifndef _robSigmoid_h
#define _robSigmoid_h

#include <cisstRobot/robFunction.h>

class robSigmoid : public robFunction {
protected:
    
  double x1, x2, x3;  // parameters scale, offset, spread
  double ts;          // time shift
  
  double xmin, xmax;
  
public:
  
  //! hack...
  double Duration() const { return xmax-xmin; }
  
  //! Create a 1D->1D sigmoid mapping
  /**
     Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
     The sigmoid is fits (scale, offset) the desired values
     \param x1 The lower value of the domain
     \param y1 The value \f$y1=sigmoid(x1)\f$
     \param x2 The upper value of the domain
     \param y2 The value \f$y2=sigmoid(x2)\f$
  */
  robSigmoid( double x1, double y1, double x2, double y2 );
  
  //! Create a 1D->1D sigmoid mapping
  /**
     Define a sigmoid function that is bounded by \f$(x1,y1)\f$ and \f$(x2,y2)\f$
     The sigmoid is fits (scale, offset) the desired values
     \param x1 The lower value of the domain
     \param x2 The upper value of the domain
     \param ydmax The maximum velocity at the saddle point
  */
  robSigmoid( double x1, double x2, double ydmax );
  
  //! Return true if the input is between x1 and x2
  robDomainAttribute IsDefinedFor( const robVariables& input ) const; 
  
  //! Evaluate the sigmoid
  robError Evaluate( const robVariables& input, 
		     robVariables& output );  
  
};

#endif
