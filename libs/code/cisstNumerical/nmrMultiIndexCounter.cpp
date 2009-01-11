/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrMultiIndexCounter.cpp,v 1.6 2007/04/26 19:33:57 anton Exp $
  
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

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrMultiIndexCounter.cpp,v $
//  Revision 1.6  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.5  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.2  2004/10/22 01:24:51  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.2  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
