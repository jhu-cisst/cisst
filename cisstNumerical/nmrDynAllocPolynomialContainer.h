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


#ifndef _nmrDynAllocPolynomialContainer_h
#define _nmrDynAllocPolynomialContainer_h

#include <cisstNumerical/nmrPolynomialContainer.h>
#include <cisstNumerical/nmrExport.h>

class CISST_EXPORT nmrDynAllocPolynomialContainer : public nmrPolynomialContainer<void *>
{
public:
	typedef nmrPolynomialContainer<void *> BaseType;

	nmrDynAllocPolynomialContainer(unsigned int numVariables, unsigned int minDegree, unsigned int maxDegree)
		: BaseType(numVariables, minDegree, maxDegree)
	{}

    virtual void SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const = 0;
    virtual void DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator) = 0;

private:
	nmrDynAllocPolynomialContainer(const nmrDynAllocPolynomialContainer & other);
	nmrDynAllocPolynomialContainer & operator=(const nmrDynAllocPolynomialContainer & other);
};


#endif
