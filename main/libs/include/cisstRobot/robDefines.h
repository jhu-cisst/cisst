
#ifndef _robDefines_h
#define _robDefines_h

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>

#ifdef _MSC_VER
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

namespace cisstRobot{

#ifdef SINGLE_PRECISION
  typedef float Real;
#else
  typedef double Real;
#endif

  typedef vctFrame4x4< Real,VCT_ROW_MAJOR> SE3;
  typedef vctMatrixRotation3<Real,VCT_ROW_MAJOR> SO3;
  typedef vctQuaternionRotation3<Real> Quaternion;
  typedef vctDynamicMatrix<Real> RMxN;
  typedef vctDynamicMatrix<Real> RMxM;
  typedef vctFixedSizeMatrix<Real, 3, 3, VCT_ROW_MAJOR> MOIT;
  typedef vctFixedSizeVector<Real,1> R1;
  typedef vctFixedSizeVector<Real,2> R2;
  typedef vctFixedSizeVector<int,3>  N3;
  typedef vctFixedSizeVector<Real,3> R3;
  typedef vctFixedSizeVector<Real,4> R4;
  typedef vctFixedSizeVector<Real,6> R6;
  typedef vctDynamicVector<Real>     Rn;
  
  enum robError{ SUCCESS, FAILURE };
  
  
  inline Real Saturate( Real x, Real xmin, Real xmax )
  { return (x<xmin) ? xmin : ( (xmax<x) ? xmax : x ); }
  
}

#endif
