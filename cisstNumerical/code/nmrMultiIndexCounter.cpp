/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ofri Sadowsky
  Created on:	2003

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrMultiIndexCounter.h>

#include <algorithm>

const nmrMultiIndexCounter::IndexType nmrMultiIndexCounter::DefaultIncrement(1);

nmrMultiIndexCounter::nmrMultiIndexCounter(unsigned int size)
  : Index(size, IndexType(0))
  , LowBounds(size, IndexType(0))
  , HighBounds(size, IndexType(0))
  , Increments(size, DefaultIncrement)
  , Status(COUNTER_IN_BOUNDS)
{
}


void nmrMultiIndexCounter::Increment()
{
	int s = GetSize();
	// i must be signed to go below zero!
	int i;
	for (i = s - 1; i >= 0; --i) {
		Index[i] += Increments[i];
		if ( Index[i] <= HighBounds[i] )
			break;
	}

	if (i < 0) {
		Status = COUNTER_OVERFLOW;
		return;
	}

	++i;
	for (; i < s; i++)
		Index[i] = LowBounds[i];

}


void nmrMultiIndexCounter::Decrement()
{
	int s = GetSize();
	// i must be signed to go below zero!
	int i; 
	for (i = s - 1; i >= 0; --i) {
		Index[i] -= Increments[i];
		if ( Index[i] >= LowBounds[i] )
			break;
	}

	if (i < 0) {
		Status = COUNTER_UNDERFLOW;
		return;
	}

	++i;
	for (; i < s; i++)
		Index[i] = HighBounds[i];
}


unsigned long nmrMultiIndexCounter::GetNumberOfCombinations() const
{
	unsigned long result = 1;
	unsigned int i;
	const unsigned int s = GetSize();
	for (i = 0; i < s; i++) {
		result *= ((HighBounds[i] - LowBounds[i]) / Increments[i]) + 1;
	}

	return result;
}

bool nmrMultiIndexCounter::IndexIsEqualTo(const nmrMultiIndexCounter & other) const
{
	if (GetSize() != other.GetSize())
		return false;

	return std::equal(Index.begin(), Index.end(), other.Index.begin());
}
