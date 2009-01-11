/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrDynAllocPolynomialContainerTest.h,v 1.5 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on: 2004-10-21
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrDynAllocPolynomialContainerTest_h
#define _nmrDynAllocPolynomialContainerTest_h

#include "nmrPolynomialBaseTest.h"
#include <cisstNumerical/nmrDynAllocPolynomialContainer.h>

class nmrDynAllocPolynomialContainerTest : public nmrPolynomialBaseTest
{
public:
    typedef nmrPolynomialBaseTest BaseType;

	// Remove one random term from a polynomial. This is one of the terms in the container,
	// and so the function is declared for a nmrDynAllocPolynomialContainer rather than
	// to the abstract nmrPolynomialBase.
	static nmrDynAllocPolynomialContainer & RemoveRandomTerm
		(nmrDynAllocPolynomialContainer & polynomial, cmnRandomSequence & randomGenerator);


	static nmrDynAllocPolynomialContainer & SetRandomCoefficients
		(nmrDynAllocPolynomialContainer & polynomial, cmnRandomSequence & randomGenerator);

	// Test all criteria of emptiness of a polynomial:
	// IsEmpty()
	// GetNumberOfTerms() == 0
	// FirstTermIterator() == EndTermIterator()
	//
	// The given polynomial must be empty for the test to have a chance of success.
	static void TestEmptiness(const nmrDynAllocPolynomialContainer & polynomial);


	// Insert some random terms into the polynomial, then remove all the terms in a random
	// order.  Verify that the polynomial is empty.
	static void TestInsertRemove(nmrDynAllocPolynomialContainer & polynomial,
        cmnRandomSequence & randomGenerator);
    void TestInsertRemove()
    {
        TestInsertRemove(*DynAllocTestTarget, RandomGenerator);
    }

	// Fill all possible terms into the polynomial, then Clear() the polynomial.
	// Verify that the polynomial is empty.  Note that Clear() is not necessarily identical
	// to removing all the terms one by one, and needs to be tested separately (following
	// bug discovery).
	static void TestFillClear(nmrDynAllocPolynomialContainer & polynomial);
    void TestFillClearContainer()
    {
        nmrDynAllocPolynomialContainerTest::TestFillClear(*DynAllocTestTarget);
    }

	// Fill the polynomial with some random terms, and test the different iterator
	// functions.
	static void TestIterator(nmrDynAllocPolynomialContainer & polynomial, cmnRandomSequence & randomGenerator);
    void TestIterator()
    {
        TestIterator(*DynAllocTestTarget, RandomGenerator);
    }

    CPPUNIT_TEST_SUB_SUITE( nmrDynAllocPolynomialContainerTest, nmrPolynomialBaseTest );
    CPPUNIT_TEST( TestInsertRemove );
    CPPUNIT_TEST( TestFillClearContainer );
    CPPUNIT_TEST( TestIterator );
    CPPUNIT_TEST_SUITE_END();

protected:
    nmrDynAllocPolynomialContainer * DynAllocTestTarget;
};


#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrDynAllocPolynomialContainerTest.h,v $
// Revision 1.5  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.4  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.1  2004/10/24 03:51:03  ofri
// Renamed cisstNumerical test classes and files.  Tests run but fail for some
// cases.
//
// ****************************************************************************
