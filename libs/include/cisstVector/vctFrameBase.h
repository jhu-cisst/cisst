/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2004-02-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Declaration of vctFrameBase
 */


#ifndef _vctFrameBase_h
#define _vctFrameBase_h

#include <cisstVector/vctFixedSizeMatrixBase.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Template base class for a frame.

  The template parameter allows to change the orientation
  representation as well as the dimension.  The orientation (or
  rotation) type must define an enum DIMENSION which is used to
  determine the size of the vector representing the translation (see
  vctFixedSizeVector).

  This templating allows to create a frame based on a quaternion
  (dimension 3) or a matrix (dimension 2 or 3).

  \param _rotationType The type of rotation

  \sa vctQuaternionRotation3 vctMatrixRotation3 vctFixedSizeVector
*/
template<class _rotationType>
class vctFrameBase
{
public:
    enum {DIMENSION = _rotationType::DIMENSION};
    typedef typename _rotationType::value_type value_type;
    typedef vctFrameBase<_rotationType> ThisType;
    typedef _rotationType RotationType;
    typedef vctFixedSizeVector<value_type, DIMENSION> TranslationType;
    typedef cmnTypeTraits<value_type> TypeTraits;
    
protected:
    RotationType RotationMember;
    TranslationType TranslationMember;
    
public:


    /*! Default constructor. Sets the rotation matrix to identity. */
    vctFrameBase() {
        Assign(Identity());
    }
    
    /*! Constructor from a translation and a rotation. */
    template<int __stride, class __dataPtrType>
    vctFrameBase(const RotationType & rotation,
                 const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & translation):
        RotationMember(rotation),
        TranslationMember(translation)
    {}
    
    /*! Const reference to the identity.  In this case, the
        translation is set to <tt>(0, 0, 0)</tt> and the rotation is
        set to identity using its own method
        <tt>RotationType::Identity()</tt>. */
    static CISST_EXPORT const ThisType & Identity();
    
    
    inline ThisType & Assign(const ThisType & otherFrame) {
        RotationMember.Assign(otherFrame.Rotation());
        TranslationMember.Assign(otherFrame.Translation());
        return *this;
    }

    template<int __stride, class __dataPtrType>
	inline ThisType & Assign(const RotationType & rotation,
                             const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & translation) {
        RotationMember.Assign(rotation);
        TranslationMember.Assign(translation);
        return *this;
    }

    inline const TranslationType & Translation(void) const {
        return TranslationMember;
    }

    inline TranslationType & Translation(void) {
        return TranslationMember;
    }

    inline const RotationType & Rotation(void) const {
        return RotationMember;
    }

    inline RotationType & Rotation(void) {
        return RotationMember;
    }


    /*! Inverse this frame. */
    inline ThisType & InverseSelf(void) {
        // R -> Rinv
        RotationMember.InverseSelf();
        // T -> Rinv * (-T)
        TranslationType temp = -TranslationMember;
        RotationMember.ApplyTo(temp, TranslationMember);
        return *this;
    }

    
    inline ThisType & InverseOf(const ThisType & otherFrame) {
        TranslationMember = otherFrame.Translation();
        RotationMember = otherFrame.Rotation();
        InverseSelf();
        return *this;
    }

    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }


    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is stored into a vector of size DIMENSION provided by the caller
      and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        RotationMember.ApplyTo(input, output);
        output.Add(TranslationMember);
    }


    /*! Apply the transformation to a vector of fixed size DIMENSION. The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        TranslationType result;
        this->ApplyTo(input, result);
        return result;
    }

    /*! Apply the transformation to a dynamic vector of size DIMENSION. The
      result is returned by value.
      \param input The input vector
      \return The output vector
    */
    template<class _vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & input) const
    {
        TranslationType result;
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
    inline void ApplyTo(const ThisType & input, ThisType & output) const {
        TranslationType temp;
        RotationMember.ApplyTo(input.Translation(), temp);
        (output.Rotation()).ProductOf(RotationMember, input.Rotation());
        (output.Translation()).SumOf(temp, TranslationMember);
    }


    /*! Set this frame as the compose two transformations.  This
      method is provided to be consistent with all other containers
      which provide the method "ProductOf" and the operator "*".
    */
    inline void ProductOf(const ThisType & left, ThisType & right) {
        left.ApplyTo(right, *this);
    }


    /*! Apply the transformation to another transformation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input transformation
      \return The output transformation
    */
    inline ThisType ApplyTo(const ThisType & input) const {
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the transformation to a dynamic vector.  The result is
      stored into another dynamic vector.  It is assumed that both are
      of size DIMENSION.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        // Implementation note: we think that computing output first to a local variable,
        // then copying the result to the dynamic vector output, is more efficient than
        // computing directly to the output vector, because there is less pointer
        // arithmetic when doing the local variable.
        // Hopefully, the conditions below can be optimized out since their value is
        // known in compile time.
        TranslationType result;
        this->ApplyTo(input, result);
        if (DIMENSION > 0)
            output[0] = result[0];
        if (DIMENSION > 1)
            output[1] = result[1];
        if (DIMENSION > 2)
            output[2] = result[2];
    }

    /*! Apply the transformation to a dynamic vector.  The result is
      stored into a fixed size vector.  It is assumed that both are
      of size DIMENSION.
    */
    template<class _vectorOwnerType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>  & output) const
    {
        TranslationType result;
        // See implementation notes for rotation class
        RotationMember.ApplyTo(input, result);
        output.SumOf(result, TranslationMember);
    }


    /*! Apply this transform to a matrix of three rows */
    template<unsigned int __cols, int __rowStride1, int __colStride1, class __dataPtrType1, 
             int __rowStride2, int __colStride2, class __dataPtrType2>
    inline void ApplyTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                        vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        RotationMember.ApplyTo(input, output);
        if (DIMENSION > 0)
            output.Row(0).Add(TranslationMember[0]);
        if (DIMENSION > 1)
            output.Row(1).Add(TranslationMember[1]);
        if (DIMENSION > 2)
            output.Row(2).Add(TranslationMember[2]);
    }


    /*! Apply this transform to an array of DIMENSION-vectors given as a
      pointer and a size.  This method can be generalized to be
      templated with different vector types, defined by strides and
      dataPtrType.  For simplicity, this version is writtend for a
      DIMENSION-vector object.
    */
    inline void ApplyTo(unsigned int inputSize, const vctFixedSizeVector<value_type, DIMENSION> *input,
                        vctFixedSizeVector<value_type, DIMENSION> * output) const
    {
        unsigned int index;
        for (index = 0; index < inputSize; ++index) {
            this->ApplyTo(input[index], output[index]);
        }
    }

    /*! Apply the transofrmation to a dynamic matrix of DIMENSION rows.  Store the result
      to a second dynamic matrix.
    */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
                        vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        RotationMember.ApplyTo(input, output);
        if (DIMENSION > 0)
            output.Row(0).Add(TranslationMember[0]);
        if (DIMENSION > 1)
            output.Row(1).Add(TranslationMember[1]);
        if (DIMENSION > 2)
            output.Row(2).Add(TranslationMember[2]);
    }


    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        TranslationType temp;
        temp.DifferenceOf(input, TranslationMember);
        RotationMember.ApplyInverseTo(temp, output);
    }

    /*! Apply the inverse of the transformation to a vector of fixed
      size DIMENSION. The result is stored into a vector of size DIMENSION provided by
      the caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        TranslationType temp, result;
        if (DIMENSION > 0)
            temp[0] = input[0] - TranslationMember[0];
        if (DIMENSION > 1)
            temp[1] = input[1] - TranslationMember[1];
        if (DIMENSION > 2)
            temp[2] = input[2] - TranslationMember[2];
        RotationMember.ApplyInverseTo(temp, result);
        if (DIMENSION > 0)
            output[0] = result[0];
        if (DIMENSION > 1)
            output[1] = result[1];
        if (DIMENSION > 2)
            output[2] = result[2];
    }


    /*! Apply the inverse transformation to a dynamic vector.  The result is
      stored into a fixed size vector.  It is assumed that both are
      of size DIMENSION.
    */
    template<class _vectorOwnerType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>  & output) const
    {
        TranslationType temp;
        if (DIMENSION > 0)
            temp[0] = input[0] - TranslationMember[0];
        if (DIMENSION > 1)
            temp[1] = input[1] - TranslationMember[1];
        if (DIMENSION > 2)
            temp[2] = input[2] - TranslationMember[2];
        RotationMember.ApplyInverseTo(temp, output);
    }

    /*! Apply the the inverse of the transformation to a vector of
      fixed size DIMENSION. The result is returned by copy.  This interface
      might be more convenient for some but one should note that it is
      less efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the inverse transformation to a dynamic vector of size DIMENSION. The
      result is returned by value.
      \param input The input vector
      \return The output vector
    */
    template<class _vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & input) const
    {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {  
        ThisType inverse;
        inverse.InverseOf(*this);
        inverse.ApplyTo(input, output);
    }


    inline ThisType ApplyInverseTo(const ThisType & input) const {
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    /*! Apply the inverse transform to a fixed-size matrix of three rows */
    template<unsigned int __cols, int __rowStride1, int __colStride1, class __dataPtrType1, 
             int __rowStride2, int __colStride2, class __dataPtrType2>
    inline void ApplyInverseTo(const vctFixedSizeConstMatrixBase<DIMENSION, __cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & input,
                        vctFixedSizeMatrixBase<DIMENSION, __cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & output) const
    {
        const TranslationType invTranslation = RotationMember.ApplyInverseTo(-TranslationMember);
        RotationMember.ApplyInverseTo(input, output);
        if (DIMENSION > 0)
            output.Row(0).Add(invTranslation[0]);
        if (DIMENSION > 1)
            output.Row(1).Add(invTranslation[1]);
        if (DIMENSION > 2)
            output.Row(2).Add(invTranslation[2]);
    }

    /*! Apply the inverse transofrmation to a dynamic matrix of DIMENSION rows.  Store the result
      to a second dynamic matrix.
    */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline void ApplyInverseTo(const vctDynamicConstMatrixBase<__matrixOwnerType1, value_type> & input,
    vctDynamicMatrixBase<__matrixOwnerType2, value_type> & output) const
    {
        CMN_ASSERT((input.rows() == DIMENSION) && (output.rows() == DIMENSION) && (input.cols() == output.cols()));
        CMN_ASSERT(input.Pointer() != output.Pointer());
        const TranslationType invTranslation = RotationMember.ApplyInverseTo(-TranslationMember);
        RotationMember.ApplyInverseTo(input, output);
        if (DIMENSION > 0)
            output.Row(0).Add(invTranslation[0]);
        if (DIMENSION > 1)
            output.Row(1).Add(invTranslation[1]);
        if (DIMENSION > 2)
            output.Row(2).Add(invTranslation[2]);
    }


    /*! Implement operator * between frame and fixed or dynamic vector of length
        DIMENSION.  The return value is always a fixed-size vector.
    */
    //@{
    template <int _stride, class _dataPtrType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    operator * (const vctFixedSizeConstVectorBase<DIMENSION, _stride, value_type, _dataPtrType> & vector) const
    {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(vector, result);
        return result;
    }


    template<class _vectorOwnerType>
    inline vctFixedSizeVector<value_type, DIMENSION>
    operator * (const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & input) const
    {
        vctFixedSizeVector<value_type, DIMENSION> result;
        this->ApplyTo(input, result);
        return result;
    }
    //@}

    /*! Return true if this transformation is exactly equal to the
      other transformation.  The result is based on the Equal()
      methods provided by the different rotation representations
      (vctQuaternionRotation3, vctMatrixRotation3, ...) and the
      translation.
      
      \sa AlmostEqual
    */
    //@{
    inline bool Equal(const ThisType & other) const {
        return (RotationMember.Equal(other.Rotation())
                && TranslationMember.Equal(other.Translation()));
    }

    inline bool operator==(const ThisType & other) const {
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
        return (RotationMember.AlmostEqual(other.Rotation(), tolerance)
                && TranslationMember.AlmostEqual(other.Translation(), tolerance));
    }

    
    /*! Return true if this transformation is equivalent to the other
      transformation, up to the given tolerance.  The result is based
      on the AlmostEquivalent() methods provided by the different
      rotation representations (vctQuaternionRotation3,
      vctMatrixRotation3, ...) and AlmostEqual for the translation.

      The tolerance factor is used to compare both the translations
      and rotations.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        return (RotationMember.AlmostEquivalent(other.Rotation(), tolerance)
                && TranslationMember.AlmostEqual(other.Translation(), tolerance));
    }


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    void ToStream(std::ostream & outputStream) const {
        outputStream << "translation: "
                     << std::endl
                     << this->Translation()
                     << std::endl
                     << "rotation: "
                     << std::endl
                     << this->Rotation();
    }

};


template <class _rotationType>
inline vctFrameBase<_rotationType>
operator * (const vctFrameBase<_rotationType> & frame1,
            const vctFrameBase<_rotationType> & frame2) {
    vctFrameBase<_rotationType> result;
    frame1.ApplyTo(frame2, result);
    return result;
}

template<unsigned int _cols, int _rowStride, int _colStride, class _rotationType, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, 3, _cols >
operator *(const vctFrameBase<_rotationType> & frame,
           const vctFixedSizeConstMatrixBase<3, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<typename _rotationType::value_type, 3, _cols > result;
    frame.ApplyTo(matrix, result);
    return result;
}

/*! Stream out operator. */
template<class _rotationType>
std::ostream & operator << (std::ostream & output,
                            const vctFrameBase<_rotationType> & frame) {
    frame.ToStream(output);
    return output;
}

#endif  // _vctFrameBase_h

