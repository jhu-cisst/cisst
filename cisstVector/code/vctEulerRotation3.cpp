/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

namespace vctEulerRotation3Order {

    std::string ToString(vctEulerRotation3Order::OrderType order)
    {
        std::string str;
        switch(order) {
        case vctEulerRotation3Order::ZYZ:
            str = "ZYZ";
            break;
        case vctEulerRotation3Order::ZYX:
            str = "ZYX";
            break;
        default:
            str = "UNDEFINED";
            break;
        }
        return str;
    }

};


void vctEulerRotation3Base::Assign(double phi, double theta, double psi)
{
    this->Angles.Assign(phi, theta, psi);
}


vctEulerRotation3Base & vctEulerRotation3Base::InverseSelf(void)
{
    double tmp = Angles[0];
    if (Angles[2] == cmnPI)
        Angles[0] = Angles[2];
    else
        Angles[0] = -Angles[2];
    Angles[1] = -Angles[1];
    if (tmp == cmnPI)
        Angles[2] = tmp;
    else
        Angles[2] = -tmp;
    return *this;
}


vctEulerRotation3Base & vctEulerRotation3Base::NormalizedSelf(void)
{
    // TODO: limit Angles[1] to 0 to PI
    while (Angles[0] > cmnPI)
        Angles[0] -= 2*cmnPI;
    while (Angles[0] <= -cmnPI)
        Angles[0] += 2*cmnPI;
    while (Angles[2] > cmnPI)
        Angles[2] -= 2*cmnPI;
    while (Angles[2] <= -cmnPI)
        Angles[2] += 2*cmnPI;
    return *this;
}


bool vctEulerRotation3Base::IsNormalized(double CMN_UNUSED(tolerance)) const
{
#if 0
    return ((Angles[0] > -cmnPI) && (Angles[0] <= cmnPI) &&
            (Angles[1] >= 0.0) && (Angles[1] <= cmnPI) &&
            (Angles[2] > -cmnPI) && (Angles[2] <= cmnPI));
#else
    return true;
#endif
}


/*****************************************************************************

  ZYZ Euler Angles. This is an intrinsic rotation, where phi (angle[0]) is
  rotation about the Z axis, theta (angle[1]) is rotation about the Y' axis,
  and psi (angle[2]) is rotation about the Z'' axis.

*****************************************************************************/
template <class _matrixType>
void vctEulerToMatrixRotation3(const vctEulerZYZRotation3 & eulerRot,
                               vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    typedef typename _matrixType::value_type value_type;

    double cphi = cos(eulerRot.phi());      // 0
    double sphi = sin(eulerRot.phi());
    double ctheta = cos(eulerRot.theta());  // 1
    double stheta = sin(eulerRot.theta());
    double cpsi = cos(eulerRot.psi());      // 2
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


template <class _matrixType>
void vctEulerFromMatrixRotation3(vctEulerZYZRotation3 & eulerRot,
                                 const vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    typedef typename _matrixType::value_type value_type;

    // Initialize to the current Euler Angles -- these are used to resolve the singularity (gimbal lock)
    double phi = eulerRot.phi();
    double theta = eulerRot.theta();
    double psi = eulerRot.psi();

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
        phi = atan(matrixRot.Element(1,2)/matrixRot.Element(0,2));
        double  cphi = cos(phi);
        double  sphi = sin(phi);
        theta = atan2((cphi * matrixRot.Element(0,2) + sphi * matrixRot.Element(1,2)), (double)matrixRot.Element(2,2));
        psi = atan2((-sphi * matrixRot.Element(0,0) + cphi * matrixRot.Element(1,0)),
                    (-sphi * matrixRot.Element(0,1) + cphi * matrixRot.Element(1,1)));
    }
    eulerRot.Assign(phi, theta, psi);
}


/*****************************************************************************

  ZYX Euler Angles, which can be called Yaw-Pitch-Roll. This is an intrinsic
  rotation of yaw about the Z axis, pitch about the Y' axis, and roll
  about the X'' axis.  Note that this is also equivalent to an extrinsic
  XYZ rotation, with the angles in reverse order (i.e., roll about the X axis,
  pitch about the Y axis, and yaw about the Z axis). In this context, roll, pitch
  and yaw do not have the same meaning as in aircraft dynamics.

*****************************************************************************/
template <class _matrixType>
void vctEulerToMatrixRotation3(const vctEulerZYXRotation3 & eulerRot,
                               vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    typedef typename _matrixType::value_type value_type;

    double  yaw = eulerRot.alpha();
    double  pitch = eulerRot.beta();
    double  roll = eulerRot.gamma();

    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);

    double cosPitch = cos(pitch);
    double sinPitch = sin(pitch);

    double cosRoll = cos(roll);
    double sinRoll = sin(roll);

    matrixRot.Element(0,0) = static_cast<value_type>(cosYaw * cosPitch);
    matrixRot.Element(0,1) = static_cast<value_type>(cosYaw * sinPitch * sinRoll - sinYaw * cosRoll);
    matrixRot.Element(0,2) = static_cast<value_type>(cosYaw * sinPitch * cosRoll + sinYaw * sinRoll);

    matrixRot.Element(1,0) = static_cast<value_type>(sinYaw * cosPitch);
    matrixRot.Element(1,1) = static_cast<value_type>(cosYaw * cosRoll + sinYaw * sinPitch * sinRoll);
    matrixRot.Element(1,2) = static_cast<value_type>(sinYaw * sinPitch * cosRoll - cosYaw * sinRoll);

    matrixRot.Element(2,0) = static_cast<value_type>(-sinPitch);
    matrixRot.Element(2,1) = static_cast<value_type>(cosPitch * sinRoll);
    matrixRot.Element(2,2) = static_cast<value_type>(cosPitch * cosRoll);
}


template <class _matrixType>
void vctEulerFromMatrixRotation3(vctEulerZYXRotation3 & eulerRot,
                                 const vctMatrixRotation3Base<_matrixType> & matrixRot)
{
    // Implementation currently does not handle "gimble lock" (singularity)
    double yaw = atan2((double) matrixRot.Element(1,0), (double) matrixRot.Element(0,0));
    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);
    double pitch = atan2((double) -matrixRot.Element(2,0),
                         cosYaw*matrixRot.Element(0,0) + sinYaw*matrixRot(1,0));
    double roll = atan2( sinYaw*matrixRot.Element(0,2) - cosYaw*matrixRot.Element(1,2),
                         -sinYaw*matrixRot.Element(0,1) + cosYaw*matrixRot.Element(1,1));
    eulerRot.Assign(yaw, pitch, roll);
}


// force instantiation of helper functions
#define INSTANTIATE_EULER_TEMPLATES(ORDER)                              \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template void                                                       \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot,    \
                                const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template void                                                       \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                              vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot);

// Instantiate templates for each supported Euler angle convention (e.g., ZYZ)
INSTANTIATE_EULER_TEMPLATES(vctEulerRotation3Order::ZYZ)
INSTANTIATE_EULER_TEMPLATES(vctEulerRotation3Order::ZYX)


