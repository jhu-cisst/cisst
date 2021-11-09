/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2014, Seth Billings, Russell Taylor, Johns Hopkins University
//    All rights reserved.
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions are
//    met:
//
//    1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
//    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ****************************************************************************

#ifndef _msh3Utilities_h
#define _msh3Utilities_h

#include <string>
#include <regex>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

// get current working directory
std::string getcwd_str();

// compute a noise covariance matrix having different noise
//  magnitude parallel vs. perpendicular to a given point
//  normal orientation
vct3x3 ComputePointCovariance( const vct3 &norm, double normPerpVar, double normPrllVar);

// Decompositions for covariance matrices

// eigen values in descending order
void ComputeCovEigenValues_SVD(const vct3x3 &M, vct3 &eigenValues);
// eigen values in descending order
void ComputeCovEigenValues_Trig(const vct3x3 &M, vct3 &eigenValues);

//  eigen values in descending order
//  eigen vectors listed by column and has determinant = 1 (i.e. a rotation matrix)
void ComputeCovEigenDecomposition_NonIter(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors);

//  eigen values in descending order
//  eigen vectors listed by column
void ComputeCovEigenDecomposition_SVD(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors);

// This method is much less efficient than the SVD method
//  eigen values in descending order
//  eigen vectors listed by column
void ComputeCovEigenDecomposition_SEP(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors);

void ComputeCovInverse_NonIter(const vct3x3 &M, vct3x3 &Minv);

void ComputeCovInverse_SVD(const vct3x3 &M, vct3x3 &Minv);

// this is method is 20% slower than the non-iterative method
//  using eigen decomposition, but much faster than the SVD method
void ComputeCovInverse_Nmr(const vct3x3 &M, vct3x3 &Minv);

//--- Inline Functions ---//
//
// must be fully defined in header file
//

// efficient calculation for symmetric form R*M*Rt
//  where M is a symmetric covariance matrix and
//  R is an orthogonal matrix.
// Note: using this form is only slightly more efficient
//       than calling R*M*R.Transpose.
//       Time comparison for 100 trials:
//          time: 0.000241665
//          time: 0.000265216
inline vct3x3& Calc_RMRt(vct3x3 &R, vct3x3 &M)
{
    static vct3x3 RMRt, MRt;
    MRt.ProductOf(M, R.Transpose());
    RMRt.Element(0, 0) = vctDotProduct(R.Row(0), MRt.Column(0));
    RMRt.Element(0, 1) = vctDotProduct(R.Row(0), MRt.Column(1));
    RMRt.Element(1, 0) = RMRt.Element(0, 1);
    RMRt.Element(0, 2) = vctDotProduct(R.Row(0), MRt.Column(2));
    RMRt.Element(2, 0) = RMRt.Element(0, 2);
    RMRt.Element(1, 1) = vctDotProduct(R.Row(1), MRt.Column(1));
    RMRt.Element(1, 2) = vctDotProduct(R.Row(1), MRt.Column(2));
    RMRt.Element(2, 1) = RMRt.Element(1, 2);
    RMRt.Element(2, 2) = vctDotProduct(R.Row(2), MRt.Column(2));
    return RMRt;
}


// Compute rodrigues vector of rotation between two vectors, using an efficient
//  approximation in case of small offset angles
inline vct3 ApproxRodriguesRotationBetweenUnitVectors(double DotProd, const vct3 &XProd)
{
    // Computing magnitude if the angle (alpha) between two vectors
    //   |XProd| = sin(alpha)
    //   DotProd = cos(alpha)
    //
    //   rodriguez vector = XProd.Normalized()*alpha
    //    ~ cannot apply this equation indiscriminantly, however, since the
    //      norm of XProd blows up for approximately parrallel vectors
    //
    //   alpha ~= sin(alpha) for alpha up to 30deg (at 30deg error is approx 5%)
    //     => for alpha <= 30*pi/180, |Xprod| may be used for angle magnitude
    //        and rodriguez vector is simply = XProd vector.
    //     Otherwise, acos(DotProd) should be used for alpha. It is important
    //       to use acos(DotProd) rather than asin(|XProd|) because asin(alpha)
    //       cannot discrimiate between "alpha" and "180deg-alpha", i.e. it cannot
    //       discriminate vectors greater and smaller than 90 degrees, whereas
    //       acos(DotProd) can.    range of asin(|XProd|) = (-90deg,+90deg)
    //                             range of acos(DotProd) = (0deg,+180deg)
    //
    //   In any event, XProd should never be used for the rodriguez vector when
    //     the angle between vectors is more than 90deg, as this will produce
    //     a rodriguez vector for an angle less than 90deg.
    //
    //   Note: For alpha = 30deg, DotProd = cos(alpha*pi/180) = 0.8660
    //
    if (DotProd >= 0.8660) {
        // small angle approximation alpha ~= sin(alpha)
        return XProd;
    }
    else {
        // large angle calculation (XProd.Normalized() won't blow up)
        return XProd.Normalized()*acos(DotProd);
    }
}

// Compute rodrigues vector of rotation between two vectors
inline vct3 RodriguesRotationBetweenUnitVectors(double DotProd, const vct3 &XProd)
{
    // Computing magnitude if the angle (alpha) between two vectors
    //
    //  At alpha = 5deg; error in alpha ~= sin(alpha) approx is about 0.1%
    //
    //   Note: For alpha = 5deg, DotProd = cos(alpha*pi/180) = 0.9962
    //
    if (DotProd >= 0.9962) {
        // small angle approximation alpha ~= sin(alpha)
        return XProd;
    }
    else {
        // large angle calculation (XProd.Normalized() won't blow up)
        return XProd.Normalized()*acos(DotProd);
    }
}

// compute the Jacobian matrices of rotation wrt each element of a Rodrigues vector
//  a    ~ Rodrigues rotation vector
//  dRa  ~ Jacobian of R(a) wrt ax,ay,az
void ComputeRodriguesJacobians(const vct3 &a, vctFixedSizeVector<vctRot3, 3> &dRa);

// Converts from yaw/pitch/roll euler angles (azimuth/elevation/roll) to rotation matrix
//  assumes euler angles with local frame-of-reference
//  assumes angles given in degrees
vctRot3 euler2rot(double yaw, double pitch, double roll);

inline vct3x3 skew(const vct3 &v)
{
    return vct3x3(
                  0.0, -v[2], v[1],
                  v[2], 0.0, -v[0],
                  -v[1], v[0], 0.0
                  );
}
inline void skew(const vct3 &v, vct3x3 &sk)
{
    sk.Assign(
              0.0, -v[2], v[1],
              v[2], 0.0, -v[0],
              -v[1], v[0], 0.0
              );
}
inline void skew(const vct3 &v, vctDynamicMatrixRef<double> &sk)
{
    assert(sk.rows() == 3 && sk.cols() == 3);
    sk.Assign(
              0.0, -v[2], v[1],
              v[2], 0.0, -v[0],
              -v[1], v[0], 0.0
              );
}

inline size_t sub2ind(const vctFixedSizeVector<size_t, 3> & size, const size_t i, const size_t j, const size_t k)
{
    return i + size(0) * j + size(0) * size(1) * k;
}

inline double round6(double d){
    return std::round(d*1E6)/1E6;
}

inline double round3(double d){
    return std::round(d*1E3)/1E3;
}

inline std::string ltrim(const std::string& s) {
    return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

inline std::string rtrim(const std::string& s) {
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

inline std::string trim(const std::string& s) {
    return ltrim(rtrim(s));
}

// Compute the centroid for a set of vetors
vct3 vctCentroid(const vctDynamicVector<vct3>& A);

// Compute the weighted centroid for a set of vectors
vct3 vctWeightedMean(const vctDynamicVector<vct3>& A, const vctDoubleVec &W);

// linspacee
vctDoubleVec linspace(const double &min, const double &max, const int &n);

vctDoubleVec ComputeInverseTrilinearInterp(const vct3 &p, const vctDynamicVector<vct3> &points);

const double m_to_mm=1000.0;

#endif // _msh3Utilities_h
