/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
#ifndef _robLinear_h
#define _robLinear_h

#include <cisstVector/vctDynamicMatrixTypes.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

//! A linear function (as in Ax=b)
class CISST_EXPORT robLinear : public robFunction {
    
protected:
  
  //! The slope and offset
  double m, b;
  
  //! The minimum/maximum values of the domain
  double tmin, tmax;
  
public:
  
  //! Define a linear function \f$L: R^1\rightarrow R^1 \f$
  /**
     Define a line passing throuh \f$(t_1,y_1)\f$ and \f$(t_2,y_2)\f$. The 
     function is defined for the domain \f$[t_1,t_2]\f$.
     \param codomain The codomain of the function
     \param t1 The lower value of the domain
     \param y1 The value \f$y_1 = L(x_1)\f$
     \param t2 The upper value of the domain
     \param y2 The value \f$y_2 = L(x_2)\f$
  */
  robLinear( robSpace::Basis codomain, 
	     double t1, double y1, 
	     double t2, double y2 );
  
  //! Return the context of the function
  /**
     Call this method to find if the function is define for the input variable.
     \param[in] input The input variable
     \return CDEFINED if the function is defined for the input. CUNDEFINED if 
             the function is undefined.
  */
  robFunction::Context GetContext( const robVariable& input ) const; 
  
  //! Evaluate the function
  /**
     Evaluate the linear function for the given input. The domain of the input
     \param[in] input The input to the function. The input domain must be 
                      robSpace::TIME
     \param[out] output The output of the function. The domain of the output
                        will be set to the codomain of the function
     \return robFunction::ESUCCESS if no error occured. robFunction::EUNDEFINED
             if the function is not defined for the input.
  */
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output ); 
  
};

#endif
