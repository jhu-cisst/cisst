#ifndef _robLinear_h
#define _robLinear_h

#include <cisstRobot/robFunction.h>
#include <cisstVector/vctDynamicMatrixTypes.h>

//! A linear function (as in Ax=b)
class robLinear : public robFunction {
    
protected:
  
  //! The matrix
  vctDynamicMatrix<double> A;  // a dynamic matrix to hold the mapping
  
  //! The minimum/maximum values of the domain
  vctDynamicVector<double> xmin, xmax;
  
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
  robLinear( double x1, double y1, double x2, double y2 );
  
  //! Define a linear mapping M:vctFixedSizeVector<double,1>->vctDynamicVector<double>
  /**
     Define a linear mapping from a 1D value (time) to a Nd image. The mapping
     is defined over \f$[x1, x2]\f$
     \param x1 The lower value of the domain
     \param y1 The value \f$y1 = f(x1)\f$
     \param x2 The upper value of the domain
     \param y2 The value \f$y2 = f(x2)\f$
  */
  robLinear( double x1, const vctDynamicVector<double>& y1, 
	     double x2, const vctDynamicVector<double>& y2 );
  
  //! Define a linear mapping M:R1->R3 (read a time varying translation)
  /**
     Define a linear mapping from a 1D value (time) to a 3d image. The mapping
     is defined over \f$[x1, x2]\f$
     \param x1 The lower value of the domain
     \param y1 The value \f$y1 = f(x1)\f$
     \param x2 The upper value of the domain
     \param y2 The value \f$y2 = f(x2)\f$
  */
  robLinear( double x1, const vctFixedSizeVector<double,3>& y1, 
	     double x2, const vctFixedSizeVector<double,3>& y2 );
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const; 
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );  
  
};

#endif
