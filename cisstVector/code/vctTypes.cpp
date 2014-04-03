/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2003-09-16

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctTypes.h>

// include header files of reference and dynamic types here, to ensure that
// they get preprocessed successfully
#include <cisstVector/vctFixedSizeConstVectorRef.h>
#include <cisstVector/vctFixedSizeVectorRef.h>
#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>
#include <cisstVector/vctDynamicMatrixRef.h>

#if (CISST_COMPILER == CISST_DOTNET7)

template class vctFixedSizeVector<double, 1>;
template class vctFixedSizeVector<double, 2>;
template class vctFixedSizeVector<double, 3>;
template class vctFixedSizeVector<double, 4>;
template class vctFixedSizeVector<double, 5>;
template class vctFixedSizeVector<double, 6>;

template class vctFixedSizeVector<float, 1>;
template class vctFixedSizeVector<float, 2>;
template class vctFixedSizeVector<float, 3>;
template class vctFixedSizeVector<float, 4>;
template class vctFixedSizeVector<float, 5>;
template class vctFixedSizeVector<float, 6>;

template class vctFixedSizeVector<int, 1>;
template class vctFixedSizeVector<int, 2>;
template class vctFixedSizeVector<int, 3>;
template class vctFixedSizeVector<int, 4>;
template class vctFixedSizeVector<int, 5>;
template class vctFixedSizeVector<int, 6>;

template class vctFixedSizeVector<char, 1>;
template class vctFixedSizeVector<char, 2>;
template class vctFixedSizeVector<char, 3>;
template class vctFixedSizeVector<char, 4>;
template class vctFixedSizeVector<char, 5>;
template class vctFixedSizeVector<char, 6>;

template class vctBarycentricVector<double, 2>;
template class vctBarycentricVector<double, 3>;
template class vctBarycentricVector<double, 4>;


template class vctFixedSizeMatrix<double, 2, 2>;
template class vctFixedSizeMatrix<double, 2, 3>;
template class vctFixedSizeMatrix<double, 2, 4>;
template class vctFixedSizeMatrix<double, 3, 2>;
template class vctFixedSizeMatrix<double, 3, 3>;
template class vctFixedSizeMatrix<double, 3, 4>;
template class vctFixedSizeMatrix<double, 4, 2>;
template class vctFixedSizeMatrix<double, 4, 3>;
template class vctFixedSizeMatrix<double, 4, 4>;

template class vctFixedSizeMatrix<float, 2, 2>;
template class vctFixedSizeMatrix<float, 3, 3>;
template class vctFixedSizeMatrix<float, 4, 4>;

template class vctFixedSizeMatrix<int, 2, 2>;
template class vctFixedSizeMatrix<int, 3, 3>;
template class vctFixedSizeMatrix<int, 4, 4>;

template class vctFixedSizeMatrix<char, 2, 2>;
template class vctFixedSizeMatrix<char, 3, 3>;
template class vctFixedSizeMatrix<char, 4, 4>;


template class vctQuaternion<double>;
template class vctQuaternion<float>;


template class vctDynamicVector<float>;
template class vctDynamicVector<double>;
template class vctDynamicVector<int>;
template class vctDynamicVector<char>;
template class vctDynamicVector<bool>;
template class vctDynamicVector<unsigned int>;
template class vctDynamicVector<unsigned char>;
template class vctDynamicVector<short>;
template class vctDynamicVector<unsigned short>;

#endif

