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

#ifndef _nmrRegistrationRigid_h
#define _nmrRegistrationRigid_h

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
*/

template <class _matrixOwnerType>
bool CISST_EXPORT nmrRegistrationRigid(vctDynamicConstVectorBase<_matrixOwnerType, vct3> &dataSet1,
                                       vctDynamicConstVectorBase<_matrixOwnerType, vct3> &dataSet2,
                                       vctFrm3 &transform, double *fre = 0);

#endif

