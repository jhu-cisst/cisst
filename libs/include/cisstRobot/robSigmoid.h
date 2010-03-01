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

#ifndef _robSigmoid_h
#define _robSigmoid_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robSigmoid : public robFunction {
protected:
    
  double x[4];        // parameters scale, spread and offsets
  
  double tmin, tmax;
  
public:
  
  //! Return the sigmoid's final time
  double FinalTime() const { return tmax; }
  
  //! Define a sigmoid function \f$S: R^1\rightarrow R^1 \f$
  /**
     Define a sigmoid passing through \f$(0,y_1)\f$ and \f$(t,y_2)\f$ where 
     \f$t\f$ is determined by the value \f$ ydmax = \max S \f$.
     \param t1 The lower value of the domain
     \param y1 The initial value
     \param y2 The final value
     \param ydmax The maximum velocity at the saddle point
  */
  robSigmoid( robSpace::Basis codomain, 
	      double t1, double y1, 
	      double y2, double ydmax );
  
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
  robFunction::Errno Evaluate( const robVariable& input, 
			             robVariable& output );  
  
};

#endif
