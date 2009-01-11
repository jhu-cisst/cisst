/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrDynAllocPolynomialContainer.h,v 1.5 2007/04/26 19:33:57 anton Exp $

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

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrDynAllocPolynomialContainer.h,v $
//  Revision 1.5  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.4  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.3  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.2  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.5  2003/12/24 17:57:58  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.4  2003/09/22 18:28:46  ofri
//  1. Replace macro name cmnExport with CISST_EXPORT (following Anton's
//  update).
//  2. Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.3  2003/07/16 22:07:02  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.2  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************

