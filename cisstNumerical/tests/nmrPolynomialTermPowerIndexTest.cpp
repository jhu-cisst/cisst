/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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


#include "nmrPolynomialTermPowerIndexTest.h"

const nmrPolynomialTermPowerIndexTest::VariableIndexType nmrPolynomialTermPowerIndexTest::CounterSize = 4;

const nmrPolynomialTermPowerIndexTest::PowerType nmrPolynomialTermPowerIndexTest::DegreeMax = 8;

nmrPolynomialTermPowerIndex 
nmrPolynomialTermPowerIndexTest::CreateRandomPowerIndexDegreeBounds(VariableIndexType numVariables,
                                                                 cmnRandomSequence & randomGenerator)
{
	PowerType minDegree = randomGenerator.ExtractRandomInt(0, DegreeMax);
	PowerType maxDegree = randomGenerator.ExtractRandomInt(minDegree+1, DegreeMax+1);
	if (maxDegree == 0)
		maxDegree = 1;

	nmrPolynomialTermPowerIndex result(numVariables, minDegree, maxDegree);

	return result;
}



nmrPolynomialTermPowerIndex & 
nmrPolynomialTermPowerIndexTest::MakeRandomPowerIndex(nmrPolynomialTermPowerIndex & termIndex,
                                                   cmnRandomSequence & randomGenerator)
{
	const MultinomialCoefficientType countPowers = termIndex.CountPowerCombinations();
	// randomly select an enumeration of 
	MultinomialCoefficientType powerEnumeration = randomGenerator.ExtractRandomInt(0, countPowers);

	termIndex.GoBegin();
	while (powerEnumeration > 0) {
		termIndex.Increment();
		powerEnumeration--;
	}

	return termIndex;
}


void nmrPolynomialTermPowerIndexTest::TestIncrementCountPowerCombinations(nmrPolynomialTermPowerIndex & termIndex)
{
	const MultinomialCoefficientType multinom = termIndex.CountPowerCombinations();
	MultinomialCoefficientType countPowers = 0;

	termIndex.GoBegin();
	while(termIndex.IsValid()) {
		termIndex.Increment();
		countPowers++;
	}

	CPPUNIT_ASSERT(multinom == countPowers);
}


void nmrPolynomialTermPowerIndexTest::TestDecrementCountPowerCombinations(nmrPolynomialTermPowerIndex & termIndex)
{
	const MultinomialCoefficientType multinom = termIndex.CountPowerCombinations();
	MultinomialCoefficientType countPowers = 0;

	termIndex.GoEnd();
	while(termIndex.IsValid()) {
		termIndex.Decrement();
		countPowers++;
	}

	CPPUNIT_ASSERT(multinom == countPowers);
}


void nmrPolynomialTermPowerIndexTest::TestIncrementDecrement(nmrPolynomialTermPowerIndex & termIndex)
{
	const nmrPolynomialTermPowerIndex initialIndex(termIndex);

	termIndex.Increment();
    CPPUNIT_ASSERT(initialIndex < termIndex);

	termIndex.Decrement();
	CPPUNIT_ASSERT(initialIndex.Compare(termIndex) == 0);
}


void nmrPolynomialTermPowerIndexTest::TestDecrementIncrement(nmrPolynomialTermPowerIndex & termIndex)
{
	const nmrPolynomialTermPowerIndex initialIndex(termIndex);

	termIndex.Decrement();
    CPPUNIT_ASSERT(initialIndex > termIndex);

	termIndex.Increment();
	CPPUNIT_ASSERT(initialIndex.Compare(termIndex) == 0);
}


void nmrPolynomialTermPowerIndexTest::TestIncreasingOrder(nmrPolynomialTermPowerIndex & termIndex1, nmrPolynomialTermPowerIndex & termIndex2)
{
	nmrPolynomialTermPowerIndex & lesserIndex = (termIndex1 < termIndex2) ? termIndex1 : termIndex2;
	nmrPolynomialTermPowerIndex & greaterIndex = (termIndex1 < termIndex2) ? termIndex2 : termIndex1;

	while ( (lesserIndex.IsValid()) && (lesserIndex.Compare(greaterIndex) < 0) )
		lesserIndex.Increment();

    CPPUNIT_ASSERT(lesserIndex.Compare(greaterIndex) == 0);
}


void nmrPolynomialTermPowerIndexTest::TestDecreasingOrder(nmrPolynomialTermPowerIndex & termIndex1, nmrPolynomialTermPowerIndex & termIndex2)
{
	nmrPolynomialTermPowerIndex & lesserIndex = (termIndex1 < termIndex2) ? termIndex1 : termIndex2;
	nmrPolynomialTermPowerIndex & greaterIndex = (termIndex1 < termIndex2) ? termIndex2 : termIndex1;

	while ( (greaterIndex.IsValid()) && (greaterIndex.Compare(lesserIndex) > 0) )
		greaterIndex.Decrement();

    CPPUNIT_ASSERT(greaterIndex.Compare(lesserIndex) == 0);
}

void nmrPolynomialTermPowerIndexTest::TestDegreeAfterIncrementAndDecrement(
    nmrPolynomialTermPowerIndex & termIndex, cmnRandomSequence & randomGenerator)
{
    MultinomialCoefficientType numIncrements
        = randomGenerator.ExtractRandomInt(1, termIndex.CountPowerCombinations() - 1);
    MultinomialCoefficientType numDecrements
        = randomGenerator.ExtractRandomInt(1, termIndex.CountPowerCombinations() - 1);
    bool increaseFirst = randomGenerator.ExtractRandomInt() & 1;

    unsigned int i;
    MultinomialCoefficientType numFirstIterations = (increaseFirst) ? numIncrements
        : numDecrements;
    MultinomialCoefficientType numSecondIterations = (increaseFirst) ? numDecrements
        : numIncrements;
    for (i = 0; i < numFirstIterations; ++i) {
        if (increaseFirst)
            termIndex.Increment();
        else
            termIndex.Decrement();
    }

    for (i = 0; i < numSecondIterations; ++i) {
        if (increaseFirst)
            termIndex.Decrement();
        else
            termIndex.Increment();
    }

    VariableIndexType varIndex;
    PowerType sumPowers = 0;
    for (varIndex = 0; varIndex < termIndex.GetNumVariables(); ++varIndex) {
        sumPowers += termIndex.GetPower(varIndex);
    }

    CPPUNIT_ASSERT( sumPowers == termIndex.GetDegree() );
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrPolynomialTermPowerIndexTest);

