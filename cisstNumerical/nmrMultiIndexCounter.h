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


#ifndef _nmrMultiIndexCounter_h
#define _nmrMultiIndexCounter_h

/*! This macro controls the conditional compilation of formatted output
 * of a nmrPolynomialTermPowerIndex . The macro and its use should be erased
 * when output format is decided on.
 */
#ifndef DEFINE_FORMATTED_OUTPUT
#define DEFINE_FORMATTED_OUTPUT 0
#endif

#include <vector>
#include <cisstCommon/cmnPortability.h>
#include <cisstNumerical/nmrExport.h>

// class nmrMultiIndexCounter
// Implements a multi-counter iterator type.
//
// Usage example:
//
//   nmrMultiIndexCounter counter(3);
//   nmrMultiIndexCounter::IndexType lowBounds[] = {0, 0, 0};
//   nmrMultiIndexCounter::IndexType highBounds[] = {2, 3, 4};
//   counter.SetBounds(lowBounds, highBounds);
//
//   // go over all values between lowBounds and highBounds.
//   for(counter.GoToLowBounds(); counter.IsBelowHighBounds(); counter.Increment()) {
//     // use the counter cells, addressing them with operator[]
//   }
//
// Note 1: the bounds are "weak", in the sense that the test is for <= or >=
// rather than the strict < or >.  That is, the above program is equivalent to:
//
// for (i0 = 0; i0 <= 2; ++i0)
//   for (i1 = 0; i1 <= 3; ++i1)
//     for (i2 = 0; i2 <= 4; ++i2) {
//       // do something
//     }
//   }
// }
//
//
// Note 2: Increment() and Decrement() are complementary operations, except for overflow
// and underflow cases. If a counter reaches overflow or underflow after Increment() or
// Decrement(), the result of the opposite operation is COUNTER_UNDEFINED.
class CISST_EXPORT nmrMultiIndexCounter
{
public:
	typedef int IndexType;

	static const IndexType DefaultIncrement;

    enum CounterStatus {COUNTER_UNDERFLOW = -1, COUNTER_IN_BOUNDS = 0, COUNTER_OVERFLOW = 1, COUNTER_UNDEFINED = 2};

	// Initialize the counter by specifying the size (number of cells).
	// The counter is initialized as all zeros, except for the increments which
	// are all DefaultIncrement.
	nmrMultiIndexCounter(unsigned int size);

	// Set all the increments to the default value (1)
	void SetAllIncrements(IndexType increment)
	{
		Increments.assign( GetSize(), increment );
	}

	// return the size (number of cells) of the counter.
	unsigned int GetSize() const
	{
		return Index.size();
	}

	// Set low and high bounds (inclusive!) for the counter.  The low bounds
	// must be <= the high bounds.  In the current implementation, there is
	// no check for it, and the user must take care of it.
	// Set the status to COUNTER_UNDEFINED, since we don't check for the relationship
	// between the actual index and the bounds at this stage.
	// Note that the bounds are "weak", in the sense that the test is for <= or >=
	// rather than the strict < or >.
	void SetBounds(const IndexType lowBounds[], const IndexType highBounds[])
	{
		register int i;
		register const int s = GetSize();
		for (i = 0; i < s; i++) {
			LowBounds[i] = lowBounds[i];
			HighBounds[i] = highBounds[i];
		}
		Status = COUNTER_UNDEFINED;
	}

	// Set the lower bound of a single counter.  'counter' is the zero-based indicator
	// of the counter to be set, 'bound' is the bound value for the counter.
	void SetLowBound(unsigned int counter, IndexType bound)
	{
		LowBounds[counter] = bound;
	}

	// Set the higher bound of a single counter.  'counter' is the zero-based indicator
	// of the counter to be set, 'bound' is the bound value for the counter.
	void SetHighBound(unsigned int counter, IndexType bound)
	{
		HighBounds[counter] = bound;
	}


	// Set the increments for each of the cells. This does not affect the status.
	// The increments have to be positive, although there is no assertion for it in
	// the current implementation.
	void SetIncrements(const IndexType increments[])
	{
		register int i;
		register const int s = GetSize();
		for (i = 0; i < s; i++) {
			Increments[i] = increments[i];
		}
	}

	// Set the increment of a single counter.  'counter' is the zero-based indicator
	// of the counter to be set, 'increment' is the incrementation value for the counter.
	void SetIncrement(unsigned int counter, IndexType increment)
	{
		Increments[counter] = increment;
	}

	// Return the value in a single cell.
	IndexType operator[](unsigned int i) const
	{ return Index[i]; }

	// Return a pointer to the cells as an array, if one wants to initialize other
	// objects with the current values of the cells.
	const IndexType * GetIndexCells() const
	{
		return &(Index[0]);
	}

	// Return the same pointer as GetIndexCells() but pointing unsigned int.
	const unsigned int * GetIndexCellsAsUnsigned() const
	{
		return reinterpret_cast<const unsigned int *>(GetIndexCells());
	}

	const IndexType * GetLowBounds() const
	{
		return &(LowBounds[0]);
	}

	IndexType GetLowBound(unsigned int counter) const
	{
		return LowBounds[counter];
	}

	const IndexType * GetHighBounds() const
	{
		return &(HighBounds[0]);
	}

	IndexType GetHighBound(unsigned int counter) const
	{
		return HighBounds[counter];
	}

	const IndexType * GetIncrements() const
	{
		return &(Increments[0]);
	}

	IndexType GetIncrement(unsigned int counter) const
	{
		return Increments[counter];
	}

	// Set the index of a single cell to a given value without checking the status first.
	// If the new value exceeds the bounds, then the function sets the status to COUNTER_UNDEFINED.
	void SetIndex(unsigned int cell, IndexType value)
	{
		Index[cell] = value;
		if ( (value < LowBounds[cell]) || (HighBounds[cell] < value) )
			Status = COUNTER_UNDEFINED;
	}

	// Update the status of this object: first set it to COUNTER_UNDEFINED. If all the cells are
	// in bounds, set it to COUNTER_IN_BOUNDS.
	void UpdateStatus()
	{
		register int i;
		register const int s = GetSize();
		Status = COUNTER_UNDEFINED;
		for (i = 0; i < s; i++) {
			if ( (Index[i] < LowBounds[i]) || (HighBounds[i] < Index[i]) )
				return;
		}

		Status = COUNTER_IN_BOUNDS;
	}

	/*! Compare the index cells of to multi-index counters.
      The method does not compare bounds, increments, etc. */
	bool IndexIsEqualTo(const nmrMultiIndexCounter & other) const;

	/*! Set the counter to the low bounds specified in SetBounds().
      Set the status to COUNTER_IN_BOUNDS. */
	void GoToLowBounds()
	{
		register int i;
		register const int s = GetSize();
		for (i = 0; i < s; i++) {
			Index[i] = LowBounds[i];
		}
		Status = COUNTER_IN_BOUNDS;
	}


	// Set the counter to the high bounds specified in SetBounds().
	// Set the status to COUNTER_IN_BOUNDS.
	void GoToHighBounds()
	{
		register int i;
		register const int s = GetSize();
		for (i = 0; i < s; i++) {
			Index[i] = HighBounds[i];
		}
		Status = COUNTER_IN_BOUNDS;
	}

    void GoTo(const IndexType where[])
    {
		register int i;
		register const int s = GetSize();
		for (i = 0; i < s; i++) {
			Index[i] = where[i];
		}
        Status = COUNTER_UNDEFINED;
    }

    /*! Increment the counter by the values specified in SetIncements(). 
	  The last index changes fastest. When an index passes the high bound,
	  the previous index is incremented, and if there was an index that was
	  incremented successfully, all the subsequent indices are wrapped back to
	  their low bounds.  When no index can be incremented, the status is set to
	  COUNTER_OVERFLOW.

	  Note that when the status is COUNTER_OVERFLOW, the indices are not determined, as
	  the loop semantics is using <= rather than <.  In the current implementation,
	  COUNTER_OVERFLOW is reached with all indices above their high bounds. Yet this may change
	  in future versions. One should not assume anything about a counter in COUNTER_OVERFLOW
	  or COUNTER_UNDERFLOW status.
    */
	void Increment();

    /*! Decrement the counter by the values specified in SetIncements(). 
	  The last index changes fastest. When an index gets below the low bound,
	  the previous index is decremented, and if there was an index that was
	  decremented successfully, all the subsequent indices are wrapped back to
	  their high bounds.  When no index can be incremented, the status is set to
	  COUNTER_UNDERFLOW.

	  Note that when the status is COUNTER_UNDERFLOW, the indices are not determined, as
	  the loop semantics is using <= rather than <.  In the current implementation,
	  COUNTER_UNDERFLOW is reached with all indices below their low bounds. Yet this may change
	  in future versions. One should not assume anything about a counter in COUNTER_OVERFLOW
	  or COUNTER_UNDERFLOW status.
    */
	void Decrement();

	/*! return one of UNDERLOW, COUNTER_IN_BOUNDS, COUNTER_OVERFLOW, COUNTER_UNDEFINED */
	CounterStatus GetStatus() const
	{
		return Status;
	}

	/*! return true if the status is defined and LowBounds <= Index */
	bool IsAboveLowBounds() const
	{
		return ( (GetStatus() != COUNTER_UNDEFINED) && (GetStatus() != COUNTER_UNDERFLOW) );
	}

	/*! return true if the status is defined and Index <= HighBounds */
	bool IsBelowHighBounds() const
	{
		return ( (GetStatus() != COUNTER_UNDEFINED) && (GetStatus() != COUNTER_OVERFLOW) );
	}

    /*! Return true of the status is COUNTER_IN_BOUNDS */
    bool IsBetweenBounds() const
    {
        return ( GetStatus() == COUNTER_IN_BOUNDS );
    }

	/*! return the total number of possible states, or combinations, of the counter. */
	unsigned long GetNumberOfCombinations() const;

protected:
	typedef std::vector<IndexType> CounterType;
	CounterType Index;
	CounterType LowBounds;
	CounterType HighBounds;
	CounterType Increments;

	CounterStatus Status;

};


#endif
