/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2005-12-01

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation2Base_h
#define _vctMatrixRotation2Base_h

/*!
  \file
  \brief Declaration of vctMatrixRotation2Base
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctAngleRotation2.h>
#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation matrix for a space of dimension 2

  This class is templated by the type of container used to store the
  rotation matrix.  This class is an internal class, i.e. it is not
  intended for the end-user.  The class which should be used by most
  is vctMatRot2 (eventually vctMatrixRotation2<_elementType>).

  The main goal of this meta rotation matrix class is to ease the
  interface with Python.  For a native Python object, a matrix
  rotation can be defined based on a vctDynamicMatrix which is much
  easier to wrap than a vctFixedSizeMatrix.  For a C++ object accessed
  from Python, the rotation matrix will be defined using
  vctMatrixRotation2Base<vctDynamicMatrixRef<double> >, referring to
  the C++ vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >.

  \param _containerType The type of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _containerType>
class vctMatrixRotation2Base: public _containerType
{
public:

    enum {ROWS = 2, COLS = 2};
    enum {DIMENSION = 2};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctMatrixRotation2Base<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const CISST_THROW(std::runtime_error) {
        if (! this->IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation2Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const CISST_THROW(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation2Base: Input is not normalized"));
        }
    }


    /*!  Allocate memory for the underlying container if needed.  By
      default, this methods does nothing.  For any container requiring
      a memory allocation, it is necessary to specialize this
      method. */
    inline void Allocate(void) {}


public:

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation2Base()
    {
        this->Allocate();
        this->Assign(Identity());
    }

    /*! The assignment from BaseType (i.e. a 2 by 2 fixed size matrix)
      has to be redefined for this class (C++ restriction).  This
      operator uses the Assign() method inherited from the BaseType.
      This operator (as well as the Assign method) allows to set a
      rotation matrix to whatever value without any further validity
      checking.  It is recommended to use it with caution. */
    inline ThisType & operator = (const ContainerType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline vctMatrixRotation2Base(const ThisType & other):
        BaseType()
    {
        this->Allocate();
        this->Assign(other);
    }

    inline vctMatrixRotation2Base(const BaseType & other)
    {
        this->Allocate();
        this->Assign(other);
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


    /*! Constructor from 4 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation2<double> matrix( 0.0, 1.0,
                                        -1.0, 0.0);
      \endcode
    */
    inline vctMatrixRotation2Base(const value_type & element00, const value_type & element01,
                                  const value_type & element10, const value_type & element11)
        CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(element00, element01,
                   element10, element11);
    }



    /*!
      Constructor from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline vctMatrixRotation2Base(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1>& v1,
                                  const vctFixedSizeConstVectorBase<2, __stride2, value_type, __dataPtrType2>& v2,
                                  bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(v1, v2, vectorsAreColumns);
    }

    /*!
      Constructor from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2Base(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                                  const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                                  bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(v1, v2, vectorsAreColumns);
    }

    /*! Construction from a vctAngleRotation2. */
    inline vctMatrixRotation2Base(const vctAngleRotation2 & angleRotation)
        CISST_THROW(std::runtime_error)
    {
        this->Allocate();
        this->From(angleRotation);
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


    /*! Constructor from 4 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > matrix( 0.0, 1.0,
                                                                       -1.0, 0.0);
      \endcode
    */
    inline vctMatrixRotation2Base(const value_type & element00, const value_type & element01,
                                  const value_type & element10, const value_type & element11,
                                  bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(element00, element01,
                                 element10, element11);
        } else {
            this->FromRaw(element00, element01,
                          element10, element11);
        }
    }



    /*!
      Constructor from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline vctMatrixRotation2Base(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                                  const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                                  bool vectorsAreColumns, bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(v1, v2, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, vectorsAreColumns);
        }
    }

    /*!
      Constructor from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2Base(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                                  const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                                  bool vectorsAreColumns, bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(v1, v2, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, vectorsAreColumns);
        }
    }

    /*! Construction from a vctAngleRotation2. */
    inline vctMatrixRotation2Base(const vctAngleRotation2 & angleRotation,
                                  bool normalizeInput)
    {
        this->Allocate();
        if (normalizeInput) {
            this->FromNormalized(angleRotation);
        } else {
            this->FromRaw(angleRotation);
        }
    }
    //@}


    /*! Initialize this rotation matrix with a base matrix.  This
      constructor only takes a matrix of the same element type.

      \note This constructor does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.

      \note The constructor is declared explicit, to force the user to
      be aware of the conversion being made.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    explicit inline
    vctMatrixRotation2Base(const vctFixedSizeMatrixBase<ROWS, COLS, __rowStride, __colStride, value_type, __dataPtrType> & matrix)
    {
        this->Assign(matrix);
    }


    /*! Const reference to the identity.  In this case, the identity matrix:
      <pre>
      1 0
      0 1
      </pre>
    */
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

    /*! Conversion from 4 elements. */
    inline ThisType &
    From(const value_type & element00, const value_type & element01,
         const value_type & element10, const value_type & element11)
        CISST_THROW(std::runtime_error)
    {
        this->FromRaw(element00, element01,
                      element10, element11);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline ThisType &
    From(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
         const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
         bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline ThisType &
    From(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
         const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
         bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from an angle rotation. */
    inline ThisType &
    From(const vctAngleRotation2 & angleRotation)
        CISST_THROW(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(angleRotation);
        return this->FromRaw(angleRotation);
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

    /*! Conversion from 4 elements. */
    inline ThisType &
    FromNormalized(const value_type & element00, const value_type & element01,
                   const value_type & element10, const value_type & element11)
    {
        this->FromRaw(element00, element01,
                      element10, element11);
        this->NormalizedSelf();
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline ThisType &
    FromNormalized(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                   const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                   bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }


    /*!
      Conversion from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline ThisType &
    FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                   const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                   bool vectorsAreColumns = true)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }

    /*! Conversion from an angle rotation. */
    inline ThisType &
    FromNormalized(const vctAngleRotation2 & angleRotation)
    {
        return this->FromRaw(angleRotation.Normalized());
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


    /*! Conversion from 4 elements. */
    inline ThisType &
    FromRaw(const value_type & element00, const value_type & element01,
            const value_type & element10, const value_type & element11)
    {
        this->Assign(element00, element01,
                     element10, element11);
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline ThisType &
    FromRaw(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
            const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
            bool vectorsAreColumns = true)
    {
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
        }
        return *this;
    }

    /*!
      Conversion from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline ThisType &
    FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
            bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        CMN_ASSERT(v1.size() == DIMENSION);
        CMN_ASSERT(v2.size() == DIMENSION);
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
        }
        return *this;
    }

    /*! Conversion from an angle rotation */
    CISST_EXPORT ThisType &
    FromRaw(const vctAngleRotation2 & angleRotation);

    /*! A complementary form of assigning one matrix rotation to
      another.  The method is provided mostly for generic programming
      interfaces and for testing various operations on rotations */
    inline ThisType &
    FromRaw(const ThisType & otherRotation) {
        return reinterpret_cast<ThisType &>(this->Assign(otherRotation));
    }

    /*! Assign a 2x2 matrix to this rotation matrix.  This method does
      not substitute the Assign() method.  Assign() may perform type
      conversion, while From() only takes a matrix of the same element
      type.

      \note This method does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType &
    FromRaw(const vctFixedSizeMatrixBase<DIMENSION, DIMENSION, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        this->Assign(matrix);
        return *this;
    }

    //@}



    /*! Normalizes this matrix.  This method converts the matrix to an
      angle and convert back to a matrix. */
    CISST_EXPORT ThisType & NormalizedSelf(void);

    /*! Sets this rotation matrix as the normalized version of another one.
      \param otherMatrix Matrix used to compute the normalized matrix. */
    inline ThisType & NormalizedOf(const ThisType & otherMatrix) {
        *this = otherMatrix;
        this->NormalizedSelf();
        return *this;
    }

    /*! Returns the normalized version of this matrix.  This method
      returns a copy of the normalized version and does not modify
      this matrix. */
    ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }


    /*! Test if this matrix is normalized.  This methods checks that
      all the columns are normalized (within a margin of tolerance)
      and then checks that the two vectors are orthogonal to each
      other.

      \param tolerance Tolerance for the norm and scalar product tests.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const
    {
        if ((vctUnaryOperations<value_type>::AbsValue::Operate(value_type(this->Column(0).Norm() - 1)) > tolerance)
            || (vctUnaryOperations<value_type>::AbsValue::Operate(value_type(this->Column(1).Norm() - 1)) > tolerance)
            || (vctUnaryOperations<value_type>::AbsValue::Operate(this->Column(0).DotProduct(this->Column(1))) > tolerance)) {
            return false;
        } else {
            return true;
        }
    }


    /*! Inverse this rotation matrix.  This methods assumes that the
      matrix is normalized and sets this matrix as its transposed. */
    inline ThisType & InverseSelf(void) {
        // could use the transpose operator but this seems more efficient
        value_type tmp;
        tmp = this->Element(0, 1);
        this->Element(0, 1) = this->Element(1, 0);
        this->Element(1, 0) = tmp;
        return *this;
    }


    /*! Set this rotation as the inverse of another one.  See also
      InverseSelf(). */
    inline ThisType & InverseOf(const ThisType & otherRotation) {
        this->TransposeOf(otherRotation);
        return *this;
    }


    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy.  See
      also InverseSelf().  */
    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }


    /*! Apply the rotation to a vector of fixed size 2. The result is
      stored into another vector of size 2 provided by the caller and
      passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }


    /*! Apply the rotation to a vector of fixed size 2. The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input vector
      \return The output vector
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 2>
    ApplyTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const
    {
        vctFixedSizeVector<value_type, 2> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to another rotation.  The result is stored
      into a vctMatrixRotation2Base (ThisType) provided by the caller and
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
    inline ThisType
    ApplyTo(const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to a dynamic vector.  The result is stored
      into another dynamic vector passed by reference by the caller.
      It is assumed that both are of size 2.
    */
    template <class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        output[0] = this->Element(0, 0) * input[0] + this->Element(0, 1) * input[1];
        output[1] = this->Element(1, 0) * input[0] + this->Element(1, 1) * input[1];
    }


    /*! Apply the inverse of the rotation to a vector of fixed size
      2. The result is stored into a vector of size 2 provided by the
      caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }


    /*! Apply the the inverse of the rotation to a vector of fixed
      size 2. The result is returned by copy.  This interface might be
      more convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 2>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, 2> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctMatrixRotation2Base (ThisType) provided
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
      reference by the caller.  It is assumed that both are of size 2.
    */
    template <class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == DIMENSION);
        CMN_ASSERT(output.size() == DIMENSION);
        output[0] = this->Element(0, 0) * input[0] + this->Element(1, 0) * input[1];
        output[1] = this->Element(0, 1) * input[0] + this->Element(1, 1) * input[1];
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
    inline vctFixedSizeVector<value_type, 2>
    operator * (const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & input) const
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


#endif  // _vctMatrixRotation2Base_h
