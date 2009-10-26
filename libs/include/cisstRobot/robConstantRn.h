#ifndef _robConstantRn_h
#define _robConstantRn_h

#include <cisstVector/vctDynamicVector.h>

#include <cisstRobot/robFunction.h>

class robConstantRn : public robFunction {
 protected:
  
  vctDynamicVector<double> constant;
  vctDynamicVector<double> xmin, xmax;
  
public:
  
  //! Define a constant function
  /**
     Create a function \f$y=C\f$ that is constant for the domain \f$[x1,x2]\f$.
     \param y The value of the constant
     \param x1 The lower value of the domain (default -infinity)
     \param x2 The upper value of the domain (default infinity)
  */
  robConstantRn( double y, double x1=FLT_MIN, double x2=FLT_MAX);
  
  //! Define a constant function vctFixedSizeVector<double,1>->vctDynamicVector<double>
  /**
     Create a function \f$y=[c1,...,cn]\f$ that is constant for the domain 
     \f$[x1,x2]\f$.
     \param y The value of the constant
     \param x1 The lower value of the domain (default -infinity)
     \param x2 The upper value of the domain (default infinity)
  */
  robConstantRn( const vctDynamicVector<double>& y, double x1=FLT_MIN, double x2=FLT_MAX);
  
  //! Define a constant function vctFixedSizeVector<double,1>->vctFixedSizeVector<double,3>
  /**
     Create a function \f$y=[c1,c2,c3]\f$ that is constant for the domain 
     \f$[x1,x2]\f$.
     \param y The value of the constant
     \param x1 The lower value of the domain (default -infinity)
     \param x2 The upper value of the domain (default infinity)
  */
  robConstantRn( const vctFixedSizeVector<double,3>& y, double x1=FLT_MIN, double x2=FLT_MAX);
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  //! Return the constant value
  robError Evaluate( const robVariables& input, robVariables& output );
  
};

#endif
