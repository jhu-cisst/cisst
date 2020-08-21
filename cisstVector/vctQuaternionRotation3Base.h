/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2005-08-24

  (C) Copyright 2005-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctQuaternionRotation3Base_h
#define _vctQuaternionRotation3Base_h

/*!
  \file
  \brief Declaration of vctQuaternionRotation3Base
 */

#include <cisstVector/vctQuaternionBase.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>
#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctFixedSizeMatrix.h>

#include <cisstVector/vctExport.h>

#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _quaternionType, class _matrixType>
void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<_quaternionType> & quaternionRotation,
                                  const vctMatrixRotation3Base<_matrixType> & matrixRotation);

#endif // SWIG
#endif // DOXYGEN


/*!
  \brief Define a rotation quaternion for a space of dimension 3

  This class is templated by the element type.  It is derived from
  vctQuaternionBase.  This class is templated by the type of container
  used to store the quaternion.  It is an "inner" class of the
  library, i.e. a regular user should use either the predefined type
  ::vctQuatRot3 or the class vctQuaternionRotation3.

  \param _containerType The type of container used for the quaternion.

  \sa vctQuaternionRotation3, vctQuaternionBase
*/
template <class _containerType>
class vctQuaternionRotation3Base: public vctQuaternionBase<_containerType>
{
public:
    enum {SIZE = 4};
    enum {DIMENSION = 3};

    /*! Type of base class. */
    typedef vctQuaternionBase<_containerType> BaseType;

    /*! Type of container.  This must be a vector of 4 elements
      (either fixed size or dynamic).  In any case, this class should
      only be used by the library programmers. */
    typedef _containerType ContainerType;
    typedef vctQuaternionRotation3Base<_containerType> ThisType;

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const CISST_THROW(std::runtime_error) {
        if (! this->IsNormalized()) {
            cmnThrow(std::runtime_error("vctQuaternionRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const CISST_THROW(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctQuaternionRotation3Base: Input is not normalized"));
        }
    }

    /*!  Allocate memory for the underlying container if needed.  By
      default, this methods does nothing.  For any container requiring
      a memory allocation, it is necessary to specialize this
      method. */
    inline void Allocate(void) {}

public:


    /*! Default constructor. Sets the rotation quaternion to identity. */
    inline vctQuaternionRotation3Base(void)
    {
        this->Allocate();
        this->Assign(ThisType::Identity());
    }

    /*!
      Copy constructor.
      \param quaternionRotation A rotation quaternion.
    */
    inline vctQuaternionRotation3Base(const ThisType & quaternionRotation):
        BaseType()
    {
        this->Allocate();
        this->Assign(quaternionRotation);
    }

    inline ThisType & operator = (const ThisType & quaternionRotation) {
        this->Allocate();
        this->Assign(quaternionRotation);
        return *this;
    }

    /*!
      \name Constructors with normalization test.

      These constructors will check that the input is valid,
      i.e. normalized.  If the input is not normalized, an exception
      (of type \c std::runtime_error) will be thrown.  Each
      constructor uses the corresponding From() method based on the
      input type.

      \note See the cmnThrow() function if an \c abort is better than
      an exception for your application.
    */
    //@{

    /*!
      Constructor from 4 elements.
      \param x First component of the real part.
      \param y Second component of the real part.
      \param z Third component of the real part.
      \param r The imaginary part.
    */
    inline
    vctQuaternionRotation3Base(const value_type & x, const value_type & y, const value_type & z,
                               const value_type & r) CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(x, y, z, r);
    }

    /*!
      Constructor from a rotation matrix.
      \param matrixRotation A rotation matrix.
    */
    template <class __containerType>
    inline explicit
    vctQuaternionRotation3Base(const vctMatrixRotation3Base<__containerType> & matrixRotation) CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(matrixRotation);
    }

    /*!
      Constructor from an axis and angle.
      \param axisAngleRotation An axis/angle rotation.
     */
    inline
    vctQuaternionRotation3Base(const vctAxisAngleRotation3<value_type> & axisAngleRotation) CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(axisAngleRotation);
    }

    /*!
      Constructor from a Rodriguez rotation.
      \param rodriguezRotation A Rodriguez rotation.
    */
    template <class __containerType>
    inline
    vctQuaternionRotation3Base(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(rodriguezRotation);
    }
    //@}




    /*!
      \name Constructors without normalization test

      These constructors will either assume that the input is
      normalized or normalize the input (a copy of it, if required)
      based on the last parameter provided.

      <ul>

      <li>If the normalization flag is set to #VCT_DO_NOT_NORMALIZE,
      the input is considered already normalized and the constructor
      will not perform any sanity check.  This can lead to numerical
      instabilities which have to be handled by the caller.

      <li>If the normalization flag is set to #VCT_NORMALIZE, the
      input will be normalized.  This option should be used whenever
      it is important to obtain a result as "normalized" as possible.

      </ul>
    */
    //@{

    /*!
      Copy constructor.
      \param quaternionRotation A rotation quaternion.
      \param normalizeInput Force the input normalization.
    */
    inline
    vctQuaternionRotation3Base(const ThisType & quaternionRotation,
                               bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(quaternionRotation);
        } else {
            this->FromRaw(quaternionRotation);
        }
    }


    /*!
      Constructor from 4 elements.
      \param x First component of the real part.
      \param y Second component of the real part.
      \param z Third component of the real part.
      \param r The imaginary part.
      \param normalizeInput Force the input normalization.
    */
    inline
    vctQuaternionRotation3Base(const value_type & x, const value_type & y, const value_type & z,
                               const value_type & r,
                               bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(x, y, z, r);
        } else {
            this->FromRaw(x, y, z, r);
        }
    }

    /*!
      Constructor from a rotation matrix.
      \param matrixRotation A rotation matrix.
      \param normalizeInput Force the input normalization.
    */
    template <class __containerType>
    inline explicit
    vctQuaternionRotation3Base(const vctMatrixRotation3Base<__containerType> & matrixRotation,
                               bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(matrixRotation);
        } else {
            this->FromRaw(matrixRotation);
        }
    }

    /*!
      Constructor from an axis and angle.
      \param axisAngleRotation An axis/angle rotation.
      \param normalizeInput Force the input normalization.
     */
    inline
    vctQuaternionRotation3Base(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                               bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(axisAngleRotation);
        } else {
            this->FromRaw(axisAngleRotation);
        }
    }


    /*!
      Constructor from a Rodriguez rotation.
      \param rodriguezRotation A Rodriguez rotation.
      \param normalizeInput Force the input normalization.
    */
    template <class __containerType>
    inline
    vctQuaternionRotation3Base(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation,
                               bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(rodriguezRotation);
        } else {
            this->FromRaw(rodriguezRotation);
        }
    }
    //@}


    /*! Const reference to the identity.  In this case, <tt>(0, 0, 0,
        1)</tt>*/
    static CISST_EXPORT const ThisType & Identity();


    /*!
      \name Conversion from normalized input.

      These methods will check that the input is normalized.  If the
      input is not normalized, an exception (\c std::runtime_error)
      will be thrown using cmnThrow().

      \note Since all exceptions are thrown using cmnThrow(), it is
      possible to configure these methods to use \c abort() if the
      normalization requirements are not met (see cmnThrow()).

    */
    //@{

    /*! Conversion from 4 numbers.  This method actually performs an
      assignement and then check that the result is normalized. */
    inline ThisType &
    From(value_type x, value_type y, value_type z, value_type r) CISST_THROW(std::runtime_error) {
        this->Assign(x, y, z, r);
        this->ThrowUnlessIsNormalized(*this);
        return *this;
    }

    /*! Conversion from axis/angle. */
    inline ThisType &
    From(const vctAxisAngleRotation3<value_type> axisAngleRotation) CISST_THROW(std::runtime_error) {
        this->ThrowUnlessIsNormalized(axisAngleRotation);
        return FromRaw(axisAngleRotation);
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    From(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) CISST_THROW(std::runtime_error) {
        this->ThrowUnlessIsNormalized(rodriguezRotation);
        return this->FromRaw(rodriguezRotation);
    }

    /*! Conversion from a rotation matrix. */
    template <class __containerType>
    inline ThisType &
    From(const vctMatrixRotation3Base<__containerType> & matrixRotation) CISST_THROW(std::runtime_error) {
        this->ThrowUnlessIsNormalized(matrixRotation);
        return FromRaw(matrixRotation);
    }
    //@}




    /*!
      \name Conversion and normalization.

      These method will accept any input and attempt to either
      normalize the input and then convert or convert and then
      normalize the quaternion itself.<br>The order depends on the
      type of input.
    */
    //@{

    /*! Conversion from another quaternion.  This method actually performs an
      assignement and then normalize the quaternion (\c this). */
    template <class __containerType>
    inline ThisType &
    FromNormalized(const vctQuaternionRotation3Base<__containerType> & other) {
        this->Assign(other);
        this->NormalizedSelf();
        return *this;
    }

    /*! Conversion from 4 numbers.  This method actually performs an
      assignement and then normalize the quaternion (\c this). */
    inline ThisType &
    FromNormalized(value_type x, value_type y, value_type z, value_type r) {
        this->Assign(x, y, z, r);
        this->NormalizedSelf();
        return *this;
    }

    /*! Conversion from axis/angle. */
    inline ThisType &
    FromNormalized(const vctAxisAngleRotation3<value_type> axisAngleRotation) {
        return FromRaw(axisAngleRotation.Normalized());
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    FromNormalized(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(rodriguezRotation.Normalized());
    }

    /*!
      Conversion from a rotation matrix.

      This method could normalize the input first, but the rotation
      matrix normalization relies on -1- a conversion to quaternion,
      -2- normalization and -3- a conversion back to rotation matrix.
      Therefore it converts to quaternion and then normalizes the
      resulting quaternion (\c this).
    */
    template <class __containerType>
    inline ThisType &
    FromNormalized(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        this->FromRaw(matrixRotation);
        this->NormalizedSelf();
        return *this;
    }
    //@}




    /*!
      \name Conversion.

      These method don't check if the input is normalized nor try to
      normalize the results.  They should be used with caution since
      the resulting rotation (in this case a quaternion) might not be
      normalized.
    */
    //@{

    template <class __containerType>
    inline ThisType &
    FromRaw(const vctQuaternionRotation3Base<__containerType> & other) {
        this->Assign(other);
        return *this;
    }

    inline ThisType &
    FromRaw(value_type x, value_type y, value_type z, value_type r) {
        this->Assign(x, y, z, r);
        return *this;
    }

    inline ThisType &
    FromRaw(const vctAxisAngleRotation3<value_type> axisAngleRotation) {
        typedef vctAxisAngleRotation3<value_type> AxisAngleType;
        typedef typename AxisAngleType::AxisType AxisType;

        const AngleType angle = axisAngleRotation.Angle();
        const AxisType axis = axisAngleRotation.Axis();

        const AngleType halfAngle = angle * 0.5;
        const value_type s = (value_type) sin(halfAngle);
        const value_type c = (value_type) cos(halfAngle);

        this->Assign(s * axis[0], s * axis[1], s * axis[2], c);
        return *this;
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    FromRaw(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(vctAxisAngleRotation3<value_type>(rodriguezRotation, VCT_DO_NOT_NORMALIZE));
    }

    /*! Conversion from a rotation matrix.  The algorithm is based on
      http://www.j3d.org/matrix_faq/matrfaq_latest.html.  This method
      is important since we use it to convert a rotation matrix to a
      quaternion to normalize the matrix.  The method From can not be
      used since it asserts that the input matrix is already
      normalized.

      \param matrixRotation A rotation matrix
    */
    template <class __containerType>
    inline ThisType & FromRaw(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        vctQuaternionRotation3BaseFromRaw(*this, matrixRotation);
        return *this;
    }
    //@}




    /*! A complementary form of assigning one quaternion rotation to
      another.  The method is provided mostly for generic programming
      interfaces, and for testing various operations on rotations */
    inline ThisType & From(const ThisType & otherRotation) {
        return reinterpret_cast<ThisType &>(this->Assign(otherRotation));
    }


    /*! Sets this rotation quaternion as the normalized version of another one.
      \param otherQuaternion rotation quaternion used to compute the normalized quaternion. */
    inline ThisType & NormalizedOf(const ThisType & otherQuaternion) {
        CMN_ASSERT(otherQuaternion.Pointer() != this->Pointer());
        this->Assign(otherQuaternion);
        this->NormalizedSelf();
        return *this;
    }


    /*! Normalizes this quaternion rotation. */
    inline ThisType & NormalizedSelf(void) {
        BaseType::NormalizedSelf();
        return *this;
    }


    /*! Returns the normalized version of this quaternion rotation.
      This method returns a copy of the normalized version and does
      not modify this quaternion. */
    inline ThisType
    Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }


    /*! Inverse this rotation quaternion.  This methods assumes that
      the quaternion is normalized and sets this unit quaternion as
      its transposed. */
    inline ThisType & InverseSelf(void) {
        this->ConjugateSelf();
        return *this;
    }


    /*! Set this rotation as the inverse of another one. */
    inline ThisType & InverseOf(const ThisType & otherRotation) {
        this->ConjugateOf(otherRotation);
        return *this;
    }


    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy. */
    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }


    /*! Apply the rotation to a vector of fixed size 3. The result is stored into
      a vector of size 3 provided by the caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        vctQuaternionVectorProductByElements(this->X(), this->Y(), this->Z(), this->R(),
                                             input.X(), input.Y(), input.Z(), output);
    }


    template <class __vectorOwnerType, stride_type __stride, class __dataPtrType>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        vctQuaternionVectorProductByElements(this->X(), this->Y(), this->Z(), this->R(),
                                             input.X(), input.Y(), input.Z(), output);
    }


    /*! Apply the rotation to a vector of fixed size 3. The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input vector
      \return The output vector
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const
    {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }

    template <class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const
    {
        CMN_ASSERT(input.size() == DIMENSION);
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply this rotation to a fixed size matrix with 3 rows.  The result is
      stored in another fixed matrix. */
    template <size_type __cols, stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline void ApplyTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                        vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        const size_type numCols = input.cols();
        index_type col;
        for (col = 0; col < numCols; ++col) {
            vctFixedSizeConstVectorRef<value_type, DIMENSION, __rowStride1> inputCol(input.Column(col));
            vctFixedSizeVectorRef<value_type, DIMENSION, __rowStride2> outputCol(output.Column(col));
            this->ApplyTo(inputCol, outputCol);
        }
    }

    /*! Apply this rotation to a dynamic matrix with 3 rows.  The result is
      stored in another dynamic matrix. */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
                        vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        const size_type numCols = input.cols();
        index_type col;
        for (col = 0; col < numCols; ++col) {
            vctDynamicConstVectorRef<value_type> inputCol(input.Column(col));
            vctDynamicVectorRef<value_type> outputCol(output.Column(col));
            this->ApplyTo(inputCol, outputCol);
        }
    }


    /*! Apply the rotation to another rotation.  The result is stored
      into a vctQuaternionRotation3Base (ThisType) provided by the caller and
      passed by reference.

      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }


    /*! Apply the rotation to another rotation.  The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType ApplyTo(const ThisType & input) const {
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to a dynamic vector.  The result is stored
      into another dynamic vector passed by reference by the caller.
      It is assumed that both are of size 3.
    */
    template <class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        vctFixedSizeVector<value_type, 3> tmpOutput;
        vctQuaternionVectorProductByElements(this->X(), this->Y(), this->Z(), this->R(),
            input.X(), input.Y(), input.Z(), tmpOutput);
        output.X() = tmpOutput.X();
        output.Y() = tmpOutput.Y();
        output.Z() = tmpOutput.Z();
    }


    /*! Apply the inverse of the rotation to a vector of fixed size
      3. The result is stored into a vector of size 3 provided by the
      caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        vctQuaternionVectorProductByElements(-this->X(), -this->Y(), -this->Z(), this->R(),
                                             input.X(), input.Y(), input.Z(), output);
    }


    template <class __vectorOwnerType, stride_type __stride, class __dataPtrType>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        vctQuaternionVectorProductByElements(-this->X(), -this->Y(), -this->Z(), this->R(),
                                             input.X(), input.Y(), input.Z(), output);
    }


    /*! Apply the the inverse of the rotation to a vector of fixed
      size 3. The result is returned by copy.  This interface might be
      more convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const
    {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    template <class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const
    {
        CMN_ASSERT(input.size() == DIMENSION);
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply this inverse rotation to a fixed size matrix with 3 rows.  The result is
      stored in another fixed matrix. */
    template <size_type __cols, stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline void ApplyInverseTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                               vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        const size_type numCols = input.cols();
        index_type col;
        for (col = 0; col < numCols; ++col) {
            vctFixedSizeConstVectorRef<value_type, DIMENSION, __rowStride1> inputCol(input.Column(col));
            vctFixedSizeVectorRef<value_type, DIMENSION, __rowStride2> outputCol(output.Column(col));
            this->ApplyInverseTo(inputCol, outputCol);
        }
    }

    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctQuaternionRotation3Base (ThisType) provided
      by the caller and passed by reference.

      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->Conjugate(), input);
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType ApplyInverseTo(const ThisType & input) const {
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the the inverse of the rotation to a dynamic vector.
      The result is stored into another dynamic vector passed by
      reference by the caller.  It is assumed that both are of size 3.
    */
    template <class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        vctFixedSizeVector<value_type, 3> tmpOutput;
        vctQuaternionVectorProductByElements(-this->X(), -this->Y(), -this->Z(), this->R(),
            input.X(), input.Y(), input.Z(), tmpOutput);
        output.X() = tmpOutput.X();
        output.Y() = tmpOutput.Y();
        output.Z() = tmpOutput.Z();
    }

    /*! Apply this rotation inverse to a dynamic matrix with 3 rows.  The result is
      stored in another dynamic matrix. */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyInverseTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
                        vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        const size_type numCols = input.cols();
        index_type col;
        for (col = 0; col < numCols; ++col) {
            vctDynamicConstVectorRef<value_type> inputCol(input.Column(col));
            vctDynamicVectorRef<value_type> outputCol(output.Column(col));
            this->ApplyInverseTo(inputCol, outputCol);
        }
    }


    /*! Return true if this quaternion rotation is equivalent to the
      other quaternion rotation, up to the given tolerance.
      Quaternion rotation may be effectively equivalent if one is
      elementwise equal to the other, or if one is the negation of the
      other.

      The tolerance factor is used to compare each of the elements of the
      difference vector.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        ThisType differenceVector;
        differenceVector.DifferenceOf(*this, other);
        differenceVector.AbsSelf();
        if (differenceVector.Lesser(tolerance)) {
            return true;
        }
        differenceVector.SumOf(*this, other);
        differenceVector.AbsSelf();
        if (differenceVector.Lesser(tolerance)) {
            return true;
        }
        return false;
    }


    /*! Multiply two rotation quaternions and return the result as a normalized rotation
      quaternion.
      \return (*this) * other
      \note this function overrides and shadows the operator * defined for basic
      quaternion type.  The latter returns a vctQuaternion, while this version
      returns a specialized vctQuaternionRotation3Base
    */
    ThisType operator * (const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        return this->ApplyTo(input);
    }



    /*!
      \name Deprecated methods
    */
    //@{

    /*! Convert from a unit vector and an angle. It is important to
      note that this method doesn't normalize the axis vector but
      asserts that it is normalized (in debug mode only).

      \param axis A unit vector of size 3.
      \param angle The angle in radian

      \note This method is deprecated.  Use
      From(vctAxisAngleRotation3) instead.

    */
    template <stride_type __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType &
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & axis,
         const AngleType & angle) {
        assert(vctUnaryOperations<value_type>::AbsValue::Operate(value_type(axis.Norm() - 1))
               < cmnTypeTraits<value_type>::Tolerance());
        const double halfAngle = angle * 0.5;
        const value_type s = value_type(sin(halfAngle));
        const value_type c = value_type(cos(halfAngle));
        this->ConcatenationOf(s * axis, c);
        return *this;
    }

    /*! Convert from ``Rodriguez rotation'' to rotation
      quaternion.  A Rodriguez rotation is a 3-element vector whose
      direction is the rotation axis, and magnitude is the rotation
      angle in radians.

      \param rodriguezRotation the Rodriguez rotation

      \note For reasons of numerical stability, if the magnitude of
      the Rodriguez vector is less than
      cmnTypeTraits<value_type>::Tolerance(), it is regarded as zero.

      \note This method is deprecated.  Use
      From(vctRodriguezRotation3Base) instead.
    */
    template <stride_type __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType &
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & rodriguezRotation) {
        const value_type axisLength = rodriguezRotation.Norm();
        const value_type axisTolerance = cmnTypeTraits<value_type>::Tolerance();
        const double angle = (fabs(axisLength) < axisTolerance) ? 0.0 : axisLength;
        const vctFixedSizeVector<value_type,3> defaultAxis(value_type(1), value_type(0), value_type(0));
        const vctFixedSizeVector<value_type,3> axis(  (angle == value_type(0)) ? defaultAxis : (rodriguezRotation / axisLength) );
        return this->From(axis, angle);
    }


    /*! Convert to an axis and angle representation.  It is important
      to note that this method doesn't check if the rotation
      quaternion is normalized or not.

      \param axis The axis of the rotation

      \param angle The angle around the axis to match the rotation

      \note This method is deprecated.  Use vctAxisAngleRotation3.From() instead.
    */
    template <stride_type __stride, class __dataPtrType>
    inline CISST_DEPRECATED
    void GetAxisAngle(vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & axis,
                      value_type & angle) {
        angle = value_type(acos(this->R()) * 2);
        value_type sinAngle = value_type(sqrt(1.0 - this->R() * this->R()));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(sinAngle) > cmnTypeTraits<value_type>::Tolerance()) {
            axis.X() = this->X() / sinAngle;
            axis.Y() = this->Y() / sinAngle;
            axis.Z() = this->Z() / sinAngle;
        } else {
            axis.X() = this->X();
            axis.Y() = this->Y();
            axis.Z() = this->Z();
        }
    }
    //@}
};




template <class _containerType, class _elementType, vct::stride_type _stride, class _dataPtrType>
inline vctFixedSizeVector<_elementType, 3>
operator * (const vctQuaternionRotation3Base<_containerType> & rotationQuaternion,
            const vctFixedSizeConstVectorBase<3, _stride, _elementType, _dataPtrType> & vector) {
    vctFixedSizeVector<_elementType, 3> result;
    rotationQuaternion.ApplyTo(vector, result);
    return result;
}

template <class _containerType, class _elementType, class _vectorOwnerType>
inline vctFixedSizeVector<_elementType, 3>
operator * (const vctQuaternionRotation3Base<_containerType> & rotationQuaternion,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    vctFixedSizeVector<_elementType, 3> result;
    rotationQuaternion.ApplyTo(vector, result);
    return result;
}



/*!  This is an auxiliary function to multiply q * v * q^ -- where q is
  a quaternion, v is a vector (a pure imaginary quaternion), and q^ is
  the conjugate of q.  This is the basic operation in applying a quaternion
  rotation.
*/
template <class _elementType, vct::stride_type __strideOut, class __dataPtrTypeOut>
inline void vctQuaternionVectorProductByElements(
     const _elementType qX, const _elementType qY, const _elementType qZ, const _elementType qR,
     const _elementType vX, const _elementType vY, const _elementType vZ,
     vctFixedSizeVectorBase<3, __strideOut, _elementType, __dataPtrTypeOut> & output)
{
    typedef _elementType value_type;

    // compute the product of quaternions q_input = [in, 0] and q
    // conjugate.  Since q_input[3] = 0, suppress terms with it.
    const value_type tR = - vX * qX - vY * qY - vZ * qZ;
    const value_type tX = - vX * qR + vY * qZ - vZ * qY;
    const value_type tY = - vY * qR + vZ * qX - vX * qZ;
    const value_type tZ = - vZ * qR + vX * qY - vY * qX;

    // multiply q by (qX, qY, qY, qR).  For out quaternion,
    // element 4 (qR) is not interesting since we only want the
    // vector.
    output.X() = - qR * tX - qX * tR - qY * tZ + qZ * tY;
    output.Y() = - qR * tY - qY * tR - qZ * tX + qX * tZ;
    output.Z() = - qR * tZ - qZ * tR - qX * tY + qY * tX;
}



template <class _quaternionType, class _matrixType>
void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<_quaternionType> & quaternionRotation,
                                  const vctMatrixRotation3Base<_matrixType> & matrixRotation)
{
    typedef typename _quaternionType::value_type value_type;
    typedef typename _quaternionType::NormType NormType;

    const NormType a0 = 1.0 + matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2);
    const NormType a1 = 1.0 + matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1) - matrixRotation.Element(2, 2);
    const NormType a2 = 1.0 - matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) - matrixRotation.Element(2, 2);
    const NormType a3 = 1.0 - matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2);

    NormType max = a0;
    unsigned char index = 0;
    if (a1 > max) {
        max = a1;
        index = 1;
    }
    if (a2 > max) {
        max = a2;
        index = 2;
    }
    if (a3 > max) {
        index = 3;
    }

    NormType ratio;
    switch (index) {
    case 0:
        quaternionRotation.R() = static_cast<value_type>(sqrt(a0) * 0.5);
        ratio = 0.25 / quaternionRotation.R();
        quaternionRotation.X() = static_cast<value_type>((matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2)) * ratio);
        quaternionRotation.Y() = static_cast<value_type>((matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0)) * ratio);
        quaternionRotation.Z() = static_cast<value_type>((matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1)) * ratio);
        break;
    case 1:
        quaternionRotation.X() = static_cast<value_type>(sqrt(a1) * 0.5);
        ratio = 0.25 / quaternionRotation.X();
        quaternionRotation.R() = static_cast<value_type>((matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2)) * ratio);
        quaternionRotation.Y() = static_cast<value_type>((matrixRotation.Element(1, 0) + matrixRotation.Element(0, 1)) * ratio);
        quaternionRotation.Z() = static_cast<value_type>((matrixRotation.Element(2, 0) + matrixRotation.Element(0, 2)) * ratio);
        break;
    case 2:
        quaternionRotation.Y() = static_cast<value_type>(sqrt(a2) * 0.5);
        ratio = 0.25 / quaternionRotation.Y();
        quaternionRotation.R() = static_cast<value_type>((matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0)) * ratio);
        quaternionRotation.X() = static_cast<value_type>((matrixRotation.Element(1, 0) + matrixRotation.Element(0, 1)) * ratio);
        quaternionRotation.Z() = static_cast<value_type>((matrixRotation.Element(2, 1) + matrixRotation.Element(1, 2)) * ratio);
        break;
    case 3:
        quaternionRotation.Z() = static_cast<value_type>(sqrt(a3) * 0.5);
        ratio = 0.25 / quaternionRotation.Z();
        quaternionRotation.R() = static_cast<value_type>((matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1)) * ratio);
        quaternionRotation.X() = static_cast<value_type>((matrixRotation.Element(0, 2) + matrixRotation.Element(2, 0)) * ratio);
        quaternionRotation.Y() = static_cast<value_type>((matrixRotation.Element(2, 1) + matrixRotation.Element(1, 2)) * ratio);
        break;
    default:
        break;
    }
}

#endif  // _vctQuaternionRotation3Base_h
