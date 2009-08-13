#ifndef _robLinear_h
#define _robLinear_h

#include <cisstRobot/robFunction.h>
#include <cisstVector/vctDynamicMatrixTypes.h>

namespace cisstRobot{

  //! A linear function (as in Ax=b)
  class robLinear : public robFunction {
    
  protected:
    
    //! The matrix
    vctDynamicMatrix<real> A;  // a dynamic matrix to hold the mapping
    
    //! The minimum/maximum values of the domain
    Rn xmin, xmax;
    
  public:
    
    //! Define a line
    /**
       Define a line passing throuh \f$(x1,y1)\f$ and \f$(x2,y2)\f$. The domain is 
       defined between \f$[x1,x2]\f$,
       \param x1 The lower value of the domain
       \param y1 The value \f$y1 = f(x1)\f$
       \param x2 The upper value of the domain
       \param y2 The value \f$f(x2)\f$
     */
    robLinear( real x1, real y1, real x2, real y2 );

    //! Define a linear mapping M:R1->Rn
    /**
       Define a linear mapping from a 1D value (time) to a Nd image. The mapping
       is defined over \f$[x1, x2]\f$
       \param x1 The lower value of the domain
       \param y1 The value \f$y1 = f(x1)\f$
       \param x2 The upper value of the domain
       \param y2 The value \f$y2 = f(x2)\f$
     */
    robLinear( real x1, const Rn& y1, real x2, const Rn& y2 );
    
    //! Define a linear mapping M:R1->R3 (read a time varying translation)
    /**
       Define a linear mapping from a 1D value (time) to a 3d image. The mapping
       is defined over \f$[x1, x2]\f$
       \param x1 The lower value of the domain
       \param y1 The value \f$y1 = f(x1)\f$
       \param x2 The upper value of the domain
       \param y2 The value \f$y2 = f(x2)\f$
     */
    robLinear( real x1, const R3& y1, real x2, const R3& y2 );
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    //! Evaluate the function
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };
}

#endif
