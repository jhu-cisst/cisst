/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstVector/vctDeterminant.h>
#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrRegistrationRigid.h>

template <class _matrixOwnerType>
bool nmrRegistrationRigid(vctDynamicConstVectorBase<_matrixOwnerType, vct3> &dataSet1,
                          vctDynamicConstVectorBase<_matrixOwnerType, vct3> &dataSet2,
                          vctFrm3 &transform)
{
    size_t npoints = dataSet1.size();
    if (npoints != dataSet2.size()) {
        CMN_LOG_RUN_WARNING << "nmrRegistrationPairedPoint: incompatible sizes: " << npoints << ", "
                            << dataSet2.size() << std::endl;
        return false;
    }
    else if (npoints <= 0) {
        CMN_LOG_RUN_WARNING << "nmrRegistrationPairedPoint called for " << npoints << " points." << std::endl;
        return false;
    }
    // Compute averages
    vct3 avg1 = dataSet1.SumOfElements();
	avg1.Divide(npoints);
    vct3 avg2 = dataSet2.SumOfElements();
	avg2.Divide(npoints);

    // Compute the sum of the outer products of (dataSet1-avg1) and (dataSet2-avg2)
    vctDouble3x3 H, sumH;
    sumH.SetAll(0.0);
    for (size_t i = 0; i < npoints; i++) {
        H.OuterProductOf(dataSet1[i]-avg1, dataSet2[i]-avg2);
        sumH.Add(H);
    }
    // Now, compute SVD of sumH
    vctDouble3x3 U, Vt;
    vctDouble3 S;
    nmrSVD(sumH, U, S, Vt);
    // Compute X = V*U' = (U*V')'
    vctDouble3x3 X = (U*Vt).Transpose();
    double det = vctDeterminant<3>::Compute(X);
    // If determinant is not 1, apply correction from Umeyama(1991)
    if (fabs(det-1.0) > 1e-6) {
        vctDouble3x3 Fix(0.0);
        Fix.Diagonal() = vct3(1.0, 1.0, -1.0);
        X = (U*Fix*Vt).Transpose();
        det = vctDeterminant<3>::Compute(X);
        if (fabs(det-1.0) > 1e-6) {
            CMN_LOG_RUN_WARNING << "nmrRegistrationPairedPoint: registration failed!!" << std::endl;
            return false;
        }
    }
    vctMatRot3 R;
    R.Assign(X);
    transform = vctFrm3(R, avg2-R*avg1);
    return true;
}

// Explicit template instantiations

// For vctDynamicConstVectorRef
template bool nmrRegistrationRigid<vctDynamicVectorRefOwner<vct3> >(
                               vctDynamicConstVectorBase<vctDynamicVectorRefOwner<vct3>, vct3>&,
                               vctDynamicConstVectorBase<vctDynamicVectorRefOwner<vct3>, vct3>&,
                               vctFrm3&);

// For vctDynamicConstVector
template bool CISST_EXPORT nmrRegistrationRigid<vctDynamicVectorOwner<vct3> >(
                               vctDynamicConstVectorBase<vctDynamicVectorOwner<vct3>, vct3>&,
                               vctDynamicConstVectorBase<vctDynamicVectorOwner<vct3>, vct3>&,
                               vctFrm3&);

