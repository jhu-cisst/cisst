#ifndef _robRnConstant_h
#define _robRnConstant_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{

  class robRnConstant : public robFunction {
  protected:
    
    Rn constant;
    Rn xmin, xmax;
    
  public:

    //! Define a constant function
    /**
       Create a function \f$y=C\f$ that is constant for the domain \f$[x1,x2]\f$.
       \param y The value of the constant
       \param x1 The lower value of the domain (default -infinity)
       \param x2 The upper value of the domain (default infinity)
     */
    robRnConstant( Real y, Real x1=FLT_MIN, Real x2=FLT_MAX);

    //! Define a constant function R1->Rn
    /**
       Create a function \f$y=[c1,...,cn]\f$ that is constant for the domain 
       \f$[x1,x2]\f$.
       \param y The value of the constant
       \param x1 The lower value of the domain (default -infinity)
       \param x2 The upper value of the domain (default infinity)
     */
    robRnConstant( const Rn& y, Real x1=FLT_MIN, Real x2=FLT_MAX);

    //! Define a constant function R1->R3
    /**
       Create a function \f$y=[c1,c2,c3]\f$ that is constant for the domain 
       \f$[x1,x2]\f$.
       \param y The value of the constant
       \param x1 The lower value of the domain (default -infinity)
       \param x2 The upper value of the domain (default infinity)
     */
    robRnConstant( const R3& y, Real x1=FLT_MIN, Real x2=FLT_MAX);
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const;

    //! Return the constant value
    robError Evaluate( const robDOF& input, robDOF& output );
    
  };
}

#endif
