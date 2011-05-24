/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Created on:	2011-05-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctEulerRotation3.h>
#include <cisstVector/vctMatrixRotation3Base.h>

vctEulerRotation3Base & vctEulerRotation3Base::NormalizedSelf(void)
{
    // TO FIX: angles[0] and angles[2] have 2*PI range -- should this be [0, 2*PI] or [-PI, PI]?
    //         angles[1] has PI range -- should this be [0, PI] or [-PI/2, PI/2]?
    angles[0] = fmod(angles[0], 2.0 * cmnPI);
    if (angles[0] < 0.0)
        angles[0] += (2.0 * cmnPI);
    angles[2] = fmod(angles[2], 2.0 * cmnPI);
    if (angles[2] < 0.0)
        angles[2] += (2.0 * cmnPI);
    return *this;
}

bool vctEulerRotation3Base::IsNormalized(double tolerance) const
{
    // TO FIX: angles[0] and angles[2] have 2*PI range -- should this be [0, 2*PI] or [-PI, PI]?
    //         angles[1] has PI range -- should this be [0, PI] or [-PI/2, PI/2]?
    return ((angles[0] >= 0.0) && (angles[0] < 2*cmnPI) &&
            (angles[1] >= 0.0) && (angles[1] < cmnPI) &&
            (angles[2] >= 0.0) && (angles[2] < 2*cmnPI));
}


template <vctEulerRotation3Order::OrderType order, class _matrixType>
void
vctEulerRotation3FromRaw(vctEulerRotation3<order> & eulerRot,
                         const vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    typedef typename _matrixType::value_type value_type;

    double phi = 0.0;
    double theta = 0.0;
    double psi = 0.0;

    if((matrixRot.Element(0,2) * matrixRot.Element(0,2) + matrixRot.Element(1,2) * matrixRot.Element(1,2))  < 0.0001) {
        if (matrixRot.Element(2,2) > 0){
            theta = 0.0;
            value_type ftmp = atan2(matrixRot.Element(1,0), matrixRot.Element(0,0));
            double  ftmp2 = (ftmp - ( phi + psi )) / 2.0;
            phi += ftmp2;
            psi += ftmp2;
        }
        else {
            theta = cmnPI;
            value_type  ftmp = atan2(matrixRot.Element(0,1), matrixRot.Element(1,1));
            double  ftmp2 = (ftmp - (psi - phi)) / 2.0;
            phi -= ftmp2;
            psi += ftmp2;
        }
    }
    else  {  // non-singular config.
        phi = atan(matrixRot.Element(1,2) / matrixRot.Element(0,2));
        double  cphi = cos(phi);
        double  sphi = sin(phi);
        theta = atan2((cphi * matrixRot.Element(0,2) + sphi * matrixRot.Element(1,2)), (double)matrixRot.Element(2,2));
        psi = atan2((-sphi * matrixRot.Element(0,0) + cphi * matrixRot.Element(1,0)),
                    (-sphi * matrixRot.Element(0,1) + cphi * matrixRot.Element(1,1)));
    }
    eulerRot = vctEulerRotation3<order>(phi, theta, psi);
}

template <vctEulerRotation3Order::OrderType order, class _matrixType>
void
vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<order> & eulerRot,
                                   vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    typedef typename _matrixType::value_type value_type;

    double cphi = cos(eulerRot.phi());      // 1
    double sphi = sin(eulerRot.phi());
    double ctheta = cos(eulerRot.theta());  // 2
    double stheta = sin(eulerRot.theta());
    double cpsi = cos(eulerRot.psi());      // 3
    double spsi = sin(eulerRot.psi());

    matrixRot.Element(0, 0) = static_cast<value_type>(cphi * ctheta * cpsi - sphi * spsi);
    matrixRot.Element(0, 1) = static_cast<value_type>(-cphi * ctheta * spsi - sphi * cpsi);
    matrixRot.Element(0, 2) = static_cast<value_type>(cphi * stheta);

    matrixRot.Element(1, 0) = static_cast<value_type>(sphi * ctheta * cpsi + cphi * spsi);
    matrixRot.Element(1, 1) = static_cast<value_type>(-sphi * ctheta * spsi + cphi * cpsi);
    matrixRot.Element(1, 2) = static_cast<value_type>(sphi * stheta);

    matrixRot.Element(2, 0) = static_cast<value_type>(-stheta * cpsi);
    matrixRot.Element(2, 1) = static_cast<value_type>(stheta * spsi);
    matrixRot.Element(2, 2) = static_cast<value_type>(ctheta);
}

// force instantiation of helper functions
#define INSTANTIATE_EULER_TEMPLATES(ORDER) \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template void \
    vctEulerRotation3FromRaw(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot); \
    template void  \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void  \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void  \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void  \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template void \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template void \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template void \
    vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot);

// Instantiate templates for each supported Euler angle convention (e.g., ZYZ)
INSTANTIATE_EULER_TEMPLATES(vctEulerRotation3Order::ZYZ)


