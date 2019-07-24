/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Anton Deguet
  Created on: 2006-01-10

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrLU
*/


#ifndef _nmrLU_h
#define _nmrLU_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// Always include last
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  \brief Data of LU problem (Dynamic).

  The result of an LU decomposition is composed of three different
  containers, the matrices P, L and U defined by \f$ A = P * L * U\f$.
  The sizes of these components must match exactly the input matrix A.
  To ease the use of the LU routine, the user can rely on the
  nmrLUDynamicData class to perform the required memory
  allocation.

  Another good reason to use a data object is that the memory
  allocation can be performed once during an initialization phase
  while the function nmrLU can be called numerous times later on
  without any new dynamic memory allocation.  This is crucial for such
  things as real time tasks.

  The LU routine, as most LAPACK and FORTRAN based routines, requires
  the input to be stored column first.  In cisstVector, this implies
  that all matrices must be created using #VCT_COL_MAJOR.
  Furthermore, all matrices and vectors must also be compact, i.e. use
  a contiguous block of memory.

  Any size or storage order mismatch will lead to an exception thrown
  (std::runtime_error).  Since we are using cmnThrow, it is possible
  to configure cisst (at compilation time) to abort the program
  instead of throwing an exception.

  The nmrLUDynamicData class allows 2 different configurations:

  - Allocate automatically the output (PivotIndices vector).  This can
    be performed using either the constructor from input matrix
    (i.e. nmrLUDynamicData(A)) or using the method Allocate(A).

  - Don't allocate anything.  The user has to provide the container he
    wants to use for the pivot indices.  In this case, the data is
    used mostly to check that the container is valid in terms of size.
    This can be performed using either the constructor from
    PivotIndices (i.e. nmrSVDDynamicData(A, PivotIndices)) or the
    method SetRef(A, PivotIndices).

  \sa nmrLU
 */
class nmrLUDynamicData {

public:
    /*! Type used for sizes within nmrLUDynamicData.  This type is
      compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #CISSTNETLIB_INTEGER. */
    typedef vct::size_type size_type;

    typedef vctFixedSizeVector<size_type, 2> nsize_type;

protected:
    /*! Memory allocated for pivot indices vector if needed. */
    vctDynamicVector<CISSTNETLIB_INTEGER> OutputMemory;

    /*! Reference return type, this points either to user allocated
      memory or our memory chunk if needed.
     */
    vctDynamicVectorRef<CISSTNETLIB_INTEGER> PivotIndicesReference;

    /*! Just store M, and N which are needed to check if A matrix
       passed to solve method matches the allocated size. */
    //@{
    size_type MMember;
    size_type NMember;
    //@}

    /*! Private method to set the data members MMember and NMember.
      This method must be called before AllocateOutput,
      ThrowUnlessOutputSizeIsCorrect or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type m, size_type n)
    {
        MMember = m;
        NMember = n;
    }

    /*! Private method to allocate memory for the output if needed.
      This method assumes that the dimension m and n as well as the
      storage order are already set.  It is important to use this
      method in all the methods provided in the user API, even if all
      the memory is provided by the user since this method will ensure
      that the data (nmrLUDynamicData) does not keep any memory
      allocated.  This is for the case where a single data object is
      used first to allocate everything and, later on, used with user
      allocated memory (for the output).

      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutput(bool allocateOutput)
    {
        // allocate output
        if (allocateOutput) {
            const size_type minmn = (MMember < NMember) ? MMember : NMember;
            this->OutputMemory.SetSize(minmn);
            this->PivotIndicesReference.SetRef(this->OutputMemory);
        } else {
            this->OutputMemory.SetSize(0);
        }
    }


    /*!  Verifies that the user provided references for the output
      match the size of the data object as set by SetDimension.  This
      method also checks that all containers are compact.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypePivotIndices>
    inline void ThrowUnlessOutputSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices)
        CISST_THROW(std::runtime_error)
    {
         // check sizes and compacity
        const size_type minmn = (MMember < NMember) ? MMember : NMember;
        if (minmn != pivotIndices.size()) {
            cmnThrow(std::runtime_error("nmrLUDynamicData: Size of vector pivotIndices is incorrect."));
        }
        if (!pivotIndices.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLUDynamicData: Vector pivotIndices must be compact."));
        }
    }


public:

    /*! Helper method to compute the size of the permutation matrix.
      This method can be used before UpdateMatrixP to make sure that
      the size of P is correct.

      \param A The matrix to be decomposed using ::nmrLU (it is used
      only to determine the sizes).
    */
    template <class _matrixOwnerTypeA>
    static inline
    nsize_type MatrixPSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        nsize_type matrixSize(A.rows(), A.rows());
        return matrixSize;
    }

    /*! Helper method to compute the size of the matrix L.  This
      method can be used before UpdateMatrixLU to make sure that the
      size of L is correct.

      \param A The matrix to be decomposed using ::nmrLU (it is used
      only to determine the sizes).
    */
    template <class _matrixOwnerTypeA>
    static inline
    nsize_type MatrixLSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        const size_type minmn = (A.rows() < A.cols()) ? A.rows() : A.cols();
        nsize_type matrixSize(A.rows(), minmn);
        return matrixSize;
    }

    /*! Helper method to compute the size of the matrix U.  This
      method can be used before UpdateMatrixLU to make sure that the
      size of U is correct.

      \param A The matrix to be decomposed using ::nmrLU (it is used
      only to determine the sizes).
    */
    template <class _matrixOwnerTypeA>
    static inline
    nsize_type MatrixUSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        const size_type minmn = (A.rows() < A.cols()) ? A.rows() : A.cols();
        nsize_type matrixSize(minmn, A.cols());
        return matrixSize;
    }

    /*! Helper method to create a usable permutation matrix from the
      vector of pivot indices created by ::nmrLU.

      \note This method must be called after the ::nmrLU function has been called.

      \param A The matrix decomposed using ::nmrLU.  It is used only to check the sizes.
      \param pivotIndices The vector of pivot indices as computed by ::nmrLU
      \param P The permutation matrix updated from the pivot indices.
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypePivotIndices, class _matrixOwnerTypeP>
    static inline
    vctDynamicMatrixBase<_matrixOwnerTypeP, CISSTNETLIB_DOUBLE> &
    UpdateMatrixP(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                  const vctDynamicConstVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices,
                  vctDynamicMatrixBase<_matrixOwnerTypeP, CISSTNETLIB_DOUBLE> & P)
        CISST_THROW(std::runtime_error)
    {
        const size_type minmn = (A.rows() < A.cols()) ? A.rows() : A.cols();
        // check sizes
        if (pivotIndices.size() != minmn) {
            cmnThrow(std::runtime_error("nmrLUDynamicData::UpdateMatrixP: Size of vector pivotIndices is incorrect."));
        }
        if (! P.IsSquare(A.rows())) {
            cmnThrow(std::runtime_error("nmrLUDynamicData::UpdateMatrixP: Size of matrix P is incorrect."));
        }
        // update permutation matrix
        P.SetAll(0.0);
        P.Diagonal().SetAll(1.0);
        size_type rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < minmn; ++rowIndex) {
            colIndex = pivotIndices[rowIndex] - 1;
            P.ExchangeColumns(rowIndex, colIndex);
        }
        return P;
    }

    /*! Helper method to create usable matrix L and U from the input
      matrix used and modified by ::nmrLU.  The output of ::nmrLU is a
      single matrix which contains both L and U.  This method splits
      the output of nmrLU and creates to matrices by copying the lower
      and upper parts respectively in L and U, setting all other
      elements to zero.  The diagonal of the output is copied to U
      while all the elements of the diagonal of L are set to 1.

      \note This method must be called after the ::nmrLU function has been called.

      \param A The matrix decomposed using ::nmrLU.
      \param L The lower matrix
      \param U The upper matrix
    */
    template <class _matrixOwnerTypeA, class _matrixOwnerTypeL, class _matrixOwnerTypeU>
    static inline
    void UpdateMatrixLU(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                        vctDynamicMatrixBase<_matrixOwnerTypeL, CISSTNETLIB_DOUBLE> & L,
                        vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U)
        CISST_THROW(std::runtime_error)
    {
        const size_type rows = A.rows();
        const size_type cols = A.cols();
        size_type rowIndex, colIndex;
        L.SetAll(0.0);
        L.Diagonal().SetAll(1.0);
        U.SetAll(0.0);
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for (colIndex = 0; colIndex < cols; ++colIndex) {
                if (rowIndex > colIndex) {
                    L.Element(rowIndex, colIndex) = A.Element(rowIndex, colIndex);
                } else {
                    U.Element(rowIndex, colIndex) = A.Element(rowIndex, colIndex);
                }
            }
        }

    }


#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated LU function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrLUDynamicData & Data;
    public:
        Friend(nmrLUDynamicData &data): Data(data) {
        }
        inline vctDynamicVectorRef<CISSTNETLIB_INTEGER> & PivotIndices(void) {
            return Data.PivotIndicesReference;
        }
        inline size_type M(void) {
            return Data.MMember;
        }
        inline size_type N(void) {
            return Data.NMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN
#endif // SWIG

    /*! The default constuctor.  For dynamic size, there are assigned
      default values, i.e. sets all the dimensions to zero.  These
      MUST be changed by calling the appropriate method.

      \sa nmrLUDynamicData::Allocate
      nmrLUDynamicData::SetRef
    */
    nmrLUDynamicData():
        MMember(static_cast<size_type>(0)),
        NMember(static_cast<size_type>(0))
    {
        AllocateOutput(false);
    }

    /*! Constructor where the user specifies the size.  Memory
      allocation is performed for pivot indices vector. This should be
      used when the user doesn't care much about where the output
      should be stored.

      \param m, n Dimension of the matrix to be decomposed.

      \sa nmrLUDynamicData::Allocate
    */
    nmrLUDynamicData(size_type m, size_type n)
    {
        this->Allocate(m, n);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for pivot indices vector. This should be used when the
      user doesn't care much about where the output should be stored.

      \param A input matrix

      \sa nmrLUDynamicData::Allocate
    */
    template <class _matrixOwnerTypeA>
    nmrLUDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A);
    }

    /*! Constructor where the user provides the vector to store the
      pivot indices.  The data object now acts as a composite
      container to hold, pass and manipulate a convenient storage for
      LU algorithm. Checks are made on the validity of the input and
      its consitency in terms of size.

      \param A The matrix to be decomposed, used to verify the sizes.
      \param pivotIndices Vector created by the user to store the output.

      \sa nmrLUDynamicData::SetRef
    */
    template <class _matrixOwnerTypeA,
              class _vectorOwnerTypePivotIndices>
    nmrLUDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                     vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices)
    {
        this->SetRef(A, pivotIndices);
    }

    /*! This method allocates memory for the output vector (pivot
      indices).  The input matrix is used only to determine the size
      of the problem.

      This method should be called before the nmrLUDynamicData
      object is passed on to nmrLU function.

      \param A The matrix for which LU needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A.rows(), A.cols());
    }

    /*! This method allocates the memory for the output (pivot
      indices).  This method is not meant to be a top-level user API,
      but is used by other overloaded Allocate methods.

      \param m Number of rows of input matrix A.
      \param n Number of cols of input matrix A.
    */
    void Allocate(size_type m, size_type n)
    {
        this->SetDimension(m, n);
        this->AllocateOutput(true);
    }

    /*! This method doesn't allocate any memory as it relies on the user
      provided vector (pivotIndices).

      The data object now acts as a composite container to hold,
      pass and manipulate a convenient storage for LU algorithm.  The
      method tests that all the containers provided by the user have
      the correct size and are compact.

      \param A The matrix to be decomposed, used to verify the sizes.
      \param pivotIndices Vector created by the user to store the output.
    */
    template <class _matrixOwnerTypeA,
              class _vectorOwnerTypePivotIndices>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices)
        CISST_THROW(std::runtime_error)
    {
        this->SetDimension(A.rows(), A.cols());
        this->AllocateOutput(false);
        this->ThrowUnlessOutputSizeIsCorrect(pivotIndices);
        this->PivotIndicesReference.SetRef(pivotIndices);
    }

    /*! Const reference to the result vector PivotIndices.  This
      method must be called after the data has been computed by
      the nmrLU function. */
    inline const vctDynamicVectorRef<CISSTNETLIB_INTEGER> & PivotIndices(void) const {
        return PivotIndicesReference;
    }
};



/*!
  \ingroup cisstNumerical

  \brief Data of LU problem (Fixed size).

  This class is similar to nmrLUDynamicData except that it is
  dedicated to fixed size containers.  While nmrLUDynamicData is
  designed to be modified dynamically, nmrLUFixedSizeData is
  fully defined at compilation time using template parameters.  The
  required parameters are the dimensions of the input matrix:
  \code
  nmrLUFixedSizeData<4, 3> data;
  \endcode

  \note An object of type nmrLUFixedSizeData contains the memory
  required for the output, i.e. its actual size will be equal to the
  memory required to store the vector PivotIndices.

  \note There is no dynamic memory allocation (no \c new) and the
  memory can not be used by reference.  To use memory by reference,
  one must use nmrLUDynamicData with vctDynamicMatrixRef and
  vctDynamicVectorRef (these dynamic references can actually be used
  to overlay a fixed size container).
 */
#ifndef SWIG
template <vct::size_type _rows, vct::size_type _cols>
class nmrLUFixedSizeData
{
public:
#ifndef DOXYGEN
    typedef vct::size_type size_type;
    enum {MIN_MN = (_rows < _cols) ? _rows : _cols};
#endif // DOXYGEN
    /*! Type of the input matrix A (size computed from the data
      template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> MatrixTypeA;
    /*! Type of the output vector PivotIndices (size computed from the
      data template parameters). */
    typedef vctFixedSizeVector<CISSTNETLIB_INTEGER, MIN_MN> VectorTypePivotIndices;
    /*! Type used to create the permutation matrix from the vector
      PivotIndices. */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _rows, VCT_COL_MAJOR> MatrixTypeP;
    /*! Type used to create the L matrix from the input matrix (A)
      after nmrLU has been called.  */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, MIN_MN, VCT_COL_MAJOR> MatrixTypeL;
    /*! Type used to create the U matrix from the input matrix (A)
      after nmrLU has been called.  */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, MIN_MN, _cols, VCT_COL_MAJOR> MatrixTypeU;

protected:
    VectorTypePivotIndices PivotIndicesMember; /*!< Data member used to store the output vector PivotIndices. */

public:
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated LU function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
    */
    class Friend {
    private:
        nmrLUFixedSizeData<_rows, _cols> & Data;
    public:
        Friend(nmrLUFixedSizeData<_rows, _cols> &data): Data(data) {
        }
        inline VectorTypePivotIndices & PivotIndices(void) {
            return Data.PivotIndicesMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN

    /*! Default constructor.  Does nothing since the allocation is
      performed on the stack. */
    nmrLUFixedSizeData() {};

    /*! Const reference to the result vector PivotIndices.  This
      method must be called after the data has been computed by
      the nmrLU function. */
    inline const VectorTypePivotIndices & PivotIndices(void) const {
        return PivotIndicesMember;
    }


    /*! Helper method to create a usable permutation matrix from the
      vector of pivot indices created by ::nmrLU.

      \note This method must be called after the ::nmrLU function has been called.

      \param pivotIndices The vector of pivot indices as computed by ::nmrLU
      \param P The permutation matrix updated from the pivot indices.
    */
    inline static MatrixTypeP &
    UpdateMatrixP(const VectorTypePivotIndices & pivotIndices,
                  MatrixTypeP & P)
        CISST_THROW(std::runtime_error)
    {
        // update permutation matrix
        P.SetAll(0.0);
        P.Diagonal().SetAll(1.0);
        size_type rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < MIN_MN; ++rowIndex) {
            colIndex = pivotIndices[rowIndex] - 1;
            P.ExchangeColumns(rowIndex, colIndex);
        }
        return P;
    }


    /*! Helper method to create usable matrix L and U from the input
      matrix used and modified by ::nmrLU.  The output of ::nmrLU is a
      single matrix which contains both L and U.  This method splits
      the output of nmrLU and creates to matrices by copying the lower
      and upper parts respectively in L and U, setting all other
      elements to zero.  The diagonal of the output is copied to U
      while all the elements of the diagonal of L are set to 1.

      \note This method must be called after the ::nmrLU function has been called.

      \param A The matrix decomposed using ::nmrLU.
      \param L The lower matrix
      \param U The upper matrix
    */
    static inline
    void UpdateMatrixLU(const MatrixTypeA & A,
                        MatrixTypeL & L,
                        MatrixTypeU & U)
        CISST_THROW(std::runtime_error)
    {
        vct::size_type rowIndex, colIndex;
        L.SetAll(0.0);
        L.Diagonal().SetAll(1.0);
        U.SetAll(0.0);
        for (rowIndex = 0; rowIndex < _rows; ++rowIndex) {
            for (colIndex = 0; colIndex < _cols; ++colIndex) {
                if (rowIndex > colIndex) {
                    L.Element(rowIndex, colIndex) = A.Element(rowIndex, colIndex);
                } else {
                    U.Element(rowIndex, colIndex) = A.Element(rowIndex, colIndex);
                }
            }
        }
    }
};
#endif // SWIG



/*!
  \name Algorithm LU: Lower Upper Decomposition

  These functions are different wrappers for the LAPACK function
  dgetrf.  They compute an LU factorization of a general M-by-N matrix
  A using partial pivoting with row interchanges.

  The factorization has the form \f$ A = P * L * U \f$ where P is a
  permutation matrix, L is lower triangular with unit diagonal
  elements (lower trapezoidal if m > n), and U is upper triangular
  (upper trapezoidal if m < n).

  These functions are wrappers around the LAPACK routine dgetrf,
  therefore they share some features with the LAPACK routine:
  <ol>
  <li>On exit, the content of A is altered.
  <li>The vectors and matrices must be compact, i.e. use a contiguous
  block of memory.
  </ol>

  The ::nmrLU functions add the following features:
  <ol>
  <li>A simplified interface to the cisstVector matrices, either
  vctDynamicMatrix or vctFixedSizeMatrix.
  <li>Input validation checks are performed, i.e. an
  std::runtime_error exception will be thrown if the sizes or storage
  order don't match or if the containers are not compact.
  <li>Helper classes to allocate memory for the output and workspace:
  nmrLUFixedSizeData and nmrLUDynamicData.
  <li>Methods to create usable matrices P, L and U from the pivot
  indices and the resulting matrix A.
  </ol>

  There are different ways to call this function to compute the LU of
  the matrix A.  These correspond to different overloaded ::nmrLU
  functions:
  <ol>
  <li>Using a preallocated data object.

  The user creates the input matrix A:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(12, 24 , VCT_COL_MAJOR); // 12 x 24 matrix
  vctRandom(A, -10.0, 10.0);
  \endcode
  The user allocates a data object which could be of
  type nmrLUFixedSizeData or nmrLUDynamicData.
  corresponding to fixed size or dynamic matrix A:
  \code
  nmrLUDynamicData data(A);
  \endcode
  Call the nmrLU function:
  \code
  nmrLU(A, data);
  \endcode
  The content of input matrix A is modified by this routine and a
  vector the pivot indices has been updated.  If the user needs the
  actual matrices P, L and U, he can use the different helper methods
  of the data:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> P, L, U;
  nmrLUDynamicData::SetSizeP(A, P);
  nmrLUDynamicData::SetSizeLU(A, L, U);
  nmrLUDynamicData::UpdateMatrixP(A, data.PivotIndices(), P);
  nmrLUDynamicData::UpdateMatrixLU(A, L, U);
  std::cout << P * L * U << std::endl;
  \endcode
  The matrices P, L and U can used any storage order since they are
  not used by LAPACK.

  <li>The user provides the vector pivotIndices.

  The User allocates memory for this vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4, VCT_COL_MAJOR);
  vctRandom(A, -10.0, 10.0);
  vctDynamicVector<CISSTNETLIB_INTEGER> pivotIndices(4);
  \endcode
  Call the LU routine:
  \code
  nmrLU(A, pivotIndices);
  \endcode
  The LU function verifies that the size of the data objects
  matches the input.

  <li>Using a data for fixed size containers.

  \code
  vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, 5, 4, VCT_COL_MAJOR> A;
  vctRandom(A, -10.0, 10.0);
  typedef nmrLUFixedSizeData<4, 3> DataType;
  DataType data;
  nmrLU(A, data);
  typename DataType::MatrixTypeP P;
  typename DataType::MatrixTypeL L;
  typename DataType::MatrixTypeU U;
  DataType::UpdateMatrixP(data.PivotIndices(), P);
  DataType::UpdateMatrixLU(A, L, U);
  \endcode

  </ol>

  \note The LU functions make use of LAPACK routines.  To activate
  this code, set the CISST_HAS_CNETLIB or CISST_HAS_CISSTNETLIB flag
  to ON during the configuration of cisst with CMake.
 */
//@{


/*! This function solves the LU problem for a dynamic matrix using an
  nmrLUDynamicData.

  This function checks for valid input (size and compact) and calls
  the LAPACK function.  If the input doesn't match the data, an
  exception is thrown (\c std::runtime_error).

  This function modifies the input matrix A and stores the results in
  the data.  The result can be obtained via the const method
  nmrLUDynamicData::PivotIndices().

  \param A A matrix of size MxN, either vctDynamicMatrix or vctDynamicMatrixRef.
  \param data A data object corresponding to the input matrix.

  \test nmrLUTest::TestDynamicDataColumnMajor
        nmrLUTest::TestDynamicUserOutputColumnMajor

 */
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrLU(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> & A,
                       nmrLUDynamicData & data)
    CISST_THROW(std::runtime_error)
{
    typename nmrLUDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER info;

    /* check that storage order is VCT_COL_MAJOR */
    if (!A.IsColMajor()) {
        cmnThrow(std::runtime_error("nmrLU: Input must use VCT_COL_MAJOR storage order."));
    }
    /* check sizes */
    if ((dataFriend.M() != A.rows()) || (dataFriend.N() != A.cols())) {
        cmnThrow(std::runtime_error("nmrLU: Size used for Allocate was different."));
    }
    /* check that the matrices are compact */
    if (! A.IsCompact()) {
        cmnThrow(std::runtime_error("nmrLU: Requires a compact matrix."));
    }

    CISSTNETLIB_INTEGER m = dataFriend.M();
    CISSTNETLIB_INTEGER n = dataFriend.N();
    CISSTNETLIB_INTEGER lda = (m > 1) ? m : 1;

    /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgetrf_(&m, &n,
                        A.Pointer(), &lda,
                        dataFriend.PivotIndices().Pointer(), &info);
#endif
#else // no major version
    dgetrf_(&m, &n,
            A.Pointer(), &lda,
            dataFriend.PivotIndices().Pointer(), &info);
#endif // CISSTNETLIB_VERSION
    return info;
}



/*! This function solves the LU problem for a dynamic matrix using the
  storage provided by the user for both the output (PivotIndices).

  Internally, a data is created using the storage provided by the
  user (see nmrLUDynamicData::SetRef).  While the data is
  being build, the consistency of the output is checked.  Then, the
  nmrLU(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param pivotIndices Vector created by the user to store the pivot indices.

  \test nmrLUTest::TestDynamicUserOutputColumnMajor
 */
template <class _matrixOwnerTypeA, class _vectorOwnerTypePivotIndices>
inline CISSTNETLIB_INTEGER nmrLU(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                       vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices)
{
    nmrLUDynamicData data(A, pivotIndices);
    return nmrLU(A, data);
}


#ifndef SWIG // don't have fixed size containers in Python

/*!  This function solves the LU problem for a fixed size matrix using
  the storage provided by the user for the output (PivotIndices).

  The sizes of the matrices must match at compilation time.  This is
  enforced by the template parameters and matching problems will lead
  to compilation errors.  Since there is no easy way to enforce the
  size of the vector PivotIndices with template parameters, a runtime
  check is performed.  The test uses CMN_ASSERT to determine what to
  do if the sizes don't match.  By default CMN_ASSERT calls \c abort()
  but it can be configured to be ignored or to throw an exception (see
  #CMN_ASSERT for details).

  This function modifies the input matrix A.  It stores the result in
  pivotIndices and A which now contains the elements of both L and U.
  The methods UpdateMatrixLU and UpdateMatrixP can ease the creation
  of more convenient matrices.

  \param A is a fixed size matrix of size MxN.
  \param pivotIndices Vector to store the pivot indices.

  \test nmrLUTest::TestFixedSizeDataColumnMajorMLeqN
        nmrLUTest::TestFixedSizeDataColumnMajorMGeqN
        nmrLUTest::TestFixedSizeUserOutputColumnMajorMLeqN
        nmrLUTest::TestFixedSizeUserOutputColumnMajorMGeqN
 */
template <vct::size_type _rows, vct::size_type _cols, vct::size_type _minmn>
inline CISSTNETLIB_INTEGER nmrLU(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> & A,
                       vctFixedSizeVector<CISSTNETLIB_INTEGER, _minmn> & pivotIndices)
{
#if CMN_ASSERT_IS_DEFINED
    const CISSTNETLIB_INTEGER minmn = static_cast<CISSTNETLIB_INTEGER>(nmrLUFixedSizeData<_rows, _cols>::MIN_MN);
#endif
    //Assert if requirement is equal to size provided!
    CMN_ASSERT(minmn == static_cast<CISSTNETLIB_INTEGER>(_minmn));

    CISSTNETLIB_INTEGER info;
    CISSTNETLIB_INTEGER lda = (_rows> 1) ? _rows : 1;
    CISSTNETLIB_INTEGER m = _rows;
    CISSTNETLIB_INTEGER n = _cols;

    /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgetrf_(&m, &n,
                        A.Pointer(), &lda,
                        pivotIndices.Pointer(), &info);
#endif
#else // no major version
    dgetrf_(&m, &n,
            A.Pointer(), &lda,
            pivotIndices.Pointer(), &info);
#endif // CISSTNETLIB_VERSION
    return info;
}


/*! This function solves the LU problem for a fixed size matrix using
  nmrLUFixedSizeData to allocate the memory required for the output:
  \code
  vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, 12, 7, VCT_COL_MAJOR> A;
  vctRandom(A, -10.0, 10.0);
  nmrLUFixedSizeData<12, 7> data;
  nmrLU(A, data);
  std::cout << "A: " << A << std::endl
            << "Pivot Indices: " << data.PivotIndices() << std::endl;
  \endcode

  This method calls nmrLU(A, pivotIndices).

  \param A A fixed size matrix of size MxN.
  \param data A data object.

  \test nmrLUTest::TestFixedSizeDataColumnMajorMLeqN
        nmrLUTest::TestFixedSizeDataColumnMajorMGeqN
        nmrLUTest::TestFixedSizeUserOutputColumnMajorMLeqN
        nmrLUTest::TestFixedSizeUserOutputColumnMajorMGeqN
 */
template <vct::size_type _rows, vct::size_type _cols>
inline CISSTNETLIB_INTEGER nmrLU(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> & A,
                       nmrLUFixedSizeData<_rows, _cols> & data)
{
    typename nmrLUFixedSizeData<_rows, _cols>::Friend dataFriend(data);
    return nmrLU(A, dataFriend.PivotIndices());
}
#endif // SWIG

//@}


#endif // _nmrLU_h
