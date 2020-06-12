/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Created on: 2011-05-18

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctEulerRotation3_h
#define _vctEulerRotation3_h

/*!
  \file
  \brief Declaration of vctEulerRotation3
 */

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last!
#include <cisstVector/vctExport.h>

/*!
  \brief Define an Euler angle rotation for a space of dimension 3.

  There are several conventions for Euler angle rotations, which depend on
  the order of rotations about the axes, and whether the rotations are intrinsic
  (i.e., about the body's coordinate frame) or extrinsic (i.e., about the world
  coordinate frame).

  We use the common convention of three letters to define the order of
  (intrinsic) rotations.  For example, ZYZ refers to a rotation of \f$\phi\f$
  (or \f$\alpha\f$) about Z, followed by a rotation of \f$\theta\f$ (or \f$\beta\f$) about
  Y', followed by a rotation of \f$\psi\f$ (or \f$\gamma\f$) about Z''.  Here, the Y' and Z''
  denote that the rotations are about the new (rotated) Y and Z axes, respectively.
  For convenience (and to conform to C++ naming rules), the ' and '' are omitted
  from the naming convention. Note that when composing intrinsic rotations, the order
  is as follows:  \f$R_Z(\alpha) R_Y(\beta) R_Z(\gamma)\f$.
  All angles are in radians.

  The range of angles must still be determined.  One possibility is to have
  \f$\phi\f$ and \f$\psi\f$ be in the range (\f$-\pi\f$, \f$+\pi\f$],
  and \f$\theta\f$ is in the range [0, \f$+\pi\f$].  For now, the IsNormalized method
  always returns true.

  Because there are so many possible Euler angle conventions, we implement a
  base class, vctEulerRotation3, and then specialize it with derived classes
  that are templated by the vctEulerRotation3Order::OrderType enum (see vctForwardDeclarations.h).

  Note that we could have also templated the Euler angle class by element type (double or float),
  but decided that this was not worthwhile -- all Euler angle rotation classes use double.
*/

#ifndef SWIG
// helper functions for subtemplated methods of a templated class

#define VCT_DECLARE_EULER_CONVERSIONS(ORDER) \
    template <class _matrixType> \
    void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
                        const vctMatrixRotation3Base<_matrixType> & matrixRot); \
    template <class _matrixType> \
    void \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
                      vctMatrixRotation3Base<_matrixType> & matrixRot);

VCT_DECLARE_EULER_CONVERSIONS(vctEulerRotation3Order::ZYZ)
VCT_DECLARE_EULER_CONVERSIONS(vctEulerRotation3Order::ZYX)
VCT_DECLARE_EULER_CONVERSIONS(vctEulerRotation3Order::ZXZ)
VCT_DECLARE_EULER_CONVERSIONS(vctEulerRotation3Order::YZX)
#endif


namespace vctEulerRotation3Order {
    std::string CISST_EXPORT ToString(vctEulerRotation3Order::OrderType order);
};


// This base class may not be necessary; if we keep it, it could be moved to vctEulerRotation3Base.h
class CISST_EXPORT vctEulerRotation3Base {
protected:
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<double> TypeTraits;

    vct3 Angles;

    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const CISST_THROW(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctEulerRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const CISST_THROW(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctEulerRotation3Base: Input is not normalized"));
        }
    }

public:
    /*! Constructors */
    inline vctEulerRotation3Base(void) : Angles(0.0, 0.0, 0.0) {}
    inline vctEulerRotation3Base(const vctEulerRotation3Base & other) : Angles(other.Angles) {}
    inline vctEulerRotation3Base(double phi, double theta, double psi) : Angles(phi, theta, psi) {}
    inline vctEulerRotation3Base(double * angles) : Angles(angles) {}
    inline vctEulerRotation3Base(const vct3 & angles) : Angles(angles) {}

    ~vctEulerRotation3Base() {}

    inline double phi(void) const { return Angles[0]; }
    inline double theta(void) const { return Angles[1]; }
    inline double psi(void) const { return Angles[2]; }

    inline double alpha(void) const { return Angles[0]; }
    inline double beta(void) const { return Angles[1]; }
    inline double gamma(void) const { return Angles[2]; }

    /*! Assignment methods */
    void Assign(double phi, double theta, double psi);

    /*! Inverts this rotation */
    vctEulerRotation3Base & InverseSelf(void);

    /*! Normalizes this rotation (ensures angles are within limits) */
    vctEulerRotation3Base & NormalizedSelf(void);

    /*! Test if this rotation is normalized.  This method checks that
      the angles are in the valid range.

      \param tolerance Tolerance for the norm test (not used)
    */
    bool IsNormalized(double tolerance = TypeTraits::Tolerance()) const;

};

// Euler angle class templated by order convention

template <vctEulerRotation3Order::OrderType _order>
class vctEulerRotation3 : public vctEulerRotation3Base {
public:
    typedef vctEulerRotation3<_order> ThisType;
    typedef vctEulerRotation3Base BaseType;

    inline vctEulerRotation3() : BaseType() {}
    inline vctEulerRotation3(const ThisType & other) : BaseType(other) {}
    inline vctEulerRotation3(double phi, double theta, double psi) : BaseType(phi, theta, psi) {}
    inline vctEulerRotation3(double * angles) : BaseType(angles) {}
    inline vctEulerRotation3(const vct3 & angles) : BaseType(angles) {}

    inline ThisType & operator = (const ThisType & other) {
        Angles.Assign(other.Angles);
        return *this;
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctEulerRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation)
        CISST_THROW(std::runtime_error)
    {
        From(matrixRotation);
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctEulerRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation,
                             bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(matrixRotation);
        } else {
            FromRaw(matrixRotation);
        }
    }

    ~vctEulerRotation3() {}

    /*! Conversion from a vctMatrixRotation3. */
    template <class _matrixType>
    ThisType & From(const vctMatrixRotation3Base<_matrixType> & matrixRot) CISST_THROW(std::runtime_error) {
        ThrowUnlessIsNormalized(matrixRot);
        return FromRaw(matrixRot);
    }

    /*! Conversion from a vctMatrixRotation3. */
    template <class _matrixType>
    ThisType & FromNormalized(const vctMatrixRotation3Base<_matrixType> & matrixRot) {
        return FromRaw(matrixRot.Normalized());
    }

    /*! Conversion from a vctMatrixRotation3. */
    template <class _matrixType>
    ThisType & FromRaw(const vctMatrixRotation3Base<_matrixType> & matrixRotation) {
        vctEulerFromMatrixRotation3(*this, matrixRotation);
        return *this;
    }

    /*! Returns the Euler angles in radians */
    const vct3 & GetAngles(void) const { return Angles; }
    vct3 & GetAngles(void) { return Angles; }

    /*! Returns the Euler angles in degrees */
    vct3 GetAnglesInDegrees(void) const { return (180.0/cmnPI)*Angles; }

    /*! Set this rotation as the inverse of another one.  See also
        InverseSelf(). */
    inline ThisType & InverseOf(const ThisType & otherRotation) {
        *this = otherRotation;
        InverseSelf();
        return *this;
    }

    /*! Create and return by copy the inverse of this rotation. */
    inline ThisType Inverse(void) const {
        ThisType result(*this);
        result.InverseSelf();
        return result;
    }

    /*!
      Sets this rotation as the normalized version of another one.

      \param otherRotation Euler rotation used to compute the
      normalized rotation. */
    inline ThisType & NormalizedOf(const ThisType & otherRotation) {
        Angles = otherRotation.Angles;
        NormalizedSelf();
        return *this;
    }

    /*! Returns the normalized version of this rotation.  This method
      returns a copy of the normalized rotation and does not modify
      this rotation.   See also NormalizedSelf(). */
    inline ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }

    /*! Return true if this rotation is exactly equal to the other
      rotation, without any tolerance error.  Rotations may be
      effectively equal if one is elementwise equal to the other.

      \sa AlmostEqual
    */
    //@{
    inline bool Equal(const ThisType & other) const {
        return (this->Angles == other.Angles);
    }

    inline bool operator==(const ThisType & other) const {
        return this->Equal(other);
    }
    //@}


    /*! Return true if this rotation is effectively equal to the other
      rotation, up to the given tolerance.  Rotations may be
      effectively equal if one is elementwise equal to the other.

      The tolerance factor is used to compare each of the elements of
      the difference vector.

      \sa AlmostEquivalent
    */
    inline bool AlmostEqual(const ThisType & other,
                            double tolerance = TypeTraits::Tolerance()) const {
        const vct3 angleDiff(this->Angles - other.Angles);
        return (angleDiff.MaxAbsElement() < tolerance);
    }


    /*! Return true if this rotation is effectively equavilent to the
      other rotation, up to the given tolerance.

      The tolerance factor is used to compare each of the elements of
      the difference vector.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 double tolerance = TypeTraits::Tolerance()) const {
        ThisType thisNorm = this->Normalized();
        return thisNorm.AlmostEqual(other.Normalized(), tolerance);
    }

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    /*!  Print the Euler rotation in a human readable format */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "Euler " << vctEulerRotation3Order::ToString(_order) << ": " << Angles << std::endl;
    }

    /*! Print in machine processable format */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const {
        this->Angles.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "angle-");
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        Angles.SerializeRaw(outputStream);
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        Angles.DeSerializeRaw(inputStream);
    }

};

/*! Define an Euler angle rotation in dimension 3 using ZYZ order. */
typedef vctEulerRotation3<vctEulerRotation3Order::ZYZ> vctEulerZYZRotation3;

/*! Define an Euler angle rotation in dimension 3 using ZYX (yaw-pitch-roll) order. */
typedef vctEulerRotation3<vctEulerRotation3Order::ZYX> vctEulerZYXRotation3;

/*! Define an Euler angle rotation in dimension 3 using ZXZ order. */
typedef vctEulerRotation3<vctEulerRotation3Order::ZXZ> vctEulerZXZRotation3;

/*! Define an Euler angle rotation in dimension 3 using YZX order. */
typedef vctEulerRotation3<vctEulerRotation3Order::YZX> vctEulerYZXRotation3;

#ifndef SWIG
#ifdef CISST_COMPILER_IS_MSVC
// declare instances of helper functions
#define VCT_DECLARE_EULER_CONVERSION_TEMPLATES(ORDER) \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerFromMatrixRotation3(vctEulerRotation3<ORDER> & eulerRot, \
            const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot); \
    template CISST_EXPORT void  \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template CISST_EXPORT void  \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template CISST_EXPORT void  \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRot); \
    template CISST_EXPORT void  \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot,  \
            vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRot); \
    template CISST_EXPORT void \
    vctEulerToMatrixRotation3(const vctEulerRotation3<ORDER> & eulerRot, \
            vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRot);

VCT_DECLARE_EULER_CONVERSION_TEMPLATES(vctEulerRotation3Order::ZYZ)
VCT_DECLARE_EULER_CONVERSION_TEMPLATES(vctEulerRotation3Order::ZYX)
VCT_DECLARE_EULER_CONVERSION_TEMPLATES(vctEulerRotation3Order::ZXZ)
VCT_DECLARE_EULER_CONVERSION_TEMPLATES(vctEulerRotation3Order::YZX)
#endif // CISST_COMPILER_IS_MSVC
#endif // !SWIG

#endif  // _vctEulerRotation3_h
