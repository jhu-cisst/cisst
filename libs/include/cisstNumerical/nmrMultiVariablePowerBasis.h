/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrMultiVariablePowerBasis.h,v 1.10 2007/04/26 19:33:57 anton Exp $
  
  Author(s):	Ofri Sadowsky
  Created on: 2003-08-20

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrMultiVariablePowerBasis_h
#define _nmrMultiVariablePowerBasis_h

#include <cisstNumerical/nmrPolynomialTermPowerIndex.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrExport.h>

/*!  This class represents the set of variables and variable powers that is
  used in evaluating a polynomial.  The functionality was factored out of the
  original location in the polynomial class, so that the polynomial can serve
  as a term container, and passes by const reference to functions.

  The SetVariable() and SetVariables() functions are declared virtual, to support
  various bases, such as standard basis and barycentric basis.
*/
class CISST_EXPORT nmrMultiVariablePowerBasis
{
public:
    typedef double VariableType;
    typedef nmrPolynomialTermPowerIndex::PowerType PowerType;
    typedef nmrPolynomialTermPowerIndex::VariableIndexType VariableIndexType;
    typedef VariableType ValueType;

protected:
    typedef vctDynamicMatrix<VariableType> BasisContainerType;
    BasisContainerType BasisContainer;

public:
    nmrMultiVariablePowerBasis(VariableIndexType numVariables, PowerType maxPower)
    {
        Initialize(numVariables, maxPower);
    }

    virtual ~nmrMultiVariablePowerBasis()
    {}

    /*! This function initializes the all independent variables to zero and the 
      degree to the given value.  It is to be called during events such as 
      serialization.
    */
    void Initialize(VariableIndexType numVariables, PowerType maxPower);

    VariableIndexType GetNumVariables() const
    { 
        return BasisContainer.rows();
    }

    PowerType GetMaxDegree() const
    {
        return BasisContainer.cols() - 1;
    }

    VariableType GetVariable(VariableIndexType variableIndex) const
    {
        return BasisContainer.Element(variableIndex, 1);
    }

	/*! Retrieve the values of the variables at the current point */
	void GetVariables(VariableType vars[]) const
	{
        vctDynamicVectorRef<VariableType> varsVector(GetNumVariables(), vars);
        varsVector.Assign(BasisContainer.Column(1));
	}

    /*! Return the sum of all the variables */
    VariableType GetSumOfVariables() const
    {
        return BasisContainer.Column(1).SumOfElements();
    }

	/*! Return a specific power of a specific variable */
	VariableType GetVariablePower(VariableIndexType var, PowerType power) const
    {
        return BasisContainer.Element(var, power);
    }

	/*! Set the value of one variable */
	virtual void SetVariable(VariableIndexType varIndex, VariableType value) = 0;

    /*! Set the values of the polynomial variables to a specific n-dimensional
	  point. This enables to pre-calculate the power basis for future 
	   evaluations.
	   Implemented for each concrete polynomial class
    */
	virtual void SetVariables(const VariableType vars[]) = 0;


    /*! Returns true if the value of variable indicated by the index can be set
	  using SetVariable(). */
	virtual bool CanSetVariable(VariableIndexType varIndex) const = 0;

    /*! This function is provided for efficiency reasons.  In some cases, 
      such as computing integrals, the user would want to avoid the overhead 
      of instantiating a nmrPolynomialTermPowerIndex and just evaluate the product,
      say, based on an external table.  In these case, the user can call this
      quick-access method.  Note that this method is not safe in general, since
      it does not perform validity checks on the input set of powers.  It is
      specifically made for highest runtime efficiency -- do not use it with
      untested code.
    */
    ValueType EvaluatePowerProduct(const PowerType powers[]) const;

protected:
    /*! Actually set the value of a variable by index */
	void PrivateSetVariable(VariableIndexType varIndex, VariableType value);

public:
    /*! Standard power basis, where each variable is independent of the others.
      Functionality originally was in nmrStandardPolynomial.
      For now, we declare this class as an inner class of nmrMultiVariablePowerBasis,
      So that we don't have to create so many files...
    */
    class StandardPowerBasis;

    /*!  Barycentric basis -- the sum of all variables is 1.  Functionality originally
      was in nmrBernsteinPolynomial. See notes about StandardPowerBasis
    */
    class BarycentricBasis;
};

class CISST_EXPORT nmrMultiVariablePowerBasis::StandardPowerBasis : public nmrMultiVariablePowerBasis
{
public:
    typedef nmrMultiVariablePowerBasis BaseType;
    StandardPowerBasis(VariableIndexType numVariables, PowerType maxPower)
        : BaseType(numVariables, maxPower)
    {}

    virtual bool CanSetVariable(VariableIndexType varIndex) const
    { return ( (0 <= varIndex) && (varIndex < GetNumVariables()) ); }
    
    virtual void SetVariable(VariableIndexType varIndex, VariableType value)
    {
        CMN_ASSERT(CanSetVariable(varIndex));
        PrivateSetVariable(varIndex, value);
    }
    
    virtual void SetVariables(VariableType const vars[]);
};


class CISST_EXPORT nmrMultiVariablePowerBasis::BarycentricBasis : public nmrMultiVariablePowerBasis
{
protected:
    VariableIndexType ImplicitVarIndex;
public:
    typedef nmrMultiVariablePowerBasis BaseType;
    BarycentricBasis(VariableIndexType numVariables, PowerType maxPower, 
        VariableIndexType implicitVarIndex = -1);

	/*! return the value of the implicit variable. */
	VariableType GetImplicitVariable() const
	{ 
        return GetVariable(ImplicitVarIndex); 
    }

	/*! return the index of the implicit variable */
	VariableIndexType GetImplicitVarIndex() const
	{ 
        return ImplicitVarIndex; 
    }

	/*! choose which of the variables is the implicit one. */
	void SetImplicitVarIndex(VariableIndexType newIndex)
	{ 
		CMN_ASSERT( (0 <= newIndex) && (newIndex < GetNumVariables()) );
		ImplicitVarIndex = newIndex;
	}

    virtual bool CanSetVariable(VariableIndexType varIndex) const
    {
        return ( (0 <= varIndex) && 
            (varIndex < GetNumVariables()) && 
            (varIndex != GetImplicitVarIndex()) );
    }
    
    virtual void SetVariable(VariableIndexType varIndex, VariableType value);
    
    virtual void SetVariables(VariableType const vars[]);
};

#endif

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrMultiVariablePowerBasis.h,v $
//  Revision 1.10  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.9  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.8  2006/09/15 16:31:46  anton
//  nmrMultiVariablePowerBasis.h: Added missing CISST_EXPORT.
//
//  Revision 1.7  2006/07/10 15:31:53  ofri
//  nmrMultiVariablePowerBasis: Added method GetSumOfElements
//
//  Revision 1.6  2006/06/12 19:48:06  ofri
//  nmrMultiVariablePowerBasis: Now using vctDynamicMatrix to cache variable
//  powers, eliminating the use of vctSingleVariablePowerBasis altogether.
//
//  Revision 1.5  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.4  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.3  2004/10/27 02:41:59  anton
//  cisstNumerical: Code to compile as a DLL.  Most of the code was in place but
//  never tested.  Minor additions including to disable some warnings.
//
//  Revision 1.2  2004/10/22 01:24:37  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.6  2003/11/18 15:31:42  ofri
//  no message
//
//  Revision 1.5  2003/09/22 18:28:46  ofri
//  1. Replace macro name cmnExport with CISST_EXPORT (following Anton's
//  update).
//  2. Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.4  2003/08/25 22:16:27  ofri
//  1. Factored out nmrPolynomialBase::EvaluatePowerProduct() to
//  nmrMultiVariablePowerBasis
//  2. Added nmrMultiVariablePowerBasis::GetMaxDegree() to be compatible
//  with the nmrPolynomialBase interface.
//  3. Break lines for better debugging.
//
//  Revision 1.3  2003/08/25 19:43:06  ofri
//  Separating code lines for easier debugging.
//
//  Revision 1.2  2003/08/21 19:04:49  ofri
//  Completed factoring the variable container out of the polynomial classes.
//  In this submission, the user can control conditional compilation of the library
//  with or without the use of the refactored classes, by setting the flag
//  FACTOR_OUT_POLYNOMIAL_VARIABLES .
//  The next step is to run tests and then remove the conditional compilation
//  option.
//  After that, I may completely remove the reference to the variable container
//  from the polynomial class, and have only evaulation parametrized by
//  a variable container.
//
//  Revision 1.1  2003/08/21 18:00:58  ofri
//  Added classes nmrSingleVariablePowerBasis, nmrMultiVariablePowerBasis,
//  nmrMultiVariablePowerBasis::StandardPowerBasis,
//  nmrMultiVariablePowerBasis::BarycentricBasis
//
//
// ****************************************************************************
