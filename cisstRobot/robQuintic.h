/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robQuintic_h
#define _robQuintic_h

//#include <cisstVector/vctFixedSizeVector.h>
#include <cisstRobot/robFunctionRn.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robQuintic : public robFunctionRn {

 private:

    bool IsSet;
    std::vector< vctFixedSizeVector<double,6> > X;     // the quintic parameters

    vctFixedSizeVector<double,6>
        ComputeParameters( double t1, double y1, double y1d, double y1dd,
                           double t2, double y2, double y2d, double y2dd );

    void EvaluateQuintic( double t,
                          const vctFixedSizeVector<double,6>& x,
                          double& y, double& yd, double& ydd);

 public:

    /*! Default constructor */
    robQuintic( void );

    //! Define a 3D quintic function \f$Q: R^1\rightarrow R^3 \f$
    /**
       Define a 5th order polynomial passing throuh \f$(t_1,\mathbf{y}_1)\f$ and
       \f$(t_2,\mathbf{y}_2)\f$. The final time \f$t_2\f$ is determined by the
       maximum velocities.  The function is bounded by \f$[t_1, t_2]\f$.
       \param t1 The initial time
       \param y1 The value \f$\mathbf{y}_1 = Q(t_1)\f$
       \param y1d The value \f$\mathbf{y}'_1 = Q(t_1)\f$
       \param y1dd The value \f$\mathbf{y}''_1 = Q(t_1)\f$
       \param t2 The final time
       \param y2 The value \f$\mathbf{y}_2 = Q(t_2)\f$
       \param y2d The value \f$\mathbf{y}'_2 = Q(t_2)\f$
       \param y2dd The value \f$\mathbf{y}''_2 = Q(t_2)\f$
    */
    robQuintic( double t1,
                const vctFixedSizeVector<double,3>& y1,
                const vctFixedSizeVector<double,3>& y1d,
                const vctFixedSizeVector<double,3>& y1dd,
                double t2,
                const vctFixedSizeVector<double,3>& y2,
                const vctFixedSizeVector<double,3>& y2d,
                const vctFixedSizeVector<double,3>& y2dd );

    //! Define a N quintic functions \f$Q: R^1\rightarrow R^N \f$
    /**
       Define a N 5th order polynomials passing throuh \f$(t_1,{y}_1^i)\f$ and
       \f$(t_2,{y}_2^i)\f$. The final time \f$t_2\f$ is determined by the
       maximum velocities.  The function is bounded by \f$[t_1, t_2]\f$.
       \param t1 The initial time
       \param y1 The value \f$\mathbf{y}_1 = Q(t_1)\f$
       \param y1d The value \f$\mathbf{y}'_1 = Q(t_1)\f$
       \param y1dd The value \f$\mathbf{y}''_1 = Q(t_1)\f$
       \param t2 The final time
       \param y2 The value \f$\mathbf{y}_2 = Q(t_2)\f$
       \param y2d The value \f$\mathbf{y}'_2 = Q(t_2)\f$
       \param y2dd The value \f$\mathbf{y}''_2 = Q(t_2)\f$
    */
    robQuintic( double t1,
                const vctDynamicVector<double>& y1,
                const vctDynamicVector<double>& y1d,
                const vctDynamicVector<double>& y1dd,
                double t2,
                const vctDynamicVector<double>& y2,
                const vctDynamicVector<double>& y2d,
                const vctDynamicVector<double>& y2dd );

    virtual ~robQuintic() {}

    void Set( double t1,
              const vctDynamicVector<double>& y1,
              const vctDynamicVector<double>& y1d,
              const vctDynamicVector<double>& y1dd,
              double t2,
              const vctDynamicVector<double>& y2,
              const vctDynamicVector<double>& y2d,
              const vctDynamicVector<double>& y2dd );

    void Evaluate( double t,
                   vctFixedSizeVector<double,3>& y,
                   vctFixedSizeVector<double,3>& yd,
                   vctFixedSizeVector<double,3>& ydd );

    void Evaluate( double t,
                   vctDynamicVector<double>& y,
                   vctDynamicVector<double>& yd,
                   vctDynamicVector<double>& ydd );


    void Blend( robFunction* function,
                const vctDynamicVector<double>& qdmax,
                const vctDynamicVector<double>& qddmax );

    void Blend( robFunction*, double, double );
};

#endif // _robQuintic_h
