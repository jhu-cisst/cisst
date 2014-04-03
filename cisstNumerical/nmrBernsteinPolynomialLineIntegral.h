/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2003-05-06

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrBernsteinPolynomialLineIntegral_h
#define _nmrBernsteinPolynomialLineIntegral_h

#include <cisstNumerical/nmrPolynomialBase.h>
#include <cisstNumerical/nmrBernsteinPolynomial.h>

#include <cisstNumerical/nmrExport.h>

/*!
 Conditional compilation switch to test efficiency of caching
 power-basis objects.
 Having the power basis objects cached reduces the overhead of
 creating them on each evaluation.  But then this object cannot
 be used in a multithreaded enviornment.
*/
#define CACHE_POWER_BASIS 1


/*!
 Conditional compilation switch to test efficiency of storing
 all the summation elements in a tableau
*/
#define USE_INTEGRAND_TABLEAU 1

/*!
  class nmrBernsteinPolynomialLineIntegral

  Evaluates the line integral of a Bernstein polynomial in n variables.
  The class is initialized with the integrand polynomial. To evaluate the
  integral for a segment, call the function EvaluateForSegment() and give the coordinates
  of two points in n-space between which is the line segment to be integrated.

  The integration method iterates for each term of the integrand, and evaluates the
  integral for the term, then sums all the term integrals together. The following
  paragraphs should be compiled by LaTex to be visualized as formulas.

  For the purpose of this discussion we shall use coordinate indexes starting
  with 0 instead of the mathematical convention of 1, to correspond to the
  indexing in C.
  
  Let \f$ E = \{\mathbf{x} \in \mathbf{R}^n | \sum_{i=0}^{n-1} x_i =
  1\} \f$ be the ``barycentric'' plane.  Let \f$ \mathbf{u}_0,
  \mathbf{u}_1 \in E \f$ be two points in the plane.  We want to
  integrate a (Bernstein) polynomial \f$ p(\mathbf{x}) \f$ along the
  line segment \f$ (\mathbf{u}_0, \mathbf{u}_1) \f$.  For simplicity,
  we will show here the formula for the integration of a single term
  of the polynomial, given by \f$ B_{\mathbf{p}}^{d} \f$.  We know
  that we can parametrize \f$ \mathbf{x} \f$ in the segment as:

  \f{eqnarray*}
  \mathbf{x} & = & \mathbf{x}(t) \\
   & = & (1-t)\mathbf{u}_0 + t\mathbf{u}_1
  \f}
  
  We note that \f$\nabla \mathbf{x}(t) = \mathbf{u}_1 -
  \mathbf{u}_0\f$.  To evaluate the integral using the parametrized
  version, we need to multiply the function \f$\mathbf{x}(t)\f$ by the
  norm of the gradient:
  
  \f{eqnarray*}
  \int_{\mathbf{u}_0}^{\mathbf{u}_1} B_{\mathbf{p}}^{d}(\mathbf{x}) d\mathbf{x}
   & = &
  \int_{0}^{1} B_{\mathbf{p}}^{d} (\mathbf{x}(t)) || \nabla \mathbf{x}(t) || dt \\
  & = &
  ||\mathbf{u}_1 - \mathbf{u}_0 || \frac{1}{d+1} \sum_{\mathbf{q} \leq \mathbf{p}} 
  B_{\mathbf{q}}^{|\mathbf{q}|} (\mathbf{u}_0)
  B_{\mathbf{p} - \mathbf{q}}^{|\mathbf{p} - \mathbf{q}|} (\mathbf{u}_1)
  \f}

  where \f$d\f$ is the degree of the Bernstein term; \f$\mathbf{p}\f$,
  \f$\mathbf{q}\f$ are \f$n\f$-length lists of powers of the corresponding
  variables of the polynomial, s.t. \f$|\mathbf{p}| = d\f$; \f$|\cdot|\f$ is
  the operator of summing the elements (powers) in the list; and
  \f$B_{\mathbf{k}}^m(\mathbf{x})\f$ is the Bernstein term
  \f[
  B_\mathbf{k}^m(\mathbf{x}) = \left( \begin{array}{@{}c@{}} m \\ \mathbf{k} \end{array} \right)
  x_0^{k_0} x_1^{k_1} \cdots x_{n-1}^{k_{n-1}}
  \f]

  The formulas above evaluate the line integral when
  \f$\mathbf{u}_0\f$, \f$\mathbf{u}_1\f$, and \f$\mathbf{x}(t)\f$ are
  all in barycentric coordinates.  This formula produces erroneous
  result if we want to compute the integral in the Cartesian space, as
  it does not take the Cartesian distances into account.  For example,
  consider a tetrahedron with a constant density function.  The
  integral between two points which are given in barycentric
  coordinates will be the same regardless of the size of the
  tetrahedron.  But for a tetrahedron twice as large, the segment is
  twice as long, and we integrate the same constant density function
  over a doubled distance, which should have yielded a doubled
  integral value.  To correct this error, all we need is to
  re-formulate the gradient.  Instead of taking the gradient in
  barycentric coordinates, we will take it in Cartesian coordinates.
  Thus the scaling of the integral should change from
  \f$||\mathbf{u}_1 - \mathbf{u}_0 ||\f$ to \f$||\mathbf{x}_1 -
  \mathbf{x}_0 ||\f$, where \f$\mathbf{x}_0\f$ and \f$\mathbf{x}_1\f$
  are the ends of the segment given in Cartesian coordinates.

  For an actual development of this formula, see Russ Taylor's
  technical paper on Bernstein Polynomial Properties.

  \note In the current implementation, we convert directly (using
  pointer cast) between the types 'nmrMultiIndexCounter::IndexType *'
  and 'nmrPolynomialTermPowerIndex::PowerType *'.  Make sure that the
  two types are compatible in size.
*/
class CISST_EXPORT nmrBernsteinPolynomialLineIntegral
{
public:
	// The value evaluated by the integral
	typedef nmrPolynomialBase::ValueType ValueType;

	// The type of value of a variable of the integrand function.
	typedef nmrPolynomialBase::VariableType VariableType;

	typedef nmrPolynomialBase::VariableIndexType VariableIndexType;

	// The type of a coefficient in the integrand function
	typedef nmrPolynomialBase::CoefficientType CoefficientType;

    typedef nmrPolynomialBase::PowerType PowerType;

	typedef nmrBernsteinPolynomial IntegrandType;

	typedef nmrPolynomialTermPowerIndex::MultinomialCoefficientType MultinomialType;


	// See the usage of roundoff tolerance to reduce the number of iterations.
	static const double DefaultRoundoffTolerance;

	// Profiling information that's accumulated during the integration.
	// Counts the invocations of different operations.
    // \note deprecated
	struct ProfilingInfo {
		unsigned long IntegrandTermCounter;
		unsigned long EvaluatePowerProductCounter;
        unsigned long TimeOfSingleTermIntegration;

        ProfilingInfo()
            : IntegrandTermCounter(0)
            , EvaluatePowerProductCounter(0)
            , TimeOfSingleTermIntegration(0)
        {}
	};

	// Initialize the integrator for a specific polynomial.
	nmrBernsteinPolynomialLineIntegral(const IntegrandType & integrand)
		: Integrand(integrand)
#if CACHE_POWER_BASIS
        , PowersAtP0(Integrand.GetNumVariables(), Integrand.GetMaxDegree())
        , PowersAtP1(Integrand.GetNumVariables(), Integrand.GetMaxDegree())
#endif  // CACHE_POWER_BASIS
	{
#if USE_INTEGRAND_TABLEAU
        InitializePolynomialIntegrationTableau(Integrand, IntegrationTableau);
#endif
    }

    /*!  As the integrand may change since the initialization of this object, this
      method lets the user update the integration tableau after changing the integrand
      (e.g., by adding or removing terms).
    */
    void UpdateIntegrationTableau()
    {
        InitializePolynomialIntegrationTableau(Integrand, IntegrationTableau);
    }

    /*! Evaluate the line integral for the segment [p0..p1].  The formulas for the integral
      are in this class's main documentation.

      \param p0 first vertex of the segment, given in barycentric coordinates
      \param p1 second vertex of the segmend, given in barycentric coordinates
      \param gradientNorm the norm of the gradient, as described in the integration
      formula.  As the inputs to this function are given in barycentric coordinates,
      the gradient norm must be provided from outside.  The user may use the length of
      the segment in Cartesian coordinates or in barycentric coordinates.
      \param roundoffTolerance a small positive real number. If the absolute value of 
      a variable is <= roundoffTolerance, it is rounded to zero, and reduces the number
      of iterations required for the integration.
      \param coefficients if this argument is non-null, then our algorithm uses 
      externally defined coefficients, which are stored in this array.  Note that
      the external coefficients must be in the same order as the ones in the 
      polynomial.  See nmrPolynomialBase::EvaluateForCoefficients().  Using external
      coefficients can improve the runtime efficiency when the integrals are computed
      with many sets of coefficients, and then the coefficient sets can be cached
      elsewhere (the polynomial only has one set of coefficients).
    */
	ValueType EvaluateForSegment(
		const VariableType p0[], const VariableType p1[], 
        const VariableType gradientNorm,
		const double roundoffTolerance = DefaultRoundoffTolerance,
        CoefficientType const * coefficients = NULL)
#if !CACHE_POWER_BASIS
        const
#endif
        ;

	const IntegrandType & GetIntegrand() const
	{
		return Integrand;
	}

protected:
	// The integrand polynomial
	const IntegrandType & Integrand;

#if CACHE_POWER_BASIS
    // Auxiliary objects to compute and contain the power basis of the variables
    nmrMultiVariablePowerBasis::StandardPowerBasis PowersAtP0;
    nmrMultiVariablePowerBasis::StandardPowerBasis PowersAtP1;
#endif  // CACHE_POWER_BASIS

    typedef std::vector<PowerType> PowerIndexType;
    typedef struct {
        PowerIndexType P0PowerIndex;
        MultinomialType P0MultinomialFactor;
        PowerIndexType P1PowerIndex;
        MultinomialType P1MultinomialFactor;
    } TermSummationElement;
    typedef std::vector<TermSummationElement> SingleTermIntegrationTableau;

    typedef std::vector<SingleTermIntegrationTableau> PolynomialIntegrationTableau;

    PolynomialIntegrationTableau IntegrationTableau;


	/*! Integration of a single term of the integrand polynomial, using the formula
      developed above.

      \param termCoefficient the coefficient of the integrand term
      \param gradientNorm the norm of the gradient, which is a constant equal to 
      the distance between the vertices of the segment.
      \param termIndex the power index of the integrand term
      \param powersAtP0 pre-computed power basis for all the variables at the "P0"
      vertex.  In old versions, we used nmrStandardPolynomial as a container for
      the powers.  It is now replaced with the independent object 
      nmrMultiVariablePowerBasis.
      \param powersAtP1 pre-computed power basis for all the variables at the "P1"
      vertex.
      \param roundoffTolerance a small positive real number. If the absolute 
      value of a variable is <= roundoffTolerance, it is rounded to zero, and 
      reduces the number of iterations required for the integration.
    */
	static ValueType IntegrateSingleTerm(
		const CoefficientType termCoefficient,
		const CoefficientType gradientNorm,
		const nmrPolynomialTermPowerIndex & termIndex,
		const nmrMultiVariablePowerBasis & powersAtP0,
		const nmrMultiVariablePowerBasis & powersAtP1,
		const double roundoffTolerance = DefaultRoundoffTolerance);



    typedef std::vector<VariableIndexType> IndexContainerForZeroVariables;

	/*! Integration of a single term of the integrand polynomial using a pre-computed
      tableau which stores the power-indices of all the summation elements constituting
      the term.

      \param termCoefficient the coefficient of the integrand term
      \param gradientNorm the norm of the gradient, which is a constant equal 
             to the distance between the endpoints of the segment.
      \param termDegree the degree of the integrand term.  The product
             (gradientNorm * (termDegree+1)) is a denominator of the
             integral.
      \param powersAtP0 pre-computed power basis for all the variables at the 
             "P0" vertex.
	  \param powersAtP1 pre-computed power basis for all the variables at the 
             "P1" vertex.
      \param zerosOfP0 indices of the P0 variables which are equal to zero.  
             Summation element in which the power of a zero variable is positive
             are skipped.
      \param zerosOfP1 indices of the P1 variables which are equal to zero.
	  \param profileInfo (optional) pointer to a struct that stores 
             inegration profiling. 
    */
	static ValueType IntegrateSingleTerm(
		const CoefficientType termCoefficient,
		const CoefficientType gradientNorm,
        const PowerType termDegree,
        const SingleTermIntegrationTableau & termIntegrationTableau,
		const nmrMultiVariablePowerBasis & powersAtP0,
		const nmrMultiVariablePowerBasis & powersAtP1,
        const IndexContainerForZeroVariables & zerosOfP0,
        const IndexContainerForZeroVariables & zerosOfP1);

    static void InitializePolynomialIntegrationTableau( 
        const IntegrandType & integrand,
        PolynomialIntegrationTableau & tableau );

    static void InitializeSingleTermIntegrationTableau(
        const nmrPolynomialTermPowerIndex & termIndex,
        SingleTermIntegrationTableau & termTableau );

};

#endif
