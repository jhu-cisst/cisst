/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2016-04-29

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file
    \brief Declaration and implementation of nmrLinearRegression
           and nmrLinearRegressionSolver
*/

#pragma once

#ifndef _nmrLinearRegression_h
#define _nmrLinearRegression_h

#include <vector>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctForwardDeclarations.h>

// Always the last file to include!
// (Not really needed since everything is templated)
#include <cisstNumerical/nmrExport.h>

/*!
  \ingroup cisstNumerical

  This class provides a linear regression solver using a least-squares solution.
  It is primarily designed for ongoing use, where samples arrive at discrete points in time.
  The basic approach is as follows:

  \code
  nmrLinearRegressionSolver<double> lr;
  double x,y;
  double slope, yint, mse;

  while (!done) {
     // Get and sample new data (x,y)
     lr.Sample(x,y);
  }
  lr.Estimate(slope, yint, &mse);
  \endcode

  In addition to the Estimate method, there is also an EstimateAsFractions method that
  returns slope_num, yint_num, denom, and tse_num (note that the types may be different from
  the input data types due to type promotion). In other words:

     \f$ y = (slope_num/denom)*x + (yint_num/denom) \f$

  which can be rewritten as:

     \f$ denom*y = slope_num*x + yint_num \f$

  tse_num is the numerator for the total square error. To get mean square error:

     \f$ mse = tse_num/(N*denom) \f$

  where N is the number of points.

  There are two good reasons for this method:
  <ol>
  <li>The results are always defined (i.e., there is no possible division by 0), so the
      caller can use whatever tolerance is desired. For near vertical lines, it is possible to
      instead compute the parameters of the line \f$ x = (denom/slope_num)*y - (yint_num/slope_num) \f$.
  <li>When this templated class is instantiated for fixed point numbers, such as \c int, the
      estimated slope and yint are also fixed point numbers. But, by returning them as fractions,
      the caller can decide whether to compute the final results as floating point numbers,
      or whether to perform both fixed point multiplication and division.
  </ol>

  The Sample method is overloaded so that it can also accept input vectors, either a fixed
  size vector of size 2 that contains both x and y, or dynamic vectors of multiple x and
  y values.

  All methods return a boolean flag to indicate success or failure. Possible reasons for
  failure include inconsistent vector sizes and near infinite slopes. Note that the check
  for near infinite slopes is based on a specified tolerance value. The default tolerances
  are obtained from cmnTypeTraits. These defaults may be too large for some applications
  (e.g., the default for float is currently 1e-5 and for double it is 1e-9). It is possible
  to specify a new tolerance value in the class constructor, or via the SetTolerance method.

  This class has virtual methods to enable derivation. One possible derivation would be to
  create a class that checks for impending overflow on the accumulators (Sx, Sy, Sxx, Sxy, Syy)
  and returns false in that case. This can be done using cmnTypeTraits<_elementType>::MaxPositiveValue
  and MinNegativeValue. For example, statements such as the following can be added to Sample:
  \code
     if ((Sx > (cmnTypeTraits<_elementType>::MaxPositiveValue()-x)) ||
         (Sx < cmnTypeTraits<_elementType>::MinNegativeValue()+x)) return false;
     if (Sxx > (cmnTypeTraits<_elementType>::MaxPositiveValue()-x*x)) return false;
  \endcode
*/

template <class _elementType>
class nmrLinearRegressionSolver
{
public:
    typedef _elementType ElementType;
    typedef typename cmnTypeTraits<_elementType>::VaArgPromotion SummationType;

    /*! Constructor
        \param tol Tolerance to use when checking for infinite slope (default from cmnTypeTraits)
    */
    nmrLinearRegressionSolver(_elementType tol = cmnTypeTraits<_elementType>::DefaultTolerance)
        : numpts(0), Sx(0), Sy(0), Sxx(0), Sxy(0), Syy(0), tolerance(tol) {}
    virtual ~nmrLinearRegressionSolver() {}

    /*! Returns number of sampled points */
    size_t NumPoints() const { return numpts; }

    /*! Returns the tolerance used when checking for infinite slope. */
    _elementType GetTolerance() const { return tolerance; }

    /*! Set the tolerance used when checking for infinite slope. */
    void SetTolerance(_elementType tol) { tolerance = tol; }

    /*! Initialize the accumulators (i.e., clear all sample points) */
    virtual void Clear()
    { numpts = 0; Sx = Sy = Sxx = Sxy = Syy = 0; }

    /*! Sample the specified x,y pair. Always returns true, but derived
        classes could return false (e.g., if checking for overflow) */
    virtual bool Sample(const _elementType &x, const _elementType &y);

    /*! Sample the specified x,y pair, provided as a vector. */
    virtual bool Sample(const vctFixedSizeVector<_elementType, 2> &in)
    { return Sample(in.X(), in.Y()); }

    /*! Sample the specified x,y pair, provided as a vector ref. */
    virtual bool Sample(const vctFixedSizeConstVectorRef<_elementType, 2, 1> &in)
    { return Sample(in.X(), in.Y()); }

    /*! Sample multiple x and y values, provided as two vctDynamicVectors or
        two vctDynamicVectorRefs. Returns true unless vector sizes are not equal. */
    virtual bool Sample(const vctDynamicVector<_elementType> &x,
                        const vctDynamicVector<_elementType> &y)
    {   return SampleVector<vctDynamicVectorOwner<_elementType> >(x,y); }

    virtual bool Sample(const vctDynamicConstVectorRef<_elementType> &x,
                        const vctDynamicConstVectorRef<_elementType> &y)
    {   return SampleVector<vctDynamicVectorRefOwner<_elementType> >(x,y); }

    /*! Sample multiple x and y values, provided as two std::vector objects.
        Returns true unless vector sizes are not equal. */
    virtual bool Sample(const std::vector<_elementType> &x, const std::vector<_elementType> &y)
    {
        // C++11 introduces std::vector::data, which would allow x.data() instead of &x.front()
        return SampleVector(vctDynamicConstVectorRef<_elementType>(x.size(), &x.front()),
                            vctDynamicConstVectorRef<_elementType>(y.size(), &y.front()));
    }

    /*! Estimate the slope and y intercept as fractions:
          slope = slope_num/denom
          yint = yint_num/denom
          tse = tse_num/denom   (total square error, divide by numpts to get mse)
    */
    virtual bool EstimateAsFractions(SummationType &slope_num, SummationType &yint_num,
                                     SummationType &denom, SummationType *tse_num = 0) const;

    /*! Estimate the slope and y intercept and (optionally) return the mean square error (MSE).
        RMS error is the square root of MSE. */
    virtual bool Estimate(_elementType &slope, _elementType &yint, _elementType *mse = 0) const;    

protected:
    size_t numpts;                // Number of data points
    SummationType Sx, Sy;         // Sums of X and Y values
    SummationType Sxx, Sxy, Syy;  // Sums of X*X, X*Y, and Y*Y
    SummationType tolerance;      // Tolerance for avoiding divide by 0

    /*! Common code to sample multiple x,y pairs, provided as two dynamic vectors.
        Returns true unless vector sizes are not equal. */
    template <class _vectorOwnerType>
    bool SampleVector(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> &x,
                      const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> &y)
    {
        if (x.size() != y.size()) return false;
        numpts += x.size();
        Sx += x.SumOfElements();
        Sy += y.SumOfElements();
        Sxx += x.DotProduct(x);
        Syy += y.DotProduct(y);
        Sxy += x.DotProduct(y);
        return true;
    }

};

template <class _elementType>
bool nmrLinearRegressionSolver<_elementType>::Sample(const _elementType &x, const _elementType &y)
{
    numpts++;
    Sx += x;
    Sy += y;
    Sxx += x*x;
    Sxy += x*y;
    Syy += y*y;
    return true;
}

template <class _elementType>
bool nmrLinearRegressionSolver<_elementType>::EstimateAsFractions(SummationType &slope_num,
                                                                  SummationType &yint_num, SummationType &denom,
                                                                  SummationType *tse_num) const
{
    denom = static_cast<SummationType>(numpts)*Sxx - Sx*Sx;
    slope_num = static_cast<SummationType>(numpts)*Sxy - Sx*Sy;
    yint_num  = Sxx*Sy - Sx*Sxy;
    // Terms in equation for tse_num are grouped to try to reduce likelihood of fixed point overflow
    if (tse_num)
        *tse_num = (Syy*denom - static_cast<SummationType>(numpts)*Sxy*Sxy) - Sy*(yint_num - Sx*Sxy);
    return true;
}

template <class _elementType>
bool nmrLinearRegressionSolver<_elementType>::Estimate(_elementType &slope, _elementType &yint,
                                                       _elementType *mse) const
{
    SummationType slope_num, yint_num, denom, tse_num;
    EstimateAsFractions(slope_num, yint_num, denom, &tse_num);

    // Following check is to avoid division by 0 or by very small number.
    // Note that for fixed point numbers, the default tolerance is 0 so we
    // also check whether we are equal to the tolerance.
    if ((denom >= -tolerance) && (denom <= tolerance))
        return false;

    slope = static_cast<_elementType>(slope_num/denom);
    yint  = static_cast<_elementType>(yint_num/denom);
    if (mse)
        *mse = static_cast<_elementType>(tse_num/(denom*static_cast<SummationType>(numpts)));
    return true;
}

/*!
  \ingroup cisstNumerical
 
  This computes a linear regression using the least-squares solution. If the regression
  is successfully computed, the function returns true; otherwise it returns false. Possible
  reasons for failure are if the x and y vectors are different sizes, if there are fewer
  than 2 points, or if the line is near vertical (near infinite slope). Note that this
  check is based on a specified tolerance value. The default tolerances are obtained from
  cmnTypeTraits. These defaults may be too large for some applications (e.g., the default for
  float is currently 1e-5 and for double it is 1e-9).
 
  \param x A vector of all the x values
  \param y A vector of all the y values
  \param slope Pointer for returning computed slope (if not null)
  \param yint Pointer for returning computed y-intercept (if not null)
  \param mse Pointer for returning mean square error (if not null)
  \param tolerance Tolerance to use when checking for division by (near) 0 (default from cmnTypeTraits)
  \returns true if the linear regression is successful; false otherwise.
*/

template <class _vectorType>
bool nmrLinearRegression(const _vectorType &x, const _vectorType &y,
                         typename _vectorType::value_type &slope, typename _vectorType::value_type &yint,
                         typename _vectorType::value_type *mse = 0,
                         typename _vectorType::value_type tolerance = cmnTypeTraits<typename _vectorType::value_type>::DefaultTolerance)
{
    nmrLinearRegressionSolver<_vectorType::value_type> solver(tolerance);
    bool ret = solver.Sample(x,y);
    if (ret)
       ret = solver.Estimate(slope, yint, mse);
    return ret;
}

#endif
