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

#include <cisstMesh/msh3Utilities.h>

#include <cisstVector/vctDeterminant.h>
#include <cisstVector/vctDynamicMatrixTypes.h>

#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrInverse.h>
#include <cisstNumerical/nmrPInverse.h>
#include <cisstNumerical/nmrSymmetricEigenProblem.h>

// #include <algorithm>
// #include <limits>

// getcwd
#include <errno.h>
#ifdef _WIN32
  #include <direct.h>;  // for _getcwd
  #define getcwd _getcwd
#else
  #include <unistd.h>
#endif

// uncomment this define if not using the WildMagic5 library
#define REDIRECT_COV_EIGENDECOMP_NONITER_TO_SVD

#ifndef REDIRECT_COV_EIGENDECOMP_NONITER_TO_SVD
  #include "Wm5NoniterativeEigen3x3.h"  // WildMagic5
#endif

//#define ENABLE_UTILITIES_DEBUG


// get current working directory
std::string getcwd_str()
{
  const size_t chunkSize = 512;

  char stackBuffer[chunkSize]; // Stack buffer for the "normal" case
  if (getcwd(stackBuffer, sizeof(stackBuffer)) == NULL)
  {
    if (errno != ERANGE)
    {
      // It's not ERANGE, so we don't know how to handle it
      throw std::runtime_error("Cannot determine the current path.");
    }
  }
  return stackBuffer;
}

//std::string getcwd_str()
//{
//  const size_t chunkSize = 255;
//  const int maxChunks = 10240; // 2550 KiBs of current path are more than enough
//
//  char stackBuffer[chunkSize]; // Stack buffer for the "normal" case
//  if (getcwd(stackBuffer, sizeof(stackBuffer)) != NULL)
//    return stackBuffer;
//  if (errno != ERANGE)
//  {
//    // It's not ERANGE, so we don't know how to handle it
//    throw std::runtime_error("Cannot determine the current path.");
//  }
//  // Ok, the stack buffer isn't long enough; fallback to heap allocation
//  for (int chunks = 2; chunks<maxChunks; chunks++)
//  {
//    // With boost use scoped_ptr; in C++0x, use unique_ptr
//    // If you want to be less C++ but more efficient you may want to use realloc
//    std::auto_ptr<char> cwd(new char[chunkSize*chunks]);
//    if (_getcwd(cwd.get(), chunkSize*chunks) != NULL)
//      return cwd.get();
//    if (errno != ERANGE)
//    {
//      // It's not ERANGE, so we don't know how to handle it
//      throw std::runtime_error("Cannot determine the current path.");
//    }
//  }
//  throw std::runtime_error("Cannot determine the current path; the path is apparently unreasonably long");
//}
//#else
//  #include <unistd.h>
//  // get current working directory
//  std::string getcwd_str()
//  {
//    char buffer[512];
//    std::string s_cwd;
//    // TODO: free cwd?
//    char *cwd = getcwd(buffer, sizeof(buffer));
//    if (cwd)
//    {
//      s_cwd = cwd;
//    }
//
//    return s_cwd;
//  }
//#endif


// compute a noise covariance matrix having different noise
//  magnitude in-plane vs. out-of-plane for the given plane norm
vct3x3 ComputePointCovariance(const vct3 &norm, double normPrllVar, double normPerpVar)
{
  vct3x3 M, M0;
  vctRot3 R;
  vct3 z(0.0, 0.0, 1.0);

  // set eigenvalues of noise covariance
  //  set plane perpendicular noise component along z-axis
  M0.SetAll(0.0);
  M0.Element(0, 0) = normPerpVar;
  M0.Element(1, 1) = normPerpVar;
  M0.Element(2, 2) = normPrllVar;

  // find rotation to align normal vector with the z-axis
  vct3 xProd = vctCrossProduct(norm, z);
  if (xProd.Norm() <= 1e-6)  // protect from divide by zero
  { // norm is already oriented with z-axis
    R = vctRot3::Identity();
  }
  else
  {
    // the norm of the cross product is the same for angles of x deg & x+180 deg
    //  between two vectors => use dot product to determine the angle
    //   NOTE: the angle corresponding to the cross product axis is always > 0;
    //         acos of the dot product gives the correct form
    //   NOTE: the problem with using norm of cross product isn't that we aren't
    //         going the right direction, but rather that we don't rotate far enough
    //         if A & B are seperated by more than 90 degrees.  I.e. if angular
    //         seperation between A & B is 100 degrees, then asin(norm(AxB)) gives
    //         the same angle as if A & B are seperated by 80 degrees => the
    //         separation angle is ambiguous using the norm of cross product.
    vct3 ax = xProd.Normalized();
    double an = acos(vctDotProduct(norm, z));
    //double an = asin(t.Norm());
    vctAxAnRot3 R_AxAn(ax, an);
    R = vctRot3(R_AxAn);
  }

  // compute noise covariance M of this sample and its decomposition:
  //    M = U*S*V'
  // rotate to align normal with z-axis, apply noise covariance, rotate back
  M = R.Transpose()*M0*R;

  return M;
}

void ComputeCovEigenDecomposition_NonIter(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors)
{
#ifdef REDIRECT_COV_EIGENDECOMP_NONITER_TO_SVD

  // redirect to non-iterative SVD routine
  ComputeCovEigenDecomposition_SVD(M, eigenValues, eigenVectors);

#else
  // Calls the non-iterative eigen solver of the WildMagic5 library
  //
  // eigen values in descending order
  // eigen vectors listed by column   (has determinant = 1)
  //

  bool rowMajor = 1;
  Wm5::Matrix3<double> A(M.Pointer(0,0), rowMajor);

  Wm5::NoniterativeEigen3x3<double> solver(A);

  //  change order from ascending to descending
  eigenValues.Assign(
    solver.GetEigenvalue(2),
    solver.GetEigenvalue(1),
    solver.GetEigenvalue(0));
  eigenVectors.Column(0).Assign(solver.GetEigenvector(2));
  eigenVectors.Column(1).Assign(solver.GetEigenvector(1));
  eigenVectors.Column(2).Assign(solver.GetEigenvector(0));

  // check that eigen vector matrix is a proper rotation matrix
  //  NOTE: this is probably not important, but a nice property to have
  double det =
    solver.GetEigenvector(2).Dot(solver.GetEigenvector(1).Cross(
    solver.GetEigenvector(0)));
  if (det < 0.0)
  {
    eigenVectors.Column(2).Multiply(-1.0);
  }
#endif // REDIRECT_COV_EIGENDECOMP_NONITER_TO_SVD
}


void ComputeCovEigenDecomposition_SVD(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors)
{
  // NOTE: for a covariance (positive-definite) matrix, the singular values are
  //       synonymous with the eigenvalues since all eigenvalues must be positive

  //
  // eigen values in descending order
  // eigen vectors listed by column
  //

  // Compute SVD of M
  //   M = U*diag(S)*V'   where U = V
  //
  //  NOTE: matrices must be column major
  //
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Mcopy;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> U;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Vt;
  static nmrSVDFixedSizeData<3, 3, VCT_COL_MAJOR>::VectorTypeWorkspace workspace;
  try
  {
    Mcopy.Assign(M);  // must use "assign" rather than equals to properly transfer between different vector orderings
    nmrSVD(Mcopy, U, eigenValues, Vt, workspace);
}
  catch (...)
  {
    std::cout << std::endl << "========> ERROR: ComputeCovEigenDecomposition_SVD() failed!" << std::endl << std::endl;
    assert(0);
  }

  // copy eigen vectors to output using "assign" to preserve ordering
  //  of output matrix
  eigenVectors.Assign(U);

#ifdef ENABLE_UTILITIES_DEBUG
  if (eigenValues(2) < 0.0 || eigenValues(2) > eigenValues(1) || eigenValues(1) > eigenValues(0))
  {
    std::cout << std::endl << "========> ERROR: ComputeCovEigenDecomposition_SVD() eigen values misordered or less than zero!" << std::endl << std::endl;
    assert(0);
  }
#endif
}

void ComputeCovEigenDecomposition_SEP(const vct3x3 &M, vct3 &eigenValues, vct3x3 &eigenVectors)
{
  //
  // Symmetric Eigenproblems (SEP)
  //
  // Computes all the eigen values and eigen vectors of a symmetric matrix
  //   A = V D V^T. The eigen values are sorted in ascending order. This
  //   function uses LAPACK dsyevr.
  //
  //   eigen values in descending order
  //   eigen vectors listed by column
  //
  // This method is much less efficient than the SVD method
  //   SVD: 6.42449 (sec)
  //   SEP: 9.30579 (sec)
  //

  static vctDynamicMatrix<double> Mcopy(3, 3, VCT_COL_MAJOR);
  static vctDynamicMatrix<double> eigVct(3, 3, VCT_COL_MAJOR);
  static vctDynamicVector<double> eigVal(3);
  static nmrSymmetricEigenProblem::Data workspace = nmrSymmetricEigenProblem::Data(Mcopy, eigVal, eigVct);

  Mcopy.Assign(M);
  if (nmrSymmetricEigenProblem::EFAILURE == nmrSymmetricEigenProblem(Mcopy, eigVal, eigVct, workspace))
  {
    std::cout << std::endl << "========> ERROR: ComputeCovEigenDecomposition_SEP() failed!" << std::endl << std::endl;
    assert(0);
  }

  // assign to fixed size containers
  //  change order from ascending to descending
  eigenValues.Assign(eigVal[2],eigVal[1],eigVal[0]);
  eigenVectors.Column(0).Assign(eigVct.Column(2));
  eigenVectors.Column(1).Assign(eigVct.Column(1));
  eigenVectors.Column(2).Assign(eigVct.Column(0));
}

void ComputeCovInverse_NonIter(const vct3x3 &M, vct3x3 &Minv)
{
  vct3    eigenValues;
  vct3x3  eigenVectors;

  // Compute Minv
  //   M = U*diag(S)*V'   where U = V
  //   Minv = V*diag(1/S)*V'

  ComputeCovEigenDecomposition_NonIter(M, eigenValues, eigenVectors);

  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> V_Sinv;
  static vct3 Sinv;
  Sinv[0] = 1.0 / eigenValues[0];
  Sinv[1] = 1.0 / eigenValues[1];
  Sinv[2] = 1.0 / eigenValues[2];
  V_Sinv.Column(0) = eigenVectors.Column(0)*Sinv[0];
  V_Sinv.Column(1) = eigenVectors.Column(1)*Sinv[1];
  V_Sinv.Column(2) = eigenVectors.Column(2)*Sinv[2];
  Minv.Assign(V_Sinv * eigenVectors.TransposeRef());
}

void ComputeCovInverse_SVD(const vct3x3 &M, vct3x3 &Minv)
{
  // Compute SVD of M
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Mcopy;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> U;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Vt;
  static vct3 S;
  static nmrSVDFixedSizeData<3, 3, VCT_COL_MAJOR>::VectorTypeWorkspace workspace;
  try
  {
    Mcopy.Assign(M);
    nmrSVD(Mcopy, U, S, Vt, workspace);
  }
  catch (...)
  {
    assert(0);
  }

  // Compute Minv
  //   M = U*diag(S)*V'   where U = V
  //   Minv = V*diag(1/S)*U' = U*diag(1/S)*V'
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Sinv_Ut;
  static vct3 Sinv;
  Sinv[0] = 1.0 / S[0];
  Sinv[1] = 1.0 / S[1];
  Sinv[2] = 1.0 / S[2];
  Sinv_Ut.Row(0) = Sinv[0] * Vt.Row(0);
  Sinv_Ut.Row(1) = Sinv[1] * Vt.Row(1);
  Sinv_Ut.Row(2) = Sinv[2] * Vt.Row(2);
  Minv.Assign(U*Sinv_Ut);
}

// this is only slightly slower than the non-iterative
//  method based on eigen decomposition, but much faster
//  than the SVD method
void ComputeCovInverse_Nmr(const vct3x3 &M, vct3x3 &Minv)
{
  Minv = M;
  nmrInverse(Minv); // computes inverse in-place
}

void ComputeCovEigenValues_SVD(const vct3x3 &M, vct3 &eigenValues)
{
  // NOTE: for a covariance (positive-definite) matrix, the singular values are
  //       synonymous with the eigenvalues since all eigenvalues must be positive

  // Compute SVD of M
  //   M = U*diag(S)*V'   where U = V
  //
  //  NOTE: matrices must be column major
  //        eigen values are in descending order
  //
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Mcopy;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> U;
  static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> Vt;
  static nmrSVDFixedSizeData<3, 3, VCT_COL_MAJOR>::VectorTypeWorkspace workspace;
  try
  {
    Mcopy.Assign(M);
    nmrSVD(Mcopy, U, eigenValues, Vt, workspace);
  }
  catch (...)
  {
    std::cout << std::endl << "========> ERROR: ComputeCovEigenValues_SVD() failed!" << std::endl << std::endl;
    assert(0);
  }

}

void ComputeCovEigenValues_Trig(const vct3x3 &M, vct3 &eigenValues)
{
  //
  // Code derived from an algorithm posted on Wikipedia for computing
  //  eigenvalues of a 3x3 real symmetric matrix based on the article:
  //
  //  Oliver Smith, "Eigenvalues of a symmetric 3x3 matrix",
  //   J. Comm. of ACS Vol. 4, Issue 4, pg. 168, 1961
  //
  //  NOTE: eigen values are in descending order
  //

  static vctDeterminant<3> detCalc;

  double p, p1, p2;
  double q, q1, q2, q3;
  double r, phi;
  double eig1, eig2, eig3;
  vct3x3 B;

  p1 = M.Element(0, 1) * M.Element(0, 1)
    + M.Element(0, 2) * M.Element(0, 2)
    + M.Element(1, 2) * M.Element(1, 2);

  if (p1 <= std::numeric_limits<double>::epsilon() * 10.0)
  {
    // M is diagonal
    eig1 = M.Element(0, 0);
    eig2 = M.Element(1, 1);
    eig3 = M.Element(2, 2);

    // sort eigenvalues in descending order
    if (eig1 > eig2)
    {
      if (eig3 > eig1) std::swap(eig1, eig3);
    }
    else
    {
      if (eig2 > eig3) std::swap(eig1, eig2);
      else std::swap(eig1, eig3);
    }
    // now eig1 is largest; order the remaining two
    if (eig3 > eig2) std::swap(eig2, eig3);
  }
  else
  {
    q = M.Trace() / 3.0;
    q1 = M.Element(0, 0) - q;
    q2 = M.Element(1, 1) - q;
    q3 = M.Element(2, 2) - q;
    p2 = q1*q1 + q2*q2 + q3*q3 + 2.0 * p1;
    p = sqrt(p2 / 6.0);
    B = (1.0 / p) * (M - q * vct3x3::Eye());
    r = detCalc.Compute(B) / 2.0;

    // In exact arithmetic for a symmetric matrix - 1 <= r <= 1
    //  but computation error can leave it slightly outside this range.
    if (r <= -1.0)
    {
      phi = cmnPI / 3.0;
    }
    else if (r >= 1)
    {
      phi = 0.0;
    }
    else
    {
      phi = acos(r) / 3.0;
    }

    // the eigenvalues satisfy eig1 >= eig2 >= eig3
    eig1 = q + 2.0 * p * cos(phi);
    eig3 = q + 2.0 * p * cos(phi + (2.0 * cmnPI / 3.0));
    eig2 = 3.0 * q - eig1 - eig3;   // since trace(A) = eig1 + eig2 + eig3
  }

  eigenValues.Assign(eig1, eig2, eig3);
}

// a    ~ Rodrigues rotation vector
// dRa  ~ Jacobian of R(a) wrt ax,ay,az
void ComputeRodriguesJacobians(const vct3 &a, vctFixedSizeVector<vctRot3, 3> &dRa)
{
  vct3 alpha;         // alpha = a/norm(a)
  double theta;       // theta = norm(a)
  vct3x3 sk_alpha, AlphaAlphaT_I, aaT, theta_daat_adat;
  double sTheta, cTheta, theta2, theta3, dTheta;
  vct3 gTheta;
  double EPS = 1.0e-14;
  // partial differentials for elements of rodrigues vector a
  const vctFixedSizeVector<vct3, 3> da(vct3(1.0, 0.0, 0.0), vct3(0.0, 1.0, 0.0), vct3(0.0, 0.0, 1.0));

  theta = a.Norm();
  if (theta < EPS)    // prevent division by zero
  {
    alpha.SetAll(0.0);
    theta = 0.0;
  }
  else
  {
    alpha = a / theta;
  }

  AlphaAlphaT_I.OuterProductOf(alpha, alpha);
  AlphaAlphaT_I.Subtract(vct3x3::Eye());
  aaT.OuterProductOf(a, a);
  sk_alpha = skew(alpha);
  sTheta = sin(theta);
  cTheta = cos(theta);
  theta2 = theta*theta;
  theta3 = theta2*theta;

  // gradient of theta wrt rodrigues vector a is simply alpha
  gTheta = alpha;

  // compute the jacobian of the rotation matrix wrt to each
  // rodrigues vector component: ax,ay,az
  //  Note: rather than represent the Jacobian as a single
  //        9x3 matrix (9 values in R wrt each of 3 Rodrigues elements)
  //        the Jacobian is represented as 3 3x3 matrices, with
  //        each matrix representing change in R wrt a different
  //        Rodrigues element
  double sTheta_theta2 = sTheta / theta2;               // these are used each iteration
  double one_cTheta_theta3 = (1.0 - cTheta) / theta3;   //  ''
  vct3   theta_a = theta*a;                             //  ''
  for (unsigned int i = 0; i < 3; i++)
  {
    if (theta == 0.0)
    { // Jacobian simplifies to skew(alpha) where alpha
      //  is in the direction of the rodrigues differential
      dRa[i] = skew(da[i]);
    }
    else
    {
      dTheta = gTheta[i];  // dTheta = InnerProduct(gTheta,da[i]) = gTheta[i] = alpha[i]
      theta_daat_adat.SetAll(0.0);            // daat_adat = da*a' + a*da';
      theta_daat_adat.Row(i) = theta_a;       //  ''
      theta_daat_adat.Column(i) += theta_a;   //  ''
      dRa[i] = (cTheta*dTheta)*sk_alpha
        + (sTheta_theta2)*skew(theta*da[i] - dTheta*a)
        + (sTheta*dTheta)*AlphaAlphaT_I
        + (one_cTheta_theta3)*(theta_daat_adat - (2.0*dTheta)*aaT);
    }
  }
}

// Compute the centroid for a set of vetors
vct3 vctCentroid(const vctDynamicVector<vct3>& A)
{
  vct3 mean(0.0);
  unsigned int N = A.size();
  for (unsigned int i = 0; i < N; i++)
  {
    mean.Add(A[i]);
  }
  mean.Divide(N);
  return mean;
};

// Compute weighted vector mean:  Amean = Sum(Wi*Ai)/Sum(Wi)
vct3 vctWeightedMean(const vctDynamicVector<vct3>& A, const vctDoubleVec &W)
{
  vct3 mean(0.0);
  unsigned int N = A.size();
  assert(N == W.size());
  for (unsigned int i = 0; i < N; i++)
  {
    mean.AddProductOf(W[i], A[i]);
  }
  mean.Divide(W.SumOfElements());
  return mean;
};

vctDoubleVec linspace(const double &min, const double &max, const int &n)
{
    vctDoubleVec result(n);
    for (int i = 0; i <= n-2; i++) {
        double temp = min + i*(max-min)/(floor((double)n) - 1);
        result.at(i) = temp;
    }
    result.at(n-1) = max;

    return result;
}

vctDoubleVec ComputeInverseTrilinearInterp(const vct3 &p, const vctDynamicVector<vct3> &points)
{
    assert(points.size() == 8);
    vctDoubleMat A(3,7), Ainv(7,3);
    for (int i = 0; i < 7; ++i) {
        A.Column(i).Assign(vctDoubleVec(points(i+1) - points(0)));
    }
    vctDoubleMat B(3,1);
    B.Column(0).Assign(p - points(0));

    nmrPInverse(A, Ainv);
    vctDoubleMat result(7,1);
    result = Ainv * B;

    vctDoubleVec weights(8,0.0);
    weights(0) = 1 - result.Column(0).SumOfElements();
    for (size_t i = 1; i < weights.size(); ++i) {
        weights(i) = result.Column(0).at(i-1);
    }

    return weights;
}
