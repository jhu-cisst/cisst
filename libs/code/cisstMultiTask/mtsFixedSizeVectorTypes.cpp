/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsVector.cpp 535 2009-07-14 00:27:41Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:   2010-02-17

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFixedSizeVectorTypes.h>

#define MTS_FIXED_SIZE_VECTOR_IMPLEMENT(ElementType)   \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##1); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##2); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##3); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##4); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##5); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##6)  \

MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Double);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Float);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Int);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UInt);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Char);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UChar);
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Bool);
