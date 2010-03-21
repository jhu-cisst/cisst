/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2005-08-25

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctRodriguezRotation3Base_h
#define _vctRodriguezRotation3Base_h

/*!
  \file
  \brief Declaration of vctRodriguezRotation3Base
 */

#include <cisstCommon/cmnConstants.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctForwardDeclarations.h>

#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _rodriguezType, class _matrixType>
void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctMatrixRotation3Base<_matrixType> & matrixRotation);

template <class _rodriguezType, class _matrixType>
void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctQuaternionRotation3Base<_matrixType> & quaternionRotation);

#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template CISST_EXPORT void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template CISST_EXPORT void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template CISST_EXPORT void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


/*!  \brief Define a rotation based on the rodriguez representation
  for a space of dimension 3.

  This representation is based on a
  vector.  The direction of the vector determines the axis of rotation
  and its norm defines the amplitude of the rotation.

  This class is templated by the element type.

  \param _elementType The type of elements.

  \sa vctQuaternion
*/
template <class _containerType>
class vctRodriguezRotation3Base: public _containerType
{
public:
    enum {DIMENSION = 3};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctRodriguezRotation3Base<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: Input is not normalized"));
        }
    }

public:

    /*! Default constructor. Sets the rotation to (0, 0, 0). */
    inline vctRodriguezRotation3Base():
        BaseType(Identity())
    {}

    /*! Constructor from a 3D vector.
    \param axis A vector of size 3.
    */
    template<int __stride, class __dataPtrType>
    inline vctRodriguezRotation3Base(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis):
        BaseType(axis)
    {}

    /*! Constructor from three elements: x, y, and z */
    inline vctRodriguezRotation3Base(value_type x, value_type y, value_type z) {
        this->Assign(x, y, z);
    }

    /*! Const reference to the identity.  In this case, a null
        vector. */
    static CISST_EXPORT const ThisType & Identity();

    template <int __stride, class __dataPtrType>
    inline ThisType & From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        // Always true - ThrowUnlessIsNormalized();
        return (*this);
    }

    inline ThisType & From(value_type x, value_type y, value_type z)
        throw(std::runtime_error)
    {
        FromRaw(x, y, z);
        // Always true - ThrowUnlessIsNormalized();
        return (*this);
    }


    template <class __vectorOwnerType>
    inline ThisType & From(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        // Always true - ThrowUnlessIsNormalized();
        return (*this);
    }


    template <class __containerType>
    inline ThisType & From(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        ThrowUnlessIsNormalized(quaternionRotation);
        return FromRaw(quaternionRotation);
    }


    template <class __containerType>
    inline ThisType & From(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        ThrowUnlessIsNormalized(matrixRotation);
        return FromRaw(matrixRotation);
    }


    inline ThisType & From(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        ThrowUnlessIsNormalized(axisAngleRotation);
        return FromRaw(axisAngleRotation);
    }



    template <int __stride, class __dataPtrType>
    inline ThisType & FromNormalized(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
    {
        FromRaw(vector);
        return NormalizedSelf();
    }


    inline ThisType & FromNormalized(value_type x, value_type y, value_type z)
    {
        FromRaw(x, y, z);
        return NormalizedSelf();
    }


    // might throw because of size
    template <class __vectorOwnerType>
    inline ThisType & FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        return NormalizedSelf();
    }


    template <class __containerType>
    inline ThisType & FromNormalized(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
    {
        return FromRaw(quaternionRotation.Normalized());
    }


    template <class __containerType>
    inline ThisType & FromNormalized(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        return FromRaw(matrixRotation.Normalized());
    }


    inline ThisType & FromNormalized(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        return FromRaw(axisAngleRotation.Normalized());
    }





    template <int __stride, class __dataPtrType>
    inline ThisType & FromRaw(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
    {
        this->Assign(vector);
        return *this;
    }


    inline ThisType & FromRaw(value_type x, value_type y, value_type z)
    {
        this->Assign(static_cast<value_type>(x), static_cast<value_type>(y), static_cast<value_type>(z));
        return *this;
    }


    template <class __vectorOwnerType>
    inline ThisType & FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
    {
        if (vector.size() != 3) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: From requires a vector of size 3"));
        }
        this-Assign(vector);
        return *this;
    }


    template <class __containerType>
    inline ThisType & FromRaw(const vctQuaternionRotation3Base<__containerType> & quaternionRotation) {
        vctRodriguezRotation3BaseFromRaw(*this, quaternionRotation);
        return *this;
    }


    template <class __containerType>
    inline ThisType & FromRaw(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        vctRodriguezRotation3BaseFromRaw(*this, matrixRotation);
        return *this;
    }


    inline ThisType & FromRaw(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        this->Assign(axisAngleRotation.Axis());
        this->Multiply(value_type(axisAngleRotation.Angle()));
        return *this;
    }


    inline ThisType & InverseSelf(void) {
        this->NegationSelf();
        return *this;
    }

    inline ThisType & InverseOf(const ThisType & otherRotation) {
        this->NegationOf(otherRotation);
        return *this;
    }

    inline ThisType Inverse(void) const {
        ThisType result;
        result.NegationOf(*this);
        return result;
    }

    /*! Norm lesser than 2 * PI. */
    inline ThisType & NormalizedSelf(void) {
        const NormType norm = this->Norm();
        if (norm > (2.0 * cmnPI)) {
            const NormType remainder = fmod(norm, (2.0 * cmnPI));
            // const NormType quotient = (norm - remainder) / (2.0 * cmnPI);
            this->Multiply(value_type(remainder / norm));
        }
        return *this;
    }

    /*! See NormalizedSelf */
    inline ThisType & NormalizedOf(const ThisType & otherRotation) {
        this->Assign(otherRotation);
        NormalizedSelf();
        return *this;
    }

    /*! See NormalizedSelf */
    inline ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }

    /*! Test if this rotation is normalized.  This methods always
      return "true" since any angle is considered valid.  This method
      is provided mostly for API completion.

      \param tolerance Tolerance.  This variable is not used as this
      rotation is always normalized.  The tolerance parameter is
      provided just to have the same signature as for other
      transformations.
    */
    inline bool IsNormalized(value_type CMN_UNUSED(tolerance) = TypeTraits::Tolerance()) const {
        return true;
    }


    /*! Return true if this rotation is equivalent to the other
      rotation, up to the given tolerance.  Rotations may be
      effectively equivalent if their unit axis are almost equal and
      the angles are equal modulo 2 PI.

      The tolerance factor is used to compare each of the elements of
      the difference vector.
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        const AngleType angleThis = this->Norm();
        const AngleType angleOther = other.Norm();
        const AngleType dotProduct = AngleType(this->DotProduct(other));
        // two quasi null rotations
        if ((angleThis <= tolerance) && (angleOther <= tolerance)) {
            return true;
        }
        // one rotation is almost null
        if ((angleThis <= tolerance) || (angleOther <= tolerance)) {
            return false;
        }
        // both rotations are non null
        const AngleType cosAngle = dotProduct / (angleThis * angleOther);
        const AngleType absCosAngle = cosAngle > value_type(0.0) ? cosAngle : -cosAngle;
        // axis don't seem aligned
        if ((absCosAngle <= (AngleType(1.0) - tolerance))
            || (absCosAngle >= (AngleType(1.0) + tolerance))) {
            return false;
        }
        // axis are aligned
        const AngleType angleDifference = angleThis - cosAngle * angleOther;
        AngleType angleRemain = fmod(angleDifference, 2.0 * cmnPI);
        if (angleRemain > cmnPI) {
            angleRemain -= (2.0 * cmnPI);
        } else if (angleRemain < -cmnPI) {
            angleRemain += (2.0 *cmnPI);
        }
        if ((angleRemain > -tolerance) && (angleRemain < tolerance)) {
            return true;
        }
        return false;
    }

};

#endif  // _vctRodriguezRotation3Base_h

