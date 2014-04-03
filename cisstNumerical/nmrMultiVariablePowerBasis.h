/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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
