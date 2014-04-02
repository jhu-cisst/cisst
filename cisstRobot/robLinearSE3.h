/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robLinearSE3_h
#define _robLinearSE3_h

#include <cisstRobot/robFunctionSE3.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robLinearSE3 : public robFunctionSE3 {

protected:

    double vmax; /*!< max linear velocity */
    double wmax; /*!< max angular velocity */

    bool IsSet; /*!< is start and stop pose set */

public:

    /*! Default constructor */
    robLinearSE3( void );

    /*!
     \brief construct SE3 linear function

     \param Rtw1 start SE3 transform
     \param Rtw2 stop SE3 transform
     \param vmax max linear velocity
     \param wmax max angular velocity
     \param t1 start time
    */
    robLinearSE3( const vctFrame4x4<double>& Rtw1,
                  const vctFrame4x4<double>& Rtw2,
                  double vmax, double wmax,
                  double t1 = 0.0 );

    /*!
     \brief Set new Rtw1 and Rtw2 transform

     \param Rtw1 start SE3 transform
     \param Rtw2 stop SE3 transform
     \param vmax max linear velocity
     \param wmax max angular velocity
     \param t1 start time
    */
    void Set( const vctFrame4x4<double>& Rtw1,
              const vctFrame4x4<double>& Rtw2,
              double vmax, double wmax,
              double t1 = 0.0 );

    /*!
     \brief Evaluate the funciton
     Evaluate the linear SE3 function for the given input.

     \param t time
     \param[out] Rt Tranform at time t
     \param[out] vw Velocity at time t (linear + angular)
     \param[out] vdwd Acceleration at time t (linear + angular)
    */
    void Evaluate( double t,
                   vctFrame4x4<double>& Rt,
                   vctFixedSizeVector<double,6>& vw,
                   vctFixedSizeVector<double,6>& vdwd );

    void Blend( robFunction* function, double vmax, double vdmax );

    robLinearSE3& operator=( const robLinearSE3& function );

};

#endif
