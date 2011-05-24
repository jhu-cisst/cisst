/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Created on:	2011-05-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Define an Euler angle rotation for a space of dimension 3.

  There are several conventions for Euler angle rotations, which depend on
  the order of rotations about the axes, and whether the rotations are intrinsic
  (i.e., about the body's coordinate frame) or extrinsic (i.e., about the world
  coordinate frame). Generally, intrinsic rotations are used, though it seems
  that yaw-pitch-roll is usually defined using extrinsic rotations.

  We use the common convention of three letters to define the order of 
  (intrinsic) rotations.  For example, ZYZ refers to a rotation of \f$\phi\f$
  (or \f$\alpha\f$) about Z, followed by a rotation of \f$\theta\f$ (or \f$\beta\f$) about
  Y', followed by a rotation of \f$\psi\f$ (or \f$\gamma\f$) about Z''.  Here, the Y' and Z''
  denote that the rotations are about the new (rotated) Y and Z axes, respectively.
  For convenience (and to conform to C++ naming rules), the ' and '' are omitted
  from the naming convention.

  Because there are so many possible Euler angle conventions, we implement a
  base class, vctEulerRotation3, and then specialize it with derived classes
  that are templated by the vctEulerRotation3::EulerOrder enum (see vctForwardDeclarations.h).

  Note that we could have also templated the Euler angle class by element type (double or float),
  but decided that this was not worthwhile -- all Euler angle rotation classes use double.
*/

// helper functions for subtemplated methods of a templated class

template <vctEulerRotation3Order::OrderType order, class _matrixType>
void
vctEulerRotation3FromRaw(vctEulerRotation3<order> & eulerRot,
                         const vctMatrixRotation3Base<_matrixType> & matrixRot);
                             
template <vctEulerRotation3Order::OrderType order, class _matrixType>
void
vctEulerRotation3ToMatrixRotation3(const vctEulerRotation3<order> & eulerRot,
                                   vctMatrixRotation3Base<_matrixType> & matrixRot);

// This base class may not be necessary; if we keep it, it could be moved to vctEulerRotation3Base.h
class vctEulerRotation3Base {
protected:
    typedef vctEulerRotation3Base ThisType;

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<double> TypeTraits;

    vct3 angles;

    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctEulerRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctEulerRotation3Base: Input is not normalized"));
        }
    }

public:
    /*! Constructors */
    vctEulerRotation3Base() : angles(0.0, 0.0, 0.0) {}
    vctEulerRotation3Base(double phi, double theta, double psi) : angles(phi, theta, psi) {}
    vctEulerRotation3Base(double *a) : angles(a) {}
    vctEulerRotation3Base(const vct3 &a) : angles(a) {}

    ~vctEulerRotation3Base() {}

    double phi() const { return angles[0]; }
    double theta() const { return angles[1]; }
    double psi() const { return angles[2]; }

    double alpha() const { return angles[0]; }
    double beta() const { return angles[1]; }
    double gamma() const { return angles[2]; }

    /*! Normalizes this rotation */
    ThisType & NormalizedSelf(void);

    /*! Test if this rotation is normalized.  This method checks that
      the angles are in the valid range.

      \param tolerance Tolerance for the norm test (not used)
    */
    bool IsNormalized(double tolerance = TypeTraits::Tolerance()) const;

};

// Euler angle class templated by order convention

template <vctEulerRotation3Order::OrderType order>
class vctEulerRotation3<order> : public vctEulerRotation3Base {
    typedef vctEulerRotation3<order> ThisType;
    typedef vctEulerRotation3Base BaseType;

public:
    vctEulerRotation3() : BaseType() {}
    vctEulerRotation3(double phi, double theta, double psi) : BaseType(phi, theta, psi) {}
    vctEulerRotation3(double *a) : BaseType(a) {}
    vctEulerRotation3(const vct3 &a) : BaseType(a) {}

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctEulerRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation)
        throw(std::runtime_error)
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
    ThisType & From(const vctMatrixRotation3Base<_matrixType> & matrixRot) throw(std::runtime_error) {
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
    ThisType &FromRaw(const vctMatrixRotation3Base<_matrixType> & matrixRotation) {
        vctEulerRotation3FromRaw(*this, matrixRotation);
        return *this;
    }

    /*!
      Sets this rotation as the normalized version of another one.

      \param otherRotation Axis angle rotation used to compute the
      normalized rotation. */
    inline ThisType & NormalizedOf(const ThisType & otherRotation) {
        angles = otherRotation.angles;
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
        return (this->angles == other.angles);
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
        const vct3 angleDiff(this->angles - other.angles);
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

};

#endif  // _vctEulerRotation3_h
