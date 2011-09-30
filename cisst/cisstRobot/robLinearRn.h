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
#ifndef _robLinearRn_h
#define _robLinearRn_h

#include <cisstRobot/robFunctionRn.h>
#include <cisstRobot/robExport.h>

//! A linear function (as in Ax=b)
class CISST_EXPORT robLinearRn : public robFunctionRn {
    
 private:

  vctDynamicVector<double> m;
  vctDynamicVector<double> b;  
  
  void ComputeParameters();

 public:
  
  //! Define a linear function \f$L: R^1\rightarrow R^3 \f$
  /**
     Define a line passing throuh \f$(t_1,\mathbf{y}_1)\f$ and 
     \f$(t_2,\mathbf{y}_2)\f$. The final time \f$t_2\f$ is determined by the
     maximum velocities.  The function is bounded by \f$[t_1, t_2]\f$.
     \param t1 The initial time
     \param y1 The value \f$\mathbf{y}_1 = L(t_1)\f$
     \param y2 The value \f$\mathbf{y}_2 = L(t_2)\f$
     \param vmax  The magnitude of the maximum linear velocity
  */
  robLinearRn( const vctFixedSizeVector<double,3>& y1, 
	       const vctFixedSizeVector<double,3>& y2,
	       double vmax,
	       double t1 = 0.0 );
  
  //! Define a linear function \f$L: R^1\rightarrow R^n \f$
  /**
     Define a line passing throuh \f$(t_1,\mathbf{y}_1)\f$ and 
     \f$(t_2,\mathbf{y}_2)\f$. The final time \f$t_2\f$ is determined by the
     maximum velocities.  The function is bounded by \f$[t_1, t_2]\f$.
     \param t1 The initial time
     \param y1 The value \f$\mathbf{y}_1 = L(t_1)\f$
     \param y2 The value \f$\mathbf{y}_2 = L(t_2)\f$
     \param ydmax The maximum velocity for each dimension
  */
  robLinearRn( const vctDynamicVector<double>& y1, 
	       const vctDynamicVector<double>& y2,
	       const vctDynamicVector<double>& ydmax,
	       double t1 = 0.0 );

  //! Evaluate the function
  /**
     Evaluate the linear function for the given input. The domain of the input
     \param[in] input The input to the function. The input domain must be 
                      robSpace::TIME
     \param t The time
     \param[out] y The linear function at time t
     \param[out] yd The 1st order time derivative at time t
     \param[out] ydd The 2nd order time derivative at time t (always zero)
  */
  void Evaluate( double t, double& y, double& yd, double& ydd );
  
  //! Evaluate the function
  /**
     Evaluate the linear function for the given input. The domain of the input
     \param[in] input The input to the function. The input domain must be 
                      robSpace::TIME
     \param t The time
     \param[out] p The linear function at time t
     \param[out] v The linear velocity
     \param[out] vd The linear acceleration
  */
  void Evaluate( double t, 
		 vctFixedSizeVector<double,3>& p,
		 vctFixedSizeVector<double,3>& v,
		 vctFixedSizeVector<double,3>& vd );
  

  //! Evaluate the function
  /**
     Evaluate the linear function for the given input. The domain of the input
     \param[in] input The input to the function. The input domain must be 
                      robSpace::TIME
     \param t The time
     \param[out] y The linear function at time t
     \param[out] yd The 1st order time derivative at time t
     \param[out] ydd The 2nd order time derivative at time t (always zero)
  */
  void Evaluate( double t, 
		 vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );
  
  void Blend( robFunction* function, 
	      const vctDynamicVector<double>& qd, 
	      const vctDynamicVector<double>& qdd );

  void Blend( robFunction* function, double vmax, double vdmax );


  vctDynamicVector<double> Slope() const;

};

#endif
