/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky
  Created on: 2004-10-21

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrPolynomialTermPowerIndexTest_h
#define _nmrPolynomialTermPowerIndexTest_h

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrPolynomialTermPowerIndex.h>

#include <cisstCommon/cmnRandomSequence.h>

class nmrPolynomialTermPowerIndexTest : public CppUnit::TestFixture
{
public:
	typedef  nmrPolynomialTermPowerIndex::VariableIndexType VariableIndexType;
	typedef  nmrPolynomialTermPowerIndex::PowerType PowerType;
	typedef  nmrPolynomialTermPowerIndex::MultinomialCoefficientType MultinomialCoefficientType;

	static const PowerType DegreeMax;

    static const VariableIndexType CounterSize;

    /*!
      Return a new nmrPolynomialTermPowerIndex of a specified size with random
      degree bounds
    */
	static nmrPolynomialTermPowerIndex CreateRandomPowerIndexDegreeBounds(VariableIndexType numVariables,
        cmnRandomSequence & randomGenerator);

	// Generate a valid power index with even distribution.  We count how many power
	// combinations exist for the term, then choose a random number in this range,
	// and increment the index this number of times.  Currently, we do not have a more
	// efficient way to enumerate the indices.
	//
	// Note: The function uses nmrPolynomialTermPowerIndex::CountPowerCombinations().
	// Make sure that is has been tested before using this function.
	static nmrPolynomialTermPowerIndex &
		MakeRandomPowerIndex(nmrPolynomialTermPowerIndex & termIndex, cmnRandomSequence & randomGenerator);


	// Test that the possible number of increments from GoBegin() is equal to the number
	// of combinations evaluated by CountPowerCombinations().
	static void TestIncrementCountPowerCombinations(nmrPolynomialTermPowerIndex & termIndex);
	void TestIncrementCountPowerCombinations()
    {
        TestIncrementCountPowerCombinations(TestIndex1);
    }

	// Test that the possible number of decrements from GoEnd() is equal to the number
	// of combinations evaluated by CountPowerCombinations().
	static void TestDecrementCountPowerCombinations(nmrPolynomialTermPowerIndex & termIndex);
	void TestDecrementCountPowerCombinations()
    {
        TestDecrementCountPowerCombinations(TestIndex1);
    }

	// Increment the index, and test that the result is greater than the original.
	// Then decrement, and test that we return to the same index.
	static void TestIncrementDecrement(nmrPolynomialTermPowerIndex & termIndex);
	void TestIncrementDecrement()
    {
        TestIncrementDecrement(TestIndex1);
    }

	// Decrement the index, and test that the result is less than the original.
	// Then increment, and test that we return to the same index.
	static void TestDecrementIncrement(nmrPolynomialTermPowerIndex & termIndex);
	void TestDecrementIncrement()
    {
        TestDecrementIncrement(TestIndex1);
    }

	// Choose the lower index of the two and increase it until invalid or equal
	// to the other. Note: the two indices must be compatible in their size
	// and limits, and valid, for the test to potentially succeed.
	static void TestIncreasingOrder(nmrPolynomialTermPowerIndex & termIndex1, nmrPolynomialTermPowerIndex & termIndex2);
	void TestIncreasingOrder()
    {
        TestIncreasingOrder(TestIndex1, TestIndex2);
    }

	// Choose the higher index of the two and decrease it until invalid or equal
	// to the other. Note: the two indices must be compatible in their size
	// and limits, and valid, for the test to potentially succeed.
	static void TestDecreasingOrder(nmrPolynomialTermPowerIndex & termIndex1, nmrPolynomialTermPowerIndex & termIndex2);
	void TestDecreasingOrder()
    {
        TestDecreasingOrder(TestIndex1, TestIndex2);
    }

    /*! Start from the current index, and either increment and decrement, or decrement
      and increment (based on a random decision) a random number of increments and
      decrements.  In the end, test that the degree of the index is equal to the sum
      of powers.
    */
    static void TestDegreeAfterIncrementAndDecrement(nmrPolynomialTermPowerIndex & termIndex,
        cmnRandomSequence & randomGenerator);
    void TestDegreeAfterIncrementAndDecrement()
    {
        TestDegreeAfterIncrementAndDecrement(TestIndex1, RandomGenerator);
    }


	void setUp()
	{
		MakeRandomPowerIndex(TestIndex1, RandomGenerator);
        MakeRandomPowerIndex(TestIndex2, RandomGenerator);
	}

	void tearDown()
	{
	}

	nmrPolynomialTermPowerIndexTest()
        : RandomGenerator(cmnRandomSequence::GetInstance())
		, TestIndex1( CreateRandomPowerIndexDegreeBounds(CounterSize, RandomGenerator) )
		, TestIndex2( TestIndex1 )
	{}

	virtual ~nmrPolynomialTermPowerIndexTest()
	{
	}


private:
    cmnRandomSequence & RandomGenerator;

    nmrPolynomialTermPowerIndex TestIndex1;
    nmrPolynomialTermPowerIndex TestIndex2;

	// CppUnit suite interface
	CPPUNIT_TEST_SUITE( nmrPolynomialTermPowerIndexTest );
	CPPUNIT_TEST( TestIncrementDecrement );
	CPPUNIT_TEST( TestDecrementIncrement );
	CPPUNIT_TEST( TestIncreasingOrder );
	CPPUNIT_TEST( TestDecreasingOrder );
	CPPUNIT_TEST( TestIncrementCountPowerCombinations );
	CPPUNIT_TEST( TestDecrementCountPowerCombinations );
    CPPUNIT_TEST( TestDegreeAfterIncrementAndDecrement );
	CPPUNIT_TEST_SUITE_END();
};

#endif
