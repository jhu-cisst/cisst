/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Daniel Li
  Created on:	2006-08-07
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericNArrayTest_h
#define _vctGenericNArrayTest_h

#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnPortability.h>

#include "vctGenericContainerTest.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>


/* Please check vctGenericContainerTest.h for some basic
   guidelines. */
class vctGenericNArrayTest
{
public:

    /*! Test assignments */
    template <class _containerType1, class _containerType2>
    static void TestAssignment(const _containerType1 & nArray1,
                               _containerType2 & nArray2)
    {
        typedef typename _containerType1::value_type value_type;

        typename _containerType1::const_iterator nArray1Iter;
        typename _containerType2::iterator nArray2Iter;

		// Test a simple assignment of one nArray to another using the Assign() method
        vctRandom(nArray2, value_type(-10), value_type(10));
        nArray2.Assign(nArray1);
        CPPUNIT_ASSERT( nArray2.dimension() == nArray1.dimension() );
        CPPUNIT_ASSERT( nArray2.sizes() == nArray1.sizes() );
        for (nArray2Iter = nArray2.begin(), nArray1Iter = nArray1.begin();
             nArray2Iter != nArray2.end();
             ++nArray2Iter, ++nArray1Iter)
        {
            CPPUNIT_ASSERT( *nArray2Iter == *nArray1Iter );
        }

        // Test a simple assignment of one nArray to another using the = operator
        vctRandom(nArray2, value_type(-10), value_type(10));
        nArray2 = nArray1;
        CPPUNIT_ASSERT( nArray2.dimension() == nArray1.dimension() );
        CPPUNIT_ASSERT( nArray2.sizes() == nArray1.sizes() );
        for (nArray2Iter = nArray2.begin(), nArray1Iter = nArray1.begin();
             nArray2Iter != nArray2.end();
             ++nArray2Iter, ++nArray1Iter)
        {
            CPPUNIT_ASSERT( *nArray2Iter == *nArray1Iter );
        }
    }

    
};


#endif  // _vctGenericNArrayTest_h

