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


#ifndef _nmrMultiIndexCounterTest_h
#define _nmrMultiIndexCounterTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrMultiIndexCounter.h>

#include <cisstCommon/cmnRandomSequence.h>


class nmrMultiIndexCounterTest : public CppUnit::TestFixture
{
public:
	typedef nmrMultiIndexCounter::IndexType IndexType;

	static const nmrMultiIndexCounter::IndexType CounterMax;

	static const unsigned int CounterSize;

	// Initialize a counter with random bounds.
	static nmrMultiIndexCounter & MakeRandomBoundsMultiIndexCounter(nmrMultiIndexCounter & counter,
        cmnRandomSequence & randomGenerator);

	// Initialize a counter with random increments.
	static nmrMultiIndexCounter & MakeRandomIncrementsMultiIndexCounter(nmrMultiIndexCounter & counter,
        cmnRandomSequence & randomGenerator);

	// Set a random index value in the valid range (between bounds) for each cell of the counter
	static nmrMultiIndexCounter & MakeRandomIndexCounter(nmrMultiIndexCounter & counter,
        cmnRandomSequence & randomGenerator);

	// Run a counter from the low bounds upward until stop.
	// Test that the counter is in overflow status, and that all the index cells
	// have values higher than HighBounds.
    static void TestIncrementFromLowToHigh(nmrMultiIndexCounter & counter);
	void TestIncrementFromLowToHigh()
    {
        TestIncrementFromLowToHigh(RandomIncrementCounter);
    }

	// Run a counter from the low bounds upward until stop.
	// Test that the counter is in underflow status, and that all the index cells
	// have values lower than LowBounds.
	static void TestDecrementFromHighToLow(nmrMultiIndexCounter & counter);
    void TestDecrementFromHighToLow()
    {
        TestDecrementFromHighToLow(RandomIncrementCounter);
    }

	// Run a counter from the low bounds upward until stop.
	// Test that the number of increments is equal to the evaluated GetNumberOfCombintations().
	static void TestIncrementCombinations(nmrMultiIndexCounter & counter);
	void TestIncrementCombinations()
    {
        TestIncrementCombinations(RandomIncrementCounter);
    }

	// Run a counter from the high bounds downward until stop.
	// Test that the number of decrements is equal to the evaluated GetNumberOfCombintations().
	static void TestDecrementCombinations(nmrMultiIndexCounter & counter);
	void TestDecrementCombinations()
    {
        TestDecrementCombinations(RandomIncrementCounter);
    }

	// Increment then decrement. Check that the index at the end is equal to the initial.
	// Note: This will not when the size of an increment is not a factor of
	// HighBounds - LowBounds + 1.
	static void TestIncrementDecrement(nmrMultiIndexCounter & counter);
    void TestIncrementDecrement()
    {
        TestIncrementDecrement(DefaultIncrementCounter);
    }


	// Decrement then increment. Check that the index at the end is equal to the initial.
	// Note: This will not when the size of an increment is not a factor of
	// HighBounds - LowBounds + 1.
	void TestDecrementIncrement(nmrMultiIndexCounter & counter);
	void TestDecrementIncrement()
    {
        TestDecrementIncrement(DefaultIncrementCounter);
    }

//protected:
	void setUp()
	{
		MakeRandomIndexCounter(DefaultIncrementCounter, RandomGenerator);
		MakeRandomIndexCounter(RandomIncrementCounter, RandomGenerator);
		MakeRandomIncrementsMultiIndexCounter(RandomIncrementCounter, RandomGenerator);
	}

	void tearDown()
	{
	}

	nmrMultiIndexCounterTest()
        : RandomGenerator( cmnRandomSequence::GetInstance() )
		, DefaultIncrementCounter(CounterSize)
		, RandomIncrementCounter(CounterSize)
	{
		MakeRandomBoundsMultiIndexCounter(DefaultIncrementCounter, RandomGenerator);
		MakeRandomBoundsMultiIndexCounter(RandomIncrementCounter, RandomGenerator);
	}

	virtual ~nmrMultiIndexCounterTest()
	{
	}

private:
    cmnRandomSequence & RandomGenerator;


	nmrMultiIndexCounter DefaultIncrementCounter;
	nmrMultiIndexCounter RandomIncrementCounter;

	// CppUnit suite interface
	CPPUNIT_TEST_SUITE( nmrMultiIndexCounterTest );
	CPPUNIT_TEST( TestIncrementDecrement );
	CPPUNIT_TEST( TestDecrementIncrement );
	CPPUNIT_TEST( TestIncrementFromLowToHigh );
	CPPUNIT_TEST( TestDecrementFromHighToLow );
	CPPUNIT_TEST( TestIncrementCombinations );
	CPPUNIT_TEST( TestDecrementCombinations );
	CPPUNIT_TEST_SUITE_END();

};

#endif
