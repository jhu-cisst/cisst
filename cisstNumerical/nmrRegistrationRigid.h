/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides

  (C) Copyright 2007-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _nmrRegistrationRigid_h
#define _nmrRegistrationRigid_h

#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctTypes.h>

// Always include last
#include <cisstNumerical/nmrExport.h>

/*! Perform a paired-point rigid registration using the SVD method proposed by
     Arun(1987) and modified by Umeyama(1991).

    \param dataSet1   The first set of fiducial points
    \param dataSet2   The second set of fiducial points
    \param transform  The computed transformation from dataSet1 to dataSet2
    \param fre        Pointer to location to store Fiducial Registration Error (FRE), if not 0.
    \returns true if registration successful

    \note This function is templated so that it will work when the data sets are of type
          vctDynamicConstVectorRef or vctDynamicConstVector. The explicit template instantiations
          are in the cpp file.

    This function is wrapped for Python, as follows:

    (transform, fre) = nmrRigidRegistration(dataSet1, dataSet2)

    where dataSet1 and dataSet2 are 2D numpy arrays, such as ((1.0, 2.0, 3.0), (4.0, 5.0, 6.0), ...).
    The Python function throws an exception if registration fails for any reason, including problems
    with the input data (e.g., data sets not the same number of points).
*/

template <class _vectorOwnerType>
bool CISST_EXPORT nmrRegistrationRigid(vctDynamicConstVectorBase<_vectorOwnerType, vct3> &dataSet1,
                                       vctDynamicConstVectorBase<_vectorOwnerType, vct3> &dataSet2,
                                       vctFrm3 &transform, double *fre = 0);

#if (CISST_OS == CISST_WINDOWS) && defined(CISST_COMPILER_IS_MSVC)
template CISST_EXPORT bool nmrRegistrationRigid<vctDynamicVectorRefOwner<vct3> >(
                                       vctDynamicConstVectorBase<vctDynamicVectorRefOwner<vct3>, vct3> &dataSet1,
                                       vctDynamicConstVectorBase<vctDynamicVectorRefOwner<vct3>, vct3> &dataSet2,
                                       vctFrm3 &transform, double *fre);
template CISST_EXPORT bool nmrRegistrationRigid<vctDynamicVectorOwner<vct3> >(
                                       vctDynamicConstVectorBase<vctDynamicVectorOwner<vct3>, vct3> &dataSet1,
                                       vctDynamicConstVectorBase<vctDynamicVectorOwner<vct3>, vct3> &dataSet2,
                                       vctFrm3 &transform, double *fre);
#endif

#endif

