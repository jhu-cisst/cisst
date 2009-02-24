/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2005-08-19

  (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Declaration of vctMatrixRotation3Base
 */


#ifndef _vctMatrixRotation3Base_h
#define _vctMatrixRotation3Base_h


#include <cisstVector/vctMatrixRotation3ConstBase.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _matrixType, class _quaternionType>
void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<_matrixType> & matrixRotation,
                              const vctQuaternionRotation3Base<_quaternionType> & quaternionRotation);

#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template CISST_EXPORT void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


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
class vctMatrixRotation3Base: public vctMatrixRotation3ConstBase<_containerType>
{
public:
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef _containerType ContainerType;
    typedef vctMatrixRotation3ConstBase<ContainerType> BaseType;
    typedef vctMatrixRotation3Base<ContainerType> ThisType;
    typedef vctMatrixRotation3Base<typename ContainerType::MatrixValueType> RotationValueType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

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

    /*! Conversion from 9 elements. */
    inline ThisType &
    From(const value_type & element00, const value_type & element01, const value_type & element02,
         const value_type & element10, const value_type & element11, const value_type & element12,
         const value_type & element20, const value_type & element21, const value_type & element22)
        throw(std::runtime_error)
    {
        this->FromRaw(element00, element01, element02,
                      element10, element11, element12,
                      element20, element21, element22);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline ThisType &
    From(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
         const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
         const vctFixedSizeConstVectorBase<DIMENSION, __stride3, value_type, __dataPtrType3>& v3,
         bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }


    /*!
      Conversion from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline ThisType &
    From(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
         const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
         const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
         bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from an axis/angle rotation. */
    inline ThisType &
    From(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(axisAngleRotation);
        return this->FromRaw(axisAngleRotation);
    }

    /*! Conversion from a rotation quaternion. */
    template <class __containerType>
    inline ThisType &
    From(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error) 
    {
        this->ThrowUnlessIsNormalized(quaternionRotation);
        return this->FromRaw(quaternionRotation);
    }


    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    From(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(rodriguezRotation);
        return this->FromRaw(rodriguezRotation);
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

    /*! Conversion from 9 elements. */
    inline ThisType &
    FromNormalized(const value_type & element00, const value_type & element01, const value_type & element02,
                   const value_type & element10, const value_type & element11, const value_type & element12,
                   const value_type & element20, const value_type & element21, const value_type & element22)
    {
        this->FromRaw(element00, element01, element02,
                      element10, element11, element12,
                      element20, element21, element22);
        this->NormalizedSelf();
        return *this;
    }

    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline ThisType &
    FromNormalized(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                   const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                   const vctFixedSizeConstVectorBase<DIMENSION, __stride3, value_type, __dataPtrType3>& v3,
                   bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }


    /*!
      Conversion from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline ThisType &
    FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                   const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                   const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                   bool vectorsAreColumns = true)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }
    
    /*! Conversion from an axis/angle rotation. */
    inline ThisType &
    FromNormalized(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
    {
        return this->FromRaw(axisAngleRotation.Normalized());
    }

    /*!
      Conversion from a rotation quaternion.
      
      \param quaternionRotation A rotation quaternion 
    */
    template <class __containerType>
    inline ThisType &
    FromNormalized(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
    {
        return this->FromRaw(quaternionRotation.Normalized());
    }


    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    FromNormalized(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
    {
        return this->FromRaw(rodriguezRotation.Normalized());
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


    /*! Conversion from 9 elements. */
    inline ThisType &
    FromRaw(const value_type & element00, const value_type & element01, const value_type & element02,
            const value_type & element10, const value_type & element11, const value_type & element12,
            const value_type & element20, const value_type & element21, const value_type & element22)
    {
        this->Assign(element00, element01, element02,
                     element10, element11, element12,
                     element20, element21, element22);
        return *this;
    }


    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline ThisType &
    FromRaw(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
            const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
            const vctFixedSizeConstVectorBase<DIMENSION, __stride3, value_type, __dataPtrType3>& v3,
            bool vectorsAreColumns = true)
    {
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
            this->Column(2).Assign(v3);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
            this->Row(2).Assign(v3);
        }
        return *this;
    }
    
    
    /*!
      Conversion from 3 dynamic vectors.
      
      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline ThisType &
    FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
            const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
            bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        CMN_ASSERT(v1.size() == DIMENSION);
        CMN_ASSERT(v2.size() == DIMENSION);
        CMN_ASSERT(v3.size() == DIMENSION);
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
            this->Column(2).Assign(v3);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
            this->Row(2).Assign(v3);
        }
        return *this;
    }


    /*! Conversion from an axis/angle rotation */
    CISST_EXPORT ThisType &
    FromRaw(const vctAxisAngleRotation3<value_type> & axisAngleRotation);


    /*! Conversion from a quaternion rotation. */
    template <class __containerType>
    inline ThisType &
    FromRaw(const vctQuaternionRotation3Base<__containerType> & quaternionRotation) {
        vctMatrixRotation3BaseFromRaw(*this, quaternionRotation);
        return *this;
    }

    
    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline ThisType &
    FromRaw(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(vctAxisAngleRotation3<value_type>(rodriguezRotation, VCT_DO_NOT_NORMALIZE));
    }
    
    /*! A complementary form of assigning one matrix rotation to
      another.  The method is provided mostly for generic programming
      interfaces and for testing various operations on rotations */
    inline ThisType &
    FromRaw(const ThisType & otherRotation) {
        return reinterpret_cast<ThisType &>(this->Assign(otherRotation));
    }


    /*! Assign a 3x3 matrix to this rotation matrix.  This method does
      not substitute the Assign() method.  Assign() may perform type
      conversion, while From() only takes a matrix of the same element
      type.

      \note This method does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType &
    FromRaw(const vctFixedSizeMatrixBase<ROWS, COLS, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        this->Assign(matrix);
        return *this;
    }
    //@}


    /*! Normalizes this matrix.  This method converts the matrix to a
      quaternion, normalizes it and convert back to a matrix. */
    ThisType & NormalizedSelf(void) {
        vctQuaternionRotation3Base<vctFixedSizeVector<value_type, 4> > quaternion;
        quaternion.FromRaw(*this);
        quaternion.NormalizedSelf();
        this->From(quaternion);
        return *this;
    }


    /*! Set this rotation matrix as the normalized version of another one.    
      \param otherMatrix Matrix used to compute the normalized matrix. */
    inline ThisType & NormalizedOf(ThisType & otherMatrix) {
        CMN_ASSERT(otherMatrix.Pointer() != this->Pointer());
        *this = otherMatrix;
        this->NormalizedSelf();
        return *this;
    }

    
    /*! Inverse this rotation matrix.  This methods assumes that the
      matrix is normalized and sets this matrix as its transposed. */
    inline ThisType &
    InverseSelf(void) {
        // could use the transpose operator but this seems more efficient
        value_type tmp;
        tmp = this->Element(0, 1); this->Element(0, 1) = this->Element(1, 0); this->Element(1, 0) = tmp;
        tmp = this->Element(0, 2); this->Element(0, 2) = this->Element(2, 0); this->Element(2, 0) = tmp;
        tmp = this->Element(1, 2); this->Element(1, 2) = this->Element(2, 1); this->Element(2, 1) = tmp;
        return *this;
    }
    
    
    /*! Set this rotation as the inverse of another one.  See also
        InverseSelf(). */
    inline ThisType &
    InverseOf(const ThisType & otherRotation) {
        CMN_ASSERT(otherRotation.Pointer() != this->Pointer());
        this->TransposeOf(otherRotation);
        return *this;
    }
};


#endif  // _vctMatrixRotation3Base_h

