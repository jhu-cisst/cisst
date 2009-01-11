/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialTermPowerIndex.cpp,v 1.7 2007/04/26 19:33:57 anton Exp $

  Author(s):	Ofri Sadowsky
  Created on:   2001-10-21

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrPolynomialTermPowerIndex.h>
#include <cisstNumerical/nmrPolynomialBase.h>

#include <assert.h>
#include <iostream>

// Ctor: construct a term, defining the number of variables, the minimal degree
// and the maximal degree.
// The defauls values for the parameter are only to enable constructing a default object.
// Be especially careful with the maxDegree argument.
nmrPolynomialTermPowerIndex::
nmrPolynomialTermPowerIndex(VariableIndexType numVariables, 
							PowerType minDegree, 
							PowerType maxDegree)
  : Powers(numVariables, 0)
  , MinDegree(minDegree)
  , MaxDegree(maxDegree)
  , Degree(0)
{}

// Construct a term limited by the data in the polynomial
nmrPolynomialTermPowerIndex::nmrPolynomialTermPowerIndex(const nmrPolynomialBase & p)
  : Powers(p.GetNumVariables(), 0)
  , MinDegree(p.GetMinDegree())
  , MaxDegree(p.GetMaxDegree())
  , Degree(0)
{}


void nmrPolynomialTermPowerIndex::CopyPowers(const nmrPolynomialTermPowerIndex& other)
{
	assert( other.GetNumVariables() == GetNumVariables() );

	VariableIndexType v;
	for (v = 0; v < GetNumVariables(); ++v) {
		SetPower(v, other.GetPower(v));
	}
}

void nmrPolynomialTermPowerIndex::SetDegree(PowerType degree)
{
	GoBegin();
	SetPower( 0, degree );
}

void nmrPolynomialTermPowerIndex::GoBegin()
{
	Powers.assign(GetNumVariables(), 0);
	Degree = 0;
	SetPower(0, GetMinDegree());
}


void nmrPolynomialTermPowerIndex::GoEnd()
{
	Powers.assign(GetNumVariables(), 0);
	Degree = 0;
	SetPower( GetNumVariables() - 1, GetMaxDegree() );
}


// Move to the next possible term.
void nmrPolynomialTermPowerIndex::Increment()
{
    if (Degree < 0) {
        Powers.assign(GetNumVariables(), 0);
        Degree = 0;
        return;
    }

	// store the power of the very last variable.
	PowerType lastPower = GetPower(GetNumVariables() - 1);
	SetPower( GetNumVariables() - 1, 0 );

	int j;
	int beforeLastVariable = -1;
	for (j = GetNumVariables() - 2; j >= 0; j--) {
		if (GetPower(j) > 0) {
			beforeLastVariable = j;
			break;
		}
	}

	if (beforeLastVariable >= 0) {
		// take one power from the beforeLastVariable
		SetPower( beforeLastVariable, GetPower(beforeLastVariable) - 1 );
	}

	// add that power just after beforeLastVariable, plus the lastPower.
	// We know that the power just after was originally zero
	SetPower( beforeLastVariable + 1, 1 + lastPower);
}

// Move to the previous possible term.
void nmrPolynomialTermPowerIndex::Decrement()
{
	if (GetDegree() <= 0) {
        Powers.assign(GetNumVariables(), 0);
        SetPower(GetNumVariables() - 1, -1);
		Degree = -1;
		return;
	}

	int j;
	// find the rightmost variable with a positive power
	VariableIndexType lastVariable = GetNumVariables();
	PowerType lastPower = 0;
	for (j = GetNumVariables() - 1; j >= 0; j--) {
		if (GetPower(j) > 0) {
			lastVariable = j;
			lastPower = GetPower(lastVariable);
			break;
		}

	}

    assert(lastVariable < GetNumVariables());

	// Remove all the power from the last variable
	SetPower( lastVariable, 0 );

	// increment the power on the one-before-last variable
	if (lastVariable > 0)
		SetPower( lastVariable - 1, GetPower(lastVariable - 1) + 1);

	// move the rest of the power to the last possible variable
	SetPower( GetNumVariables() - 1, lastPower - 1 );
}

// enable comparing terms first by degree, then by powers according to the
// order set by the successor function Increment()
int nmrPolynomialTermPowerIndex::Compare(const nmrPolynomialTermPowerIndex& other) const
{
	if (GetNumVariables() < other.GetNumVariables())
		return -1;

	if (GetNumVariables() > other.GetNumVariables())
		return 1;

	if (GetDegree() < other.GetDegree())
		return -1;

	if (GetDegree() > other.GetDegree())
		return 1;

    VariableIndexType v;
	for (v = 0; v < GetNumVariables(); ++v) {
		if (GetPower(v) < other.GetPower(v))
			return 1;

		if (GetPower(v) > other.GetPower(v))
			return -1;
	}

	return 0;
}
		
#if DEFINE_FORMATTED_OUTPUT
char * nmrPolynomialTermPowerIndex::FormatPowers(char * buffer, const char * widthFormat) const
{
	return FormatElementaryTypeContainer( &(Powers[0]), GetNumVariables(), widthFormat, " ", buffer );
}
#endif

#if DEFINE_FORMATTED_OUTPUT
char * nmrPolynomialTermPowerIndex::FormatTermIndex(char * buffer, const char * widthFormat) const
{
	char * initializersFormat = new char[7 + 3 * strlen(widthFormat)];
	sprintf(initializersFormat, "[%s %s %s] ", widthFormat, widthFormat, widthFormat);
	sprintf(buffer, initializersFormat, GetNumVariables(), GetMinDegree(), GetMaxDegree() );
	delete[] initializersFormat;

	buffer += strlen(buffer);
	buffer = FormatPowers(buffer, widthFormat);

	return buffer;
}
#endif

// For a set of powers 'p1',...,'pn' whose sum is 'd', return
// d! / (p1! p2! ... pn!)
nmrPolynomialTermPowerIndex::MultinomialCoefficientType 
nmrPolynomialTermPowerIndex::EvaluateMultinomialCoefficient(
	VariableIndexType numIndices, const PowerType indices[])
{
	MultinomialCoefficientType result = 1;
	PowerType degree = 0;

	VariableIndexType v;
	PowerType j;

	for (v = 0; v < numIndices; ++v) {
		const PowerType p = indices[v];
		for (j = 1; j <= p; ++j) {
			++degree;
			result *= degree;
			result /= j;
		}
	}

	return result; 

}

// For a set of powers 'p1',...,'pn' whose sum is 'd', return
// d! / (p1! p2! ... pn!).
// In this function, d is given as an argument, so there is assumed to
// be an implicit (non-independent) variable in the polynomial.
nmrPolynomialTermPowerIndex::MultinomialCoefficientType
nmrPolynomialTermPowerIndex::EvaluateMultinomialCoefficient(
	VariableIndexType numIndices, PowerType chooseFrom, const PowerType indices[])
{
	MultinomialCoefficientType result = 1;
	PowerType degree = 0;
	PowerType lastDegree = 0;

	VariableIndexType v;
	PowerType j;

	for (v = 0; v < numIndices; ++v) {
		for (j = 1; j <= indices[v]; ++j) {
			++degree;
			result *= degree;
			result /= j;
		}
	}

	lastDegree = chooseFrom - degree;
	for (j = 1; j <= lastDegree; ++j) {
		++degree;
		result *= degree;
		result /= j;
	}

	return result;

}


nmrPolynomialTermPowerIndex::MultinomialCoefficientType
nmrPolynomialTermPowerIndex::CountPowerCombinations() const
{
	// This auxiliary object is used for finding \choose{n+minDegree-1}{minDegree}
	nmrPolynomialTermPowerIndex auxiliaryBinomialTerm(2, 0, GetNumVariables() + GetMinDegree() - 1);
	auxiliaryBinomialTerm.SetPower(0, GetMinDegree());
	auxiliaryBinomialTerm.SetPower(1, GetNumVariables() - 1);
	MultinomialCoefficientType powerCount = auxiliaryBinomialTerm.GetMultinomialCoefficient();

	MultinomialCoefficientType sumCombinations = powerCount;
	PowerType degree;

	for (degree = GetMinDegree(); degree < GetMaxDegree(); ++degree) {
		powerCount = powerCount * (GetNumVariables() + degree) / (degree + 1);
		sumCombinations += powerCount;
	}

	return sumCombinations;
}


nmrPolynomialTermPowerIndex::MultinomialCoefficientType
nmrPolynomialTermPowerIndex::CountLowerOrderTerms() const
{
    const VariableIndexType numVariables = GetNumVariables();
    VariableIndexType varIndex;
    MultinomialCoefficientType result = 1;
    for (varIndex = 0; varIndex < numVariables; ++varIndex)
        result *= GetPower(varIndex) + 1;

    return result;
}


void nmrPolynomialTermPowerIndex::SerializeRaw(std::ostream & output) const
{
    VariableIndexType numVariables = GetNumVariables();
    output.write( (const char *)&numVariables, sizeof(numVariables));

    SerializeIndexRaw(output);

    output.write( (const char *)&MinDegree, sizeof(MinDegree) );
    output.write( (const char *)&MaxDegree, sizeof(MaxDegree) );
}


void nmrPolynomialTermPowerIndex::DeserializeRaw(std::istream & input)
{
    VariableIndexType numVariables;
    input.read( (char *)&numVariables, sizeof(numVariables) );
    Powers.reserve(numVariables);

    DeserializeIndexRaw(input);
    
    input.read( (char *)&MinDegree, sizeof(MinDegree) );
    input.read( (char *)&MaxDegree, sizeof(MaxDegree) );


}


void nmrPolynomialTermPowerIndex::SerializeIndexRaw(std::ostream & output) const
{
    std::vector<PowerType>::const_iterator varIterator = Powers.begin();
    for(; varIterator != Powers.end(); ++varIterator)
        output.write( (const char *)&(*varIterator), sizeof(PowerType) );

}


void nmrPolynomialTermPowerIndex::DeserializeIndexRaw(std::istream & input)
{
    VariableIndexType numVariables = GetNumVariables();
    Powers.clear();
    Degree = 0;
    for (; numVariables > 0; --numVariables) {
        PowerType power;
        input.read( (char *)&power, sizeof(power));
        Powers.push_back(power);
        Degree += power;
    }
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialTermPowerIndex.cpp,v $
//  Revision 1.7  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.6  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.5  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.4  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.3  2004/11/30 20:38:52  ofri
//  nmrPolynomialTermPowerIndex: A few changes to prevent compiler warning
//  about uninitialized variables.
//
//  Revision 1.2  2004/10/22 01:24:51  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.8  2004/08/03 14:50:33  ofri
//  Added method CountLowerOrderTerms
//
//  Revision 1.7  2003/11/18 15:43:42  ofri
//  cisstNumerical: changes related mostly to signed/unsigned MSVC7 warning
//  fixes
//
//  Revision 1.6  2003/08/26 16:29:28  ofri
//  Fixed bugs related to decrement of nmrPolynomialTermPowerIndex under
//  zero degree.
//
//  Revision 1.5  2003/07/21 17:42:22  ofri
//  Extracted method nmrPolynomialPowerTermIndex::SerializeIndexRaw()
//  from SerializeRaw()
//
//  Revision 1.4  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
