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


#include "nmrMultiIndexCounterTest.h"


const nmrMultiIndexCounterTest::IndexType nmrMultiIndexCounterTest::CounterMax = 20;
const unsigned int nmrMultiIndexCounterTest::CounterSize = 4;

nmrMultiIndexCounter & 
nmrMultiIndexCounterTest::MakeRandomBoundsMultiIndexCounter(nmrMultiIndexCounter & counter,
                                                         cmnRandomSequence & randomGenerator)
{
	std::vector<IndexType> lowBounds(counter.GetSize());
	std::vector<IndexType> highBounds(counter.GetSize());
	
	unsigned int i;
	for (i = 0; i < counter.GetSize(); i++) {
		int lowBound = randomGenerator.ExtractRandomInt(0, CounterMax);
		int highBound = randomGenerator.ExtractRandomInt(lowBound+1, CounterMax+1);

		lowBounds[i] = lowBound;
		highBounds[i] = highBound;
	}

	counter.SetBounds( &(lowBounds.front()), &(highBounds.front()) );

	return counter;
}

nmrMultiIndexCounter & 
nmrMultiIndexCounterTest::MakeRandomIncrementsMultiIndexCounter(nmrMultiIndexCounter & counter,
                                                             cmnRandomSequence & randomGenerator)
{
	std::vector<IndexType> increments(counter.GetSize());
	
	unsigned int i;
	for (i = 0; i < counter.GetSize(); i++) {
		int increment = randomGenerator.ExtractRandomInt(1, counter.GetHighBound(i) - counter.GetLowBound(i) + 1);
		increments[i] = increment;
	}

	counter.SetIncrements( &(increments.front()) );

	return counter;
}


nmrMultiIndexCounter & 
nmrMultiIndexCounterTest::MakeRandomIndexCounter(nmrMultiIndexCounter & counter,
                                              cmnRandomSequence & randomGenerator)
{
	const IndexType * highBounds = counter.GetHighBounds();
	const IndexType * lowBounds = counter.GetLowBounds();

	unsigned int i;
	for (i = 0; i < counter.GetSize(); i++) {
		int index = randomGenerator.ExtractRandomInt(lowBounds[i], highBounds[i]+1);
		counter.SetIndex(i, index);
	}

	counter.UpdateStatus();

	return counter;
}

void nmrMultiIndexCounterTest::TestIncrementFromLowToHigh(nmrMultiIndexCounter & counter)
{
	counter.GoToLowBounds();
	while (counter.IsBelowHighBounds()) {
		counter.Increment();
	}

	CPPUNIT_ASSERT(counter.GetStatus() == nmrMultiIndexCounter::COUNTER_OVERFLOW);

	unsigned int i;
	const IndexType * highBounds = counter.GetHighBounds();

	// See the documentation for nmrMultiIndexCounter::Increment() for specification
	// of stopping values for the counter.
	for (i = 0; i < counter.GetSize(); i++) {
		CPPUNIT_ASSERT(counter[i] > highBounds[i]);
	}
}


void nmrMultiIndexCounterTest::TestDecrementFromHighToLow(nmrMultiIndexCounter & counter)
{
	counter.GoToHighBounds();
	while (counter.IsAboveLowBounds()) {
		counter.Decrement();
	}

	CPPUNIT_ASSERT(counter.GetStatus() == nmrMultiIndexCounter::COUNTER_UNDERFLOW);

	unsigned int i;
	const IndexType * lowBounds = counter.GetLowBounds();

	// See the documentation for nmrMultiIndexCounter::Decrement() for specification
	// of stopping values for the counter.
	for (i = 0; i < counter.GetSize(); i++) {
		CPPUNIT_ASSERT( counter[i] < lowBounds[i] );
	}

}


void nmrMultiIndexCounterTest::TestIncrementDecrement(nmrMultiIndexCounter & counter)
{
	nmrMultiIndexCounter start(counter);
	counter.Increment();
	if (counter.GetStatus() == nmrMultiIndexCounter::COUNTER_OVERFLOW)
		return;
	counter.Decrement();

	CPPUNIT_ASSERT(counter.IndexIsEqualTo(start));
}


void nmrMultiIndexCounterTest::TestDecrementIncrement(nmrMultiIndexCounter & counter)
{
	nmrMultiIndexCounter start(counter);
	counter.Decrement();
	if (counter.GetStatus() == nmrMultiIndexCounter::COUNTER_UNDERFLOW)
		return;
	counter.Increment();
	CPPUNIT_ASSERT( counter.IndexIsEqualTo(start) );
}


void nmrMultiIndexCounterTest::TestIncrementCombinations(nmrMultiIndexCounter & counter)
{
	counter.GoToLowBounds();
	const int counterEvaluatedCombinations = counter.GetNumberOfCombinations();
	int combinationCounter = 0;
	while (counter.IsBelowHighBounds()) {
		counter.Increment();
		combinationCounter++;
	}

	CPPUNIT_ASSERT(combinationCounter == counterEvaluatedCombinations);
}


void nmrMultiIndexCounterTest::TestDecrementCombinations(nmrMultiIndexCounter & counter)
{
	counter.GoToHighBounds();
	unsigned int combinationCounter = 0;
	while (counter.IsAboveLowBounds()) {
		counter.Decrement();
		combinationCounter++;
	}

	CPPUNIT_ASSERT(combinationCounter == counter.GetNumberOfCombinations());
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrMultiIndexCounterTest);

