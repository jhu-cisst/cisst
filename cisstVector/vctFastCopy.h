/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2006-11-10

  (C) Copyright 2006-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFastCopy_h
#define _vctFastCopy_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnRequiresDeepCopy.h>

/*! \brief Container class for fast copy related methods.

  The static methods in this class should not be used directly.  These
  are helper methods used by the containers methods.  To perform a
  FastCopy (aka memcpy), always use the methods FastCopyOf and
  FastCopyCompatible defined for vctFixedSizeVector,
  vctFixedSizeMatrix, vctDynamicVector, vctDynamicMatrix and
  vctDynamicNArray.
*/
class vctFastCopy {

protected:
    /*! Helper functions to test if sizes or strides are
        compatible. */
    //@{
    template <class _vector1Type, class _vector2Type>
    inline static bool VectorSizeCompatible(const _vector1Type & vector1,
                                            const _vector2Type & vector2)
    {
        return (vector1.size() == vector2.size());
    }

    template <class _container1Type, class _container2Type>
    inline static bool ContainerSizesCompatible(const _container1Type & container1,
                                                const _container2Type & container2)
    {
        return (container1.sizes() == container2.sizes());
    }

    template <class _vector1Type, class _vector2Type>
    inline static bool VectorStrideCompatible(const _vector1Type & vector1,
                                              const _vector2Type & vector2)
    {
        return ((vector1.stride() == 1) && (vector2.stride() == 1));
    }


    template <class _matrix1Type, class _matrix2Type>
    inline static bool MatrixStridesCompatible(const _matrix1Type & matrix1,
                                               const _matrix2Type & matrix2)
    {
        return (
                /* compact and same storage order */
                (matrix1.IsCompact() && matrix2.IsCompact()
                 && (matrix1.strides() == matrix2.strides()))
                ||
                /* or row compact */
                ((matrix1.row_stride() == 1) && matrix2.row_stride() == 1)
                ||
                /* or column compact */
                ((matrix1.col_stride() == 1) && matrix2.col_stride() == 1));
    }

    template <class _nArray1Type, class _nArray2Type>
    inline static bool NArrayStridesCompatible(const _nArray1Type & nArray1,
                                               const _nArray2Type & nArray2)
    {
        return (nArray1.IsCompact() && nArray2.IsCompact()
                && (nArray1.strides() == nArray2.strides()));
    }
    //@}


	/*! Helper function to throw an exception whenever sizes mismatch.
      This enforces that a standard message is sent. */
    //@{
    template <class _vector1Type, class _vector2Type>
    inline static void ThrowUnlessValidVectorSizes(const _vector1Type & vector1,
                                                   const _vector2Type & vector2)
        CISST_THROW(std::runtime_error)
    {
        if (!VectorSizeCompatible(vector1, vector2)) {
            cmnThrow(std::runtime_error("vctFastCopy: Vector sizes mismatch"));
        }
    }

    template <class _container1Type, class _container2Type>
    inline static void ThrowUnlessValidContainerSizes(const _container1Type & container1,
                                                      const _container2Type & container2)
        CISST_THROW(std::runtime_error)
    {
        if (!ContainerSizesCompatible(container1, container2)) {
            cmnThrow(std::runtime_error("vctFastCopy: Container sizes mismatch"));
        }
    }
    //@}

public:

    /*! Flags used to skip or perform safety checks in FastCopy. */
    //@{
    static const bool SkipChecks = false;
    static const bool PerformChecks = true;
    //@}

    /*! Helper functions to test if a fast copy is possible. */
    //@{
    template <class _vector1Type, class _vector2Type>
    inline static bool VectorCopyCompatible(const _vector1Type & vector1,
                                            const _vector2Type & vector2)
    {
        return (!cmnRequiresDeepCopy<typename _vector1Type::value_type>()
                && !cmnRequiresDeepCopy<typename _vector2Type::value_type>()
                && VectorSizeCompatible(vector1, vector2)
                && VectorStrideCompatible(vector1, vector2));
    }

    template <class _matrix1Type, class _matrix2Type>
    inline static bool MatrixCopyCompatible(const _matrix1Type & matrix1,
                                            const _matrix2Type & matrix2)
    {
        return (!cmnRequiresDeepCopy<typename _matrix1Type::value_type>()
                && !cmnRequiresDeepCopy<typename _matrix2Type::value_type>()
                && ContainerSizesCompatible(matrix1, matrix2)
                && MatrixStridesCompatible(matrix1, matrix2));
    }

    template <class _nArray1Type, class _nArray2Type>
    inline static bool NArrayCopyCompatible(const _nArray1Type & nArray1,
                                            const _nArray2Type & nArray2)
    {
        return (!cmnRequiresDeepCopy<typename _nArray1Type::value_type>()
                && !cmnRequiresDeepCopy<typename _nArray2Type::value_type>()
                && ContainerSizesCompatible(nArray1, nArray2)
                && NArrayStridesCompatible(nArray1, nArray2));
    }
    //@}


    /*! Function to use memcpy whenever possible.  This is not really
      and engine since it has no loop nor plugable operation, but it
      performs a similar size check as other engines.
    */
    template <class _destinationVectorType, class _sourceVectorType>
    inline static bool VectorCopy(_destinationVectorType & destination,
                                  const _sourceVectorType & source,
                                  bool performSafetyChecks)
    {
        typedef _sourceVectorType SourceVectorType;
        typedef typename SourceVectorType::value_type value_type;

        if (performSafetyChecks) {
            // test size
            ThrowUnlessValidVectorSizes(source, destination);

            // test layout
            if (! VectorStrideCompatible(destination, source)) {
                return false;
            }
        }
        cmnMemcpy(destination.Pointer(), source.Pointer(), source.size() * sizeof(value_type));
        return true;
    }


    /*! Function to use memcpy whenever possible.  This is not really
      and engine since it has no loop nor plugable operation, but it
      performs a similar size check as other engines.  This method
      will attempt to copy either the whole matrix using a single
      memcpy or one memcpy for each row or column whenever possible.

      First the method performs a serie of checks to make sure both
      matrices are compatible.  If the sizes are incorrect, the method
      will throw an exception as do all cisstVector engines.  If the
      strides are not compatible for a memcpy, the method return
      false.  The safety checks can be skipped by turning the second
      parameter (performSafetyChecks) to false.

      Then the method will test if both matrices are compact to figure
      out if a single memcpy is possible.  Otherwise, the methods will
      compare the strides, assuming that either both row or column
      strides are equal to one.

      \note If the safety checks are turned off, the caller must make
      sure that for both matrices have the same sizes and satisfy
      either: -1- both compact, -2- both have column strides equal to
      one OR -3- both have row strides equal to one.  If these
      conditions are not satified, a memcpy might be performed on
      unallocated memory.
    */
    template <class _destinationMatrixType, class _sourceMatrixType>
    inline static bool MatrixCopy(_destinationMatrixType & destination,
                                  const _sourceMatrixType & source,
                                  bool performSafetyChecks)
    {
        typedef _sourceMatrixType SourceMatrixType;
        typedef typename SourceMatrixType::value_type value_type;

        if (performSafetyChecks) {
            // test size
            ThrowUnlessValidContainerSizes(source, destination);

            // test layout
            if (! MatrixStridesCompatible(destination, source)) {
                return false;
            }
        }
        // at that point we know the matrices are compatible.  We need to know if we memcpy all or by rows or column
        if (destination.IsCompact() && source.IsCompact()) {
            cmnMemcpy(destination.Pointer(), source.Pointer(), source.size() * sizeof(value_type));
            return true;
        } else {
            typedef _destinationMatrixType DestinationMatrixType;
            typedef _sourceMatrixType SourceMatrixType;

            typedef typename DestinationMatrixType::size_type size_type;
            typedef typename DestinationMatrixType::stride_type stride_type;

            typedef typename DestinationMatrixType::pointer DestinationPointerType;
            typedef typename SourceMatrixType::const_pointer SourcePointerType;

            const size_type rows = destination.rows();
            const size_type cols = destination.cols();

            DestinationPointerType destinationPointer = destination.Pointer();
            SourcePointerType sourcePointer = source.Pointer();

            const stride_type destinationRowStride = destination.row_stride();
            const stride_type sourceRowStride = source.row_stride();
            const stride_type destinationColStride = destination.col_stride();
            const stride_type sourceColStride = source.col_stride();


            if ((destinationColStride == 1) && (sourceColStride == 1)) {
                /* copy row by row */
                const size_type sizeOfRow = cols * sizeof(value_type);

                const DestinationPointerType destinationRowEnd = destinationPointer + rows * destinationRowStride;

                for (;
                     destinationPointer != destinationRowEnd;
                     destinationPointer += destinationRowStride, sourcePointer += sourceRowStride) {
                    cmnMemcpy(destinationPointer, sourcePointer, sizeOfRow);
                }
                return true;
            } else {
                /* copy column by column */
                const size_type sizeOfCol = rows * sizeof(value_type);
                const DestinationPointerType destinationColEnd = destinationPointer + cols * destinationColStride;

                for (;
                     destinationPointer != destinationColEnd;
                     destinationPointer += destinationColStride, sourcePointer += sourceColStride) {
                    cmnMemcpy(destinationPointer, sourcePointer, sizeOfCol);
                }
                return true;
            }
        }
    }


    /*! Function to use memcpy whenever possible.  This is not really
      and engine since it has no loop nor plugable operation, but it
      performs a similar size check as other engines.  This method
      will attempt to copy either the whole nArray using a single
      memcpy. */
    template <class _destinationNArrayType, class _sourceNArrayType>
    inline static bool NArrayCopy(_destinationNArrayType & destination,
                                  const _sourceNArrayType & source,
                                  bool performSafetyChecks)
    {
        typedef _sourceNArrayType SourceNArrayType;
        typedef typename SourceNArrayType::value_type value_type;

        if (performSafetyChecks) {
            // test size
            ThrowUnlessValidContainerSizes(source, destination);

            // test layout
            if (! NArrayStridesCompatible(destination, source)) {
                return false;
            }
        }
        cmnMemcpy(destination.Pointer(), source.Pointer(), source.size() * sizeof(value_type));
        return true;
    }
};


#endif  // _vctFastCopy_h
