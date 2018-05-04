/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2005-08-19

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation3ConstBase_h
#define _vctMatrixRotation3ConstBase_h

/*!
  \file
  \brief Declaration of vctMatrixRotation3ConstBase
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctForwardDeclarations.h>

#include <cisstVector/vctExport.h>

/*!
  \brief Define a rotation matrix for a space of dimension 3

  This class is templated by the type of container used to store the
  rotation matrix.  This class is an internal class, i.e. it is not
  intended for the end-user.  The class which should be used by most
  is vctMatRot3 (eventually vctMatrixRotation3<_elementType>).

  The main goal of this meta rotation matrix class is to ease the
  interface with Python.  For a native Python object, a matrix
  rotation can be defined based on a vctDynamicMatrix which is much
  easier to wrap than a vctFixedSizeMatrix.  For a C++ object accessed
  from Python, the rotation matrix will be defined using
  vctMatrixRotation3Base<vctDynamicMatrixRef<double> >, referring to
  the C++ vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >.

  \param _containerType The type of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _containerType>
class vctMatrixRotation3ConstBase: public _containerType
{
public:
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctMatrixRotation3ConstBase<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    /*! Type used to return by copy */
    typedef vctMatrixRotation3<value_type> RotationValueType;

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const CISST_THROW(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation3ConstBase: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const CISST_THROW(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation3ConstBase: Input is not normalized"));
        }
    }

public:

    /*! Const reference to the identity.  In this case, the identity matrix:
      <pre>
      1 0 0
      0 1 0
      0 0 1
      </pre>
    */
    static CISST_EXPORT const RotationValueType & Identity();

    /*! Returns the normalized version of this rotation.  This method
      returns a copy of the normalized rotation and does not modify
      this rotation.  See also NormalizedSelf(). */
    RotationValueType Normalized(void) const; // implemented in vctMatrixRotation3.h

    /*! Test if this matrix is normalized.  This methods checks that
      all the columns are normalized (within a margin of tolerance)
      and then checks that the three vectors are orthogonal to each
      other.

      \param tolerance Tolerance for the norm and scalar product tests.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const {
        NormType columnNorm;
        columnNorm = this->Column(0).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        columnNorm = this->Column(1).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        columnNorm = this->Column(2).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        value_type columnDot;
        columnDot = vctDotProduct(this->Column(0), this->Column(1));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        columnDot = vctDotProduct(this->Column(0), this->Column(2));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        columnDot = vctDotProduct(this->Column(1), this->Column(2));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        return true;
    }

    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy.  See
      also InverseSelf().  */
    RotationValueType Inverse(void) const; // implemented in vctMatrixRotation3.h


    /*! Apply the rotation to a vector of fixed size 3. The result is
      stored into a vector of size 3 provided by the caller and passed
      by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        // Implementation note: We think working on a local copy of the input
        // is faster than working on the members, because of fewer indirections,
        // especially when combined with the dot product operation.
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( this->Row(0), inputCopy );
        output[1] = vctDotProduct( this->Row(1), inputCopy );
        output[2] = vctDotProduct( this->Row(2), inputCopy );
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
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to another rotation.  The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType
    ApplyTo(const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to a dynamic vector.  The result is stored
      into another dynamic vector passed by reference by the caller.
      It is assumed that both are of size 3.
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const
        CISST_THROW(std::runtime_error)
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        // see implementation note for fixed size i/o version
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( this->Row(0), inputCopy );
        output[1] = vctDotProduct( this->Row(1), inputCopy );
        output[2] = vctDotProduct( this->Row(2), inputCopy );
    }


    template <class __vectorOwnerType, stride_type __stride, class __dataPtrType>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & output) const
        CISST_THROW(std::runtime_error)
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        // see implementation note for fixed size i/o version
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( this->Row(0), inputCopy );
        output[1] = vctDotProduct( this->Row(1), inputCopy );
        output[2] = vctDotProduct( this->Row(2), inputCopy );
    }


    /*! Apply the rotation to a dynamic vector of length 3.  The result is
      returned by value.
    */
    template <class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }

    /*! Apply the inverse of the rotation to a vector of fixed size
      3. The result is stored into a vector of size 3 provided by the
      caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        // see implementation note for fixed size i/o ApplyTo() version
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( inputCopy, this->Column(0) );
        output[1] = vctDotProduct( inputCopy, this->Column(1) );
        output[2] = vctDotProduct( inputCopy, this->Column(2) );
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
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    /*! Apply the inverse rotation to a dynamic vector of length 3.
      The result is returned by value.
    */
    template <class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }



    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctMatrixRotation3Base (ThisType) provided
      by the caller and passed by reference.

      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType ApplyInverseTo(const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the the inverse of the rotation to a dynamic vector.
      The result is stored into another dynamic vector passed by
      reference by the caller.  It is assumed that both are of size 3.
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        // see implementation note for fixed size i/o ApplyTo() version
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( inputCopy, this->Column(0) );
        output[1] = vctDotProduct( inputCopy, this->Column(1) );
        output[2] = vctDotProduct( inputCopy, this->Column(2) );
    }

    /*! Apply the the inverse of the rotation to a dynamic vector.
      The result is stored into a fixed-size vector passed by
      reference by the caller.  It is assumed that both are of size 3.
    */
    template <class __vectorOwnerType, stride_type __stride, class __dataPtrType>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        // see implementation note for fixed size i/o ApplyTo() version
        const vctFixedSizeVector<value_type, 3> inputCopy( input[0], input[1], input[2] );
        output[0] = vctDotProduct( inputCopy, this->Column(0) );
        output[1] = vctDotProduct( inputCopy, this->Column(1) );
        output[2] = vctDotProduct( inputCopy, this->Column(2) );
    }

    /*! Apply this rotation to a fixed-size matrix with 3 rows.  The result is
      stored in another fixed-size matrix */
    template <size_type __cols,
              stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline void ApplyTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                        vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }

    /*! Apply this rotation to a dynamic matrix with 3 rows.  The result is
      stored in another dynamic matrix. */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
                        vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        vctDynamicConstMatrixRef<value_type> myRef(*this);
        output.ProductOf(myRef, input);
    }

    /*! Apply the inverse rotation to a fixed-size matrix with 3 rows.  The result is
      stored in another fixed-size matrix */
    template <size_type __cols,
              stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline void ApplyInverseTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                               vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }

    /*! Apply this rotation inverse to a dynamic matrix with 3 rows.  The result is
      stored in another dynamic matrix. */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyInverseTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
                               vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        vctDynamicConstMatrixRef<value_type> myInvRef(this->TransposeRef());
        output.ProductOf(myInvRef, input);
    }

    /*! Multiply two rotation matrices and return the result as a rotation matrix.
      \return (*this) * other
      \note this function overrides and shadows the operator * defined for basic
      matrix type.  The latter returns a vctFixedSizeMatrix, while this version
      returns a specialized rotation matrix.
    */
    ThisType operator * (const ThisType & input) const
    {
        return this->ApplyTo(input);
    }

    /*! Override the * operator defined in the matrix base for matrix * vector.
      This prevents compiler confusion between the overloaded RotMat*RotMat and
      the base-class Mat*Vec
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    operator * (const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const
    {
        return this->ApplyTo(input);
    }

    /*! Implement operator * for MatrixRotatin * DynamicVector.  Return type
      is set to fixed-size vector of length 3.  It is the caller's responsibility
      to convert the return value to a dynamic vector, if so desired.
    */
    template <class __vectorOwnerType1>
    inline vctFixedSizeVector<value_type, DIMENSION>
    operator * (const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input) const
    {
        return this->ApplyTo(input);
    }



    /*! Return true if this rotation is effectively equivalent to the
      other rotation, up to the given tolerance.  For a rotation
      matrix, this method is strictly the same as AlmostEqual.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        return this->AlmostEqual(other, tolerance);
    }
};


#endif  // _vctMatrixRotation3ConstBase_h
