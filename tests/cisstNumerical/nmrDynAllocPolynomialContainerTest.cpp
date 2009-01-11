/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrDynAllocPolynomialContainerTest.cpp,v 1.5 2007/04/26 20:12:05 anton Exp $
  
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


#include "nmrDynAllocPolynomialContainerTest.h"


//#include "nmrPolynomialTermPowerIndexTest.h"
#include "nmrPolynomialBaseTest.h"

nmrDynAllocPolynomialContainer & 
nmrDynAllocPolynomialContainerTest::RemoveRandomTerm(nmrDynAllocPolynomialContainer & polynomial,
                                                  cmnRandomSequence & randomGenerator)
{
	int termNumber = randomGenerator.ExtractRandomInt(0, polynomial.GetNumberOfTerms());
	int i;
	nmrDynAllocPolynomialContainer::TermIteratorType termIterator = polynomial.FirstTermIterator();
	for (i = 0; i < termNumber; i++) {
		termIterator++;
	}

	polynomial.RemoveTerm(termIterator);

	return polynomial;
}


nmrDynAllocPolynomialContainer & 
nmrDynAllocPolynomialContainerTest::SetRandomCoefficients(nmrDynAllocPolynomialContainer & polynomial,
                                                       cmnRandomSequence & randomGenerator)
{
	nmrDynAllocPolynomialContainer::TermIteratorType termIterator = polynomial.FirstTermIterator();
	while (termIterator != polynomial.EndTermIterator()) {
		CoefficientType coefficient = nmrPolynomialBaseTest::ChooseRandomVariable(randomGenerator);
		polynomial.SetCoefficient(termIterator, coefficient);

		termIterator++;
	}

	return polynomial;
}



void nmrDynAllocPolynomialContainerTest::TestEmptiness(const nmrDynAllocPolynomialContainer & polynomial)
{
    BaseType::TestEmptiness(polynomial);

    CPPUNIT_ASSERT( polynomial.FirstTermIterator() == polynomial.EndTermIterator() );
}


void nmrDynAllocPolynomialContainerTest::TestInsertRemove(nmrDynAllocPolynomialContainer & polynomial,
                                                       cmnRandomSequence & randomGenerator)
{
	polynomial.Clear();

	// create up to a quarter-full polynomial
	int numInserts = (polynomial.GetMaxNumberOfTerms() / 4) + 1;

	unsigned int successfulInserts = nmrPolynomialBaseTest::InsertRandomTerms(polynomial, numInserts, randomGenerator);

    CPPUNIT_ASSERT( polynomial.GetNumberOfTerms() == successfulInserts );

	nmrPolynomialBaseTest::TestSingleRandomTerm(polynomial, randomGenerator);

	int i;
	int numTerms = polynomial.GetNumberOfTerms();
	for (i = 0; i < numTerms; i++) {
		RemoveRandomTerm(polynomial, randomGenerator);
	}

	TestEmptiness(polynomial);
}


void nmrDynAllocPolynomialContainerTest::TestFillClear(nmrDynAllocPolynomialContainer & polynomial)
{
    BaseType::TestFillClear(polynomial);
	TestEmptiness(polynomial);
}


void nmrDynAllocPolynomialContainerTest::TestIterator(nmrDynAllocPolynomialContainer & polynomial,
                                                   cmnRandomSequence & randomGenerator)
{
	polynomial.Clear();

	// create up to a sixth-full polynomial
	TermCounterType numInserts = (polynomial.GetMaxNumberOfTerms() / 6) + 1;
	TermCounterType successfulInserts = nmrPolynomialBaseTest::InsertRandomTerms(polynomial, numInserts, randomGenerator);

	nmrDynAllocPolynomialContainer::TermIteratorType iterator = polynomial.FirstTermIterator();
	nmrDynAllocPolynomialContainer::TermConstIteratorType constIterator = polynomial.FirstTermIterator();
	TermCounterType termCounter = 0;

	while (iterator != polynomial.EndTermIterator()) {
		CoefficientType coefficient = nmrPolynomialBaseTest::ChooseRandomVariable(randomGenerator);
		polynomial.SetCoefficient(iterator, coefficient);

        CPPUNIT_ASSERT( polynomial.GetCoefficient(iterator) == coefficient );

		CPPUNIT_ASSERT( polynomial.GetCoefficient(constIterator) == coefficient);

		iterator++;
		constIterator++;
		termCounter++;
	}

	CPPUNIT_ASSERT( constIterator == polynomial.EndTermIterator() );

	CPPUNIT_ASSERT( termCounter == successfulInserts );
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrDynAllocPolynomialContainerTest.cpp,v $
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
