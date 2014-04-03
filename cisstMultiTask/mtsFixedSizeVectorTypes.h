/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:   2010-02-27

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsFixedSizeVectorTypes_h
#define _mtsFixedSizeVectorTypes_h

#include <cisstMultiTask/mtsFixedSizeVector.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

#define MTS_FIXED_SIZE_VECTOR_DECLARE(elementType, ElementType) \
typedef mtsFixedSizeVector<elementType, 1> mts##ElementType##1; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##1); \
typedef mtsFixedSizeVector<elementType, 2> mts##ElementType##2; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##2); \
typedef mtsFixedSizeVector<elementType, 3> mts##ElementType##3; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##3); \
typedef mtsFixedSizeVector<elementType, 4> mts##ElementType##4;    \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##4); \
typedef mtsFixedSizeVector<elementType, 5> mts##ElementType##5; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##5); \
typedef mtsFixedSizeVector<elementType, 6> mts##ElementType##6; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##6)

#define MTS_FIXED_SIZE_VECTOR_DECLARE_UNSIGNED(elementType, ElementType) \
typedef mtsFixedSizeVector<unsigned elementType, 1> mts##ElementType##1; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##1); \
typedef mtsFixedSizeVector<unsigned elementType, 2> mts##ElementType##2; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##2); \
typedef mtsFixedSizeVector<unsigned elementType, 3> mts##ElementType##3; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##3); \
typedef mtsFixedSizeVector<unsigned elementType, 4> mts##ElementType##4; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##4); \
typedef mtsFixedSizeVector<unsigned elementType, 5> mts##ElementType##5; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##5); \
typedef mtsFixedSizeVector<unsigned elementType, 6> mts##ElementType##6; \
CMN_DECLARE_SERVICES_INSTANTIATION(mts##ElementType##6)


// define some basic vectors
MTS_FIXED_SIZE_VECTOR_DECLARE(double, Double);
MTS_FIXED_SIZE_VECTOR_DECLARE(float, Float);
MTS_FIXED_SIZE_VECTOR_DECLARE(int, Int);
MTS_FIXED_SIZE_VECTOR_DECLARE_UNSIGNED(int, UInt);
MTS_FIXED_SIZE_VECTOR_DECLARE(char, Char);
MTS_FIXED_SIZE_VECTOR_DECLARE_UNSIGNED(char, UChar);
MTS_FIXED_SIZE_VECTOR_DECLARE(bool, Bool);


#endif // _mtsFixedSizeVectorTypes_h
