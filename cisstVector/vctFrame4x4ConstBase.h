/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2007-09-13

  (C) Copyright 2007-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFrame4x4ConstBase_h
#define _vctFrame4x4ConstBase_h

/*!
  \file
  \brief Declaration of vctFrame4x4Base
 */

#include <cisstVector/vctFixedSizeVectorRef.h>
#include <cisstVector/vctMatrixRotation3Ref.h>
#include <cisstVector/vctMatrixRotation3ConstRef.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Template base class for a 4x4 frame.

  This class allows to use a 4 by 4 matrix as a frame in 3D.  This is
  a limited case of the so called homegenous transformations as this
  class is intended to support only the translation and rotation parts
  of the transformation.  It is not intended to support perspective or
  scaling operations.  The different constructors and normalization
  methods provided will set the last row to [0 0 0 1].

  \param _matrixType The type of matrix used to store the elements

  \sa vctDynamicMatrix, vctFixedSizeMatrix, vctFrameBase
*/
template <class _containerType>
class vctFrame4x4ConstBase: public _containerType
{
public:
    enum {ROWS = 4, COLS = 4};
    enum {DIMENSION = 3};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctFrame4x4ConstBase<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    enum {ROWSTRIDE = ContainerType::ROWSTRIDE, COLSTRIDE = ContainerType::COLSTRIDE};

    typedef typename BaseType::RowValueType RowValueType;
    typedef typename BaseType::ColumnValueType ColumnValueType;
    typedef typename BaseType::RowRefType RowRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;
    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;
    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::RefTransposeType RefTransposeType;
    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef vctFrame4x4<value_type, COLSTRIDE <= ROWSTRIDE> FrameValueType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /* Types to maintain internal references to subparts of the 4x4 matrix. */
    typedef vctFixedSizeVectorRef<value_type, DIMENSION, ROWSTRIDE> TranslationRefType;
    typedef vctFixedSizeConstVectorRef<value_type, DIMENSION, ROWSTRIDE> ConstTranslationRefType;
    typedef vctFixedSizeVectorRef<value_type, DIMENSION, COLSTRIDE> PerspectiveRefType;
    typedef vctFixedSizeConstVectorRef<value_type, DIMENSION, COLSTRIDE> ConstPerspectiveRefType;
    typedef vctMatrixRotation3Ref<value_type, ROWSTRIDE, COLSTRIDE> RotationRefType;
    typedef vctMatrixRotation3ConstRef<value_type, ROWSTRIDE, COLSTRIDE> ConstRotationRefType;

protected:
    /* Internal references to rotation matrix, translation vector and
       first 3 element of last row. */
    RotationRefType RotationRef;
    TranslationRefType TranslationRef;
    PerspectiveRefType PerspectiveRef;

    /*! Update internal references to rotation matrix, translation
        vector and first 3 elements of last row.  This method is
        called by allocate, so make sure that any specialization of
        Allocate includes a call to UpdateReferences. */
    inline void UpdateReferences(void) {
        // Rotation matrix is 3 by 3 starting at 0, 0
        RotationRef.SetRef(this->Pointer(0, 0));
        // Translation is a vector 3, column starting at 0, 3
        TranslationRef.SetRef(this->Pointer(0, DIMENSION));
        // Perspective is a vector 3, row starting a 3, 0
        PerspectiveRef.SetRef(this->Pointer(DIMENSION, 0));
    }

public:


    /*! Default constructor. Updates internal references. */
    vctFrame4x4ConstBase(void) {
        this->UpdateReferences();
    }

    /*! Const reference to the identity.  In this case, the
        translation is set to <tt>(0, 0, 0)</tt> and the rotation is
        set to identity using its own method
        <tt>RotationType::Identity()</tt>. */
    static CISST_EXPORT const FrameValueType & Identity(void);

    inline ConstTranslationRefType Translation(void) const {
        return this->TranslationRef;
    }

    inline ConstRotationRefType Rotation(void) const {
        return this->RotationRef;
    }

    inline ConstPerspectiveRefType Perspective(void) const {
        return this->PerspectiveRef;
    }


    FrameValueType Inverse(void) const;  // implemented in vctFrame4x4.h


    /*! Test if the rotation part is normalized and the last row is
      almost equal to [0 0 0 1].  See
      vctMatrixRotation3ConstBase::IsNormalized and
      vctFixedSizeConstVectorBase::AlmostEqual.

      \param tolerance Tolerance for the norm and scalar product tests.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const {
        return (this->Row(DIMENSION).AlmostEqual(RowValueType(static_cast<value_type>(0.0),
                                                              static_cast<value_type>(0.0),
                                                              static_cast<value_type>(0.0),
                                                              static_cast<value_type>(1.0)),
                                                 tolerance)
                && this->RotationRef.IsNormalized(tolerance));
    }

    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is stored into a vector of size DIMENSION provided by the caller
      and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        this->RotationRef.ApplyTo(input, output);
        output.X() += this->TranslationRef.X();
        output.Y() += this->TranslationRef.Y();
        output.Z() += this->TranslationRef.Z();
    }


    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

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


    /*! Compose this transform over the input transform to obtain a
      new output transform passed by reference by the caller.  The
      semantics of the operation are:

      output = (*this) * input

      if (*this) is [R1 | p1], input is [R2 | p2], then output will be
      [R1*R2 | R1*p2 + p1]
    */
    template <class __containerType1, class __containerType2>
    inline void ApplyTo(const vctFrame4x4ConstBase<__containerType1> & input,
                        vctFrame4x4Base<__containerType2> & output) const {
        typename TranslationRefType::CopyType temp;
        RotationRef.ApplyTo(input.Translation(), temp);
        (output.Rotation()).ProductOf(RotationRef, input.Rotation());
        (output.Translation()).SumOf(temp, TranslationRef);
    }


    /*! Apply the transformation to another transformation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input transformation
      \return The output transformation
    */
    template <class __containerType>
    inline FrameValueType ApplyTo(const vctFrame4x4ConstBase<__containerType> & input) const {
        FrameValueType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the transformation to a dynamic vector.  The result is
      stored into another dynamic vector.  It is assumed that both are
      of size DIMENSION.
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const
    {
        this->RotationRef.ApplyTo(input, output);
        output[0] += this->TranslationRef[0];
        output[1] += this->TranslationRef[1];
        output[2] += this->TranslationRef[2];
    }


    /*! Apply this transform to a matrix of three rows */
    template <size_type __cols, stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
            vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        typedef vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> inputType;
        typedef typename inputType::ConstColumnRefType inputColumnType;
        typedef vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> outputType;
        typedef typename outputType::ColumnRefType outputColumnType;
        index_type columnIndex;
        for (columnIndex = 0; columnIndex < __cols; ++columnIndex) {
            const inputColumnType inputColumn(input.Column(columnIndex));
            outputColumnType outputColumn(output.Column(columnIndex));
            this->ApplyTo(inputColumn, outputColumn);
        }
    }


    /*! Apply this transform to an array of DIMENSION-vectors given as a
      pointer and a size.  This method can be generalized to be
      templated with different vector types, defined by strides and
      dataPtrType.  For simplicity, this version is writtend for a
      DIMENSION-vector object.
    */
    inline void
    ApplyTo(size_type inputSize, const vctFixedSizeVector<value_type, DIMENSION> * input,
            vctFixedSizeVector<value_type, DIMENSION> * output) const
    {
        index_type index;
        for (index = 0; index < inputSize; ++index) {
            this->ApplyTo(input[index], output[index]);
        }
    }


    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        vctFixedSizeVector<value_type, DIMENSION> temp;
        temp.DifferenceOf(input, this->TranslationRef);
        this->RotationRef.ApplyInverseTo(temp, output);
    }

    /*! Apply the inverse of the transformation to a vector of fixed
      size DIMENSION. The result is stored into a vector of size DIMENSION provided by
      the caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const {
        const vctFixedSizeVector<value_type, DIMENSION> temp(input[0] - this->TranslationRef[0],
                                                             input[1] - this->TranslationRef[1],
                                                             input[2] - this->TranslationRef[2]);
        typename TranslationRefType::CopyType result;
        this->RotationRef.ApplyInverseTo(temp, result);
        output[0] = result[0];
        output[1] = result[1];
        output[2] = result[2];
    }


    /*! Apply the the inverse of the transformation to a vector of
      fixed size DIMENSION. The result is returned by copy.  This interface
      might be more convenient for some but one should note that it is
      less efficient since it requires a copy.

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


    template <class __containerType1, class __containerType2>
    inline void ApplyInverseTo(const vctFrame4x4ConstBase<__containerType1> & input,
                               vctFrame4x4Base<__containerType2> & output) const {
        FrameValueType inverse;
        inverse.InverseOf(*this);
        inverse.ApplyTo(input, output);
    }

    template <class __containerType>
    inline FrameValueType ApplyInverseTo(const vctFrame4x4ConstBase<__containerType> & input) const {
        FrameValueType result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    /*! Return true if this transformation is exactly equal to the
      other transformation.  The result is based on the Equal()
      methods provided by the different rotation representations
      (vctQuaternionRotation3, vctMatrixRotation3, ...) and the
      translation.

      \sa AlmostEqual
    */
    //@{
    inline bool Equal(const ThisType & other) const {
        return (this->RotationRef.Equal(other.Rotation())
                && this->TranslationRef.Equal(other.Translation()));
    }

    inline bool operator == (const ThisType & other) const {
        return this->Equal(other);
    }
    //@}


    /*! Return true if this transformation is equal to the other
      transformation, up to the given tolerance.  The result is based
      on the AllowsEqual() methods provided by the different rotation
      representations (vctQuaternionRotation3, vctMatrixRotation3,
      ...) and the translation.

      The tolerance factor is used to compare both the translations
      and rotations.

      \sa AlmostEquivalent
    */
    inline bool AlmostEqual(const ThisType & other,
                            value_type tolerance = TypeTraits::Tolerance()) const {
        return (this->RotationRef.AlmostEqual(other.Rotation(), tolerance)
                && this->TranslationRef.AlmostEqual(other.Translation(), tolerance));
    }


    /*! Return true if this frame is effectively equivalent to the
      other frame, up to the given tolerance.  For an homogeneous
      frame, this method uses AlmostEqual.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        return this->AlmostEqual(other, tolerance);
    }

};


#endif  // _vctFrame4x4Base_h

