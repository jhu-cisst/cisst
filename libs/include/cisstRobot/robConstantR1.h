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

#ifndef _robConstantR1_h
#define _robConstantR1_h

#include <cisstVector/vctDynamicVector.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

//! A constant function \f$ C: R^1 \rightarrow R^n \f$
/**
   Constant function are used to keep a DOF "locked". For example,
   if you want the robot to remain still you need to insert a constant function
   into the trajectory. The domain of robConstantR1 is defined by an interval
   \f$ \begin{bmatrix} t_{min} & t_{max} \end{bmatrix} \f$.
*/

class CISST_EXPORT robConstantR1 : public robFunction {

protected:
  
  double constant;  // The constant vector
  double tmin;      // Lower time bound
  double tmax;      // Higher time bound
  
public:
  
  //! Define a constant function \f$ C: R^1 \rightarrow R^1 \f$
  /**
     Create a function \f$ C: R^1 \rightarrow R^1 \f$ that is constant for the 
     domain \f$\begin{bmatrix} t_{min} & t_{max} \end{bmatrix}\f$. The codomain
     must be a subset of the configuration space or Cartesian space.
     \param codomainbasis The basis of the codomain 
     \param constant The value of the constant
     \param tin The lower value of the domain (default -infinity)
     \param tf The upper value of the domain (default infinity)
  */
  robConstantR1(robSpace::Basis codomain, double ti, double yi, double tf);

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
