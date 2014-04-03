/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2006-01-29

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrNNLS
*/


#ifndef _nmrNNLS_h
#define _nmrNNLS_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// Always include last
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  \brief Data for NNLS problem (Dynamic).

  --- description of problem here ---

  The sizes of these components must match exactly the input matrix A.
  To ease the use of the NNLS routine, the user can rely on the
  nmrNNLSDynamicData class to perform the required memory allocation.
  Furthermore, the underlying Fortran routine from LAPACK requires a
  workspace (aka a scratch space).  This workspace can also be
  allocated by the nmrNNLSDynamicData.

  Another good reason to use a "data" object is that the memory
  allocation can be performed once during an initialization phase
  while the function nmrNNLS can be called numerous times later on
  without any new dynamic memory allocation.  This is crucial for such
  things as real time tasks.

  The NNLS routine must be used with matrices allocated column first,
  i.e. using #VCT_COL_MAJOR.  Matrices and vectors must also be
  compact, i.e. use a contiguous block of memory.

  Any size or storage order mismatch will lead to an exception thrown
  (std::runtime_error).  Since we are using cmnThrow, it is possible
  to configure cisst (at compilation time) to abort the program
  instead of throwing an exception.

  Contrary to most "data" objects of cisstNumerical (see
  nmrSVDDynamicData or nmrLUDynamicData), the "data" object for NNLS
  can also be used to allocate memory for the input.  So far, we have
  referred to the input as the couple (A, b).  For efficiency reasons,
  the underlying Fortran routine actually bundles the matrix A and the
  vector b in one single matrix Ab.  The class nmrNNLSDynamicData
  allows to use either a single matrix Ab or the matrix A and the
  vector b separately.  If one prefers to use (A, b) instead of Ab,
  this is possible.  The programmer just has to be consistent,
  i.e. use the same input for all the nmrNNLSDynamicData methods and
  the function ::nmrNNLS.  The only drawback of this approach is that
  everytime the function ::nmrNNLS will be called, the content of A
  and b will be copied to a single matrix Ab compatible with the
  Fortran routine.

  The nmrNNLSDynamicData class allows 4 different configurations, for
  each of these, Ab can be replaced by A, b:

  - Allocate automatically everything, i.e. the output (X, W) as well
    as the workspace.  This can be performed using either the
    constructor from input matrix (i.e. nmrNNLSDynamicData(Ab)) or
    using the method Allocate(Ab).

  - Automatically allocate the workspace but rely on user allocated X
    and W.  The input matrix A is required to find the size of the
    problem.  This can be performed using either the constructor from
    Ab, X and W (i.e. nmrNNLSDynamicData(Ab, X, W)) or the method
    SetRefOutput(Ab, X, W).

  - Automatically allocate the output but rely on user allocated
    workspaces.  This can be useful if the user decides to create a
    large workspace for multiple numerical routines.  The user will
    have to make sure his program is thread-safe.  This can be
    performed using either the constructor from Ab and the workspaces
    (i.e. nmrNNLSDynamicData(Ab, workspace, workspaceInt)) or the
    method SetRefWorkspace(Ab, workspace, workspaceInt).  Please note
    that the matrix Ab is required to compute the dimension of the
    problem since the only requirement on the workspaces is that they
    must be large enough.

  - Don't allocate anything.  The user has to provide the containers
    he wants to use for X, W and the workspaces.  In this case, the
    "data" is used mostly to check that all the containers are valid
    in terms of size and storage order.  This can be performed using
    either the constructor from X, W and workspaces
    (i.e. nmrNNLSDynamicData(Ab, X, W, workspace, workspaceInt)) or
    the method SetRef(Ab, X, W, workspace, workspaceInt).

  \sa nmrNNLS
 */
class nmrNNLSDynamicData {

public:
    /*! Type used for sizes within nmrNNLSDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #CISSTNETLIB_INTEGER. */
    typedef unsigned int size_type;

    typedef vctFixedSizeVector<size_type, 2> MatrixSizeType;

protected:
    /*! Memory allocated for Workspace vectors if needed. */
    //@{
    vctDynamicVector<CISSTNETLIB_DOUBLE> WorkspaceMemory;
    vctDynamicVector<CISSTNETLIB_INTEGER> WorkspaceIntMemory;
    //@}

    /*! Memory allocated for vector X and W if needed.  This method
       allocates a single block of memory for these 2 containers; m
       elements of X followed by n elements of W.
      */
    vctDynamicVector<CISSTNETLIB_DOUBLE> OutputMemory;

    /*! Memory allocated for the input.  This is used only if the user
      provides separate A and b and the class needs to copy them in
      the single matrix Ab. */
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> AbMemory;

    /*! References to workspaces or return containers, these point
      either to user allocated memory or our memory chunks if needed.
     */
    //@{
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> AbReference;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> XReference;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> WReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> WorkspaceReference;
    vctDynamicVectorRef<CISSTNETLIB_INTEGER> WorkspaceIntReference;
    //@}

    /*! Store M, N which are needed to check if input matrix passed to
       solve method matches the allocated size. */
    //@{
    size_type MMember;
    size_type NMember;
    //@}

    /*! Private method to set the data members MMember and NMember.
      This method must be called before AllocateOutputWorkspace,
      ThrowUnlessOutputSizeIsCorrect or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type m, size_type n)
    {
        MMember = m;
        NMember = n;
    }

    /*! Private method to allocate memory for the output and the
      workspaces if needed.  This method assumes that the dimension m
      and n are already set.  It is important to use this method in
      all the methods provided in the user API, even if all the memory
      is provided by the user since this method will ensure that the
      "data" (nmrNNLSDynamicData) does not keep any memory allocated.
      This is for the case where a single "data" is used first to
      allocate everything and, later on, used with user allocated
      memory (for either the workspace or the output).

      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutputWorkspaces(bool allocateOutput, bool allocateWorkspaces)
    {
        // allocate output
        if (allocateOutput) {
            this->OutputMemory.SetSize(2 * NMember);
            this->XReference.SetRef(NMember, 1, this->OutputMemory.Pointer(0));
            this->WReference.SetRef(NMember, 1, this->OutputMemory.Pointer(NMember));
        } else {
            this->OutputMemory.SetSize(0);
        }
        // allocate workspaces
        if (allocateWorkspaces) {
            this->WorkspaceMemory.SetSize(MMember);
            this->WorkspaceReference.SetRef(this->WorkspaceMemory);
            this->WorkspaceIntMemory.SetSize(NMember);
            this->WorkspaceIntReference.SetRef(this->WorkspaceIntMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
            this->WorkspaceIntMemory.SetSize(0);
        }
    }


    /*!  Verifies that the user provided references for the output
      match the size of the "data" as set by SetDimension.  This
      method also checks that the storage orders are consistent across
      the provided matrices and that all containers are compact.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypeX,
              class _vectorOwnerTypeW>
    inline void ThrowUnlessOutputSizeIsCorrect(const vctDynamicVectorBase<_matrixOwnerTypeX, CISSTNETLIB_DOUBLE> & inX,
                                               const vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & inW) const
        throw(std::runtime_error)
    {
         // check sizes and compact-ness
        if (inX.size != NMember) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Size of vector X is incorrect."));
        }
        if (!inX.IsCompact()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Vector X must be compact."));
        }
        if (inW.size != NMember) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Size of vector W is incorrect."));
        }
        if (!inW.IsCompact()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Vector W must be compact."));
        }
    }


    /*! Verifies that the user provided references for the workspaces
      match (or is greated than) the size of the "data" as set by
      SetDimension.  This method also checks that the workspaces are
      compact.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypeWorkspace,
              class _vectorOwnerTypeWorkspaceInt>
    inline void
    ThrowUnlessWorkspacesSizeIsCorrect(const vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace,
                                       const vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & inWorkspaceInt) const
        throw(std::runtime_error)
    {
        if (MMember > inWorkspace.size()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Workspace is too small."));
        }
        if (!inWorkspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: Workspace must be compact."));
        }
        if (NMember > inWorkspaceInt.size()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: WorkspaceInt is too small."));
        }
        if (!inWorkspaceInt.IsCompact()) {
            cmnThrow(std::runtime_error("nmrNNLSDynamicData: WorkspaceInt must be compact."));
        }
    }


public:

    /*! Helper method to determine the minimum working space required
      by NNLS routine.
    */
    //@{
    template <class _matrixOwnerTypeAb>
    static inline size_type WorkspaceSize(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab)
    {
        return Ab.rows();
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB>
    static inline size_type WorkspaceSize(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                                          const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b)
    {
        return A.rows();
    }
    //@}

    /*! Helper method to determine the minimum integer working space
      required by NNLS routine.
    */
    //@{
    template <class _matrixOwnerTypeAb>
    static inline size_type WorkspaceIntSize(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab)
    {
        // Ab has one too many columns
        return (Ab.cols() - 1);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB>
    static inline size_type WorkspaceIntSize(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                                             const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b)
    {
        return A.cols();
    }
    //@}


#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated NNLS function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrNNLSDynamicData & Data;
    public:
        Friend(nmrNNLSDynamicData & data): Data(data) {
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & X(void) {
            return Data.XReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & W(void) {
            return Data.WReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & Workspace(void) {
            return Data.WorkspaceReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_INTEGER> & WorkspaceInt(void) {
            return Data.WorkspaceIntReference;
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

      \sa nmrNNLSDynamicData::Allocate
      nmrNNLSDynamicData::SetRefOutput
      nmrNNLSDynamicData::SetRefWorkspace
      nmrNNLSDynamicData::SetRef
    */
    nmrNNLSDynamicData():
        MMember(static_cast<size_type>(0)),
        NMember(static_cast<size_type>(0)),
    {
        AllocateOutputWorkspaces(false, false);
        AbMemory.SetSize(0);
    }


    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for the output matrices and vectors as well as
      workspaces. This should be used when the user doesn't care much
      about where the output should be stored and doesn't need to
      share the workspace between different algorithms.

      \sa nmrNNLSDynamicData::Allocate
    */
    //@{
    template <class _matrixOwnerTypeAb>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab)
    {
        this->Allocate(Ab);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                       const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b)
    {
        this->Allocate(A, b);
    }
    //@}

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for the output matrices and vectors only.  This
      constructor should be used when the user cares wants to avoid
      allocating different workspaces for different numerical
      routines.  Please note that since multiple routines can share
      the workspace, these routines must be called in a thread safe
      manner.

      \sa nmrNNLSDynamicData::SetRefWorkspace
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetRefWorkspace(A, b, workspace, workspaceInt);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                       const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetRefWorkspace(A, b, workspace, workspaceInt);
    }
    //@}


    /*! Constructor where the user provides the vectors X and W as
      well as the workspaces.  The data object now acts as a composite
      container to hold, pass and manipulate a convenient storage for
      NNLS algorithm. Checks are made on the validity of the input and
      its consitency in terms of size.  Please note that since the
      workspace and the input are now created by the user, special
      attention must be given to thread safety issues.

      \sa nmrNNLSDynamicData::SetRef
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                       vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                       vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetRef(Ab, X, W, workspace, workspaceInt);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                       const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                       vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                       vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                       vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetRef(Ab, X, W, workspace, workspaceInt);
    }
    //@}

    /*! Constructor where the user provides the vectors X and W.  The
      workspaces will be allocated and managed by the "data".  This
      constructor should be used when the user already has a storage
      for the data but doesn't care much about the workspace.

      \sa nmrNNLSDynamicData::SetRefOutput
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                       vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                       vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W)
    {
        this->SetRefOutput(Ab, X, W);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW>
    nmrNNLSDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                       const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                       vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                       vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W)
    {
        this->SetRefOutput(A, b, X, W);
    }
    //@}

    /*! This method allocates memory for the output vectors as well as
      the workspace.  The input matrix is used only to determine the
      size of the problem.

      This method should be called before the nmrNNLSDynamicData
      object is passed on to nmrNNLS function.
    */
    //@{
    template <class _matrixOwnerTypeAb>
    inline void Allocate(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab)
    {
        this->SetDimension(Ab.rows(), (Ab.cols() - 1));
        this->AllocateOutputWorkspace(true, true);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB>
    inline void Allocate(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                         const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b)
    {
        this->SetDimension(A.rows(), A.cols());
        this->AllocateOutputWorkspaces(true, true);
        this->AbMemory.SetSize(MMember, NMember + 1, VCT_COL_MAJOR);
    }
    //@}


    /*! This method allocates the memory for the output vectors and
      uses the memory provided by user for workspaces.  The input
      matrix A is used to determine the size of the problem.

      This method verifies that the workspaces provided by the user is
      large enough and is compact.
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    inline void SetRefWorkspaces(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                                 vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                                 vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetDimension(Ab.rows(), (Ab.cols() - 1));

        // allocate output and set references
        this->AllocateOutputWorkspaces(true, false);

        // set reference on user provided workspace
        this->ThrowUnlessWorkspacesSizeIsCorrect(workspace, workspaceInt);
        this->WorkspaceReference.SetRef(workspace);
        this->WorkspaceIntReference.SetRef(workspaceInt);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    inline void SetRefWorkspaces(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                                 const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                                 vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                                 vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
    {
        this->SetDimension(A.rows(), A.cols());

        // allocate output and set references
        this->AllocateOutputWorkspaces(true, false);

        // set reference on user provided workspace
        this->ThrowUnlessWorkspacesSizeIsCorrect(workspace, workspaceInt);
        this->WorkspaceReference.SetRef(workspace);
        this->WorkspaceIntReference.SetRef(workspaceInt);

        this->AbMemory.SetSize(MMember, NMember + 1, VCT_COL_MAJOR);
    }
    //@}


    /*! This method doesn't allocate any memory as it relies on user
      provided vectors for the output as well as the
      workspaces.

      The data object now acts as a composite container to hold,
      pass and manipulate a convenient storage for NNLS algorithm.  The
      method tests that all the containers provided by the user have
      the correct size and are compact.
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    void SetRef(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
        throw(std::runtime_error)
    {
        this->SetDimension(Ab.rows(), (Ab.cols() - 1));
        this->AllocateOutputWorkspace(false, false);
        this->ThrowUnlessOutputSizeIsCorrect(X, W);
        this->ThrowUnlessWorkspacesSizeIsCorrect(workspace, workspaceInt);

        this->XReference.SetRef(X);
        this->WReference.SetRef(W);
        this->WorkspaceReference.SetRef(workspace);
        this->WorkspaceReference.SetRef(workspaceInt);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW,
              class _vectorOwnerTypeWorkspace, class _vectorOwnerTypeWorkspaceInt>
    void SetRef(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                const vctDynamicVectorBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspaceInt, CISSTNETLIB_INTEGER> & workspaceInt)
        throw(std::runtime_error)
    {
        this->SetDimension(A.rows(), A.cols());
        this->AllocateOutputWorkspace(false, false);
        this->ThrowUnlessOutputSizeIsCorrect(X, W);
        this->ThrowUnlessWorkspacesSizeIsCorrect(workspace, workspaceInt);

        this->XReference.SetRef(X);
        this->WReference.SetRef(W);
        this->WorkspaceReference.SetRef(workspace);
        this->WorkspaceReference.SetRef(workspaceInt);

        this->AbMemory.SetSize(MMember, NMember + 1, VCT_COL_MAJOR);
    }
    //@}

    /*! This method allocates the memory for the workspaces.  The
      output memory is provided by the user.  The method computes the
      size of the problem based on the user provided output and
      verifies that the output components (X and W) are consistent
      with respect to their size.
    */
    //@{
    template <class _matrixOwnerTypeAb,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW>
    void SetRefOutput(const vctDynamicMatrixBase<_matrixOwnerTypeAb, CISSTNETLIB_DOUBLE> & Ab,
                      vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                      vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W)
        throw(std::runtime_error)
    {
        this->SetDimension(Ab.rows(), (Ab.cols() - 1));
        this->ThrowUnlessOutputSizeIsCorrect(X, W);

        this->XReference.SetRef(X);
        this->WReference.SetRef(W);

        AllocateOutputWorkspaces(false, true);
    }
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeB,
              class _vectorOwnerTypeX, class _vectorOwnerTypeW>
    void SetRefOutput(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                      const vctDynamicMatrixBase<_vectorOwnerTypeB, CISSTNETLIB_DOUBLE> & b,
                      vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> & X,
                      vctDynamicVectorBase<_vectorOwnerTypeW, CISSTNETLIB_DOUBLE> & W)
        throw(std::runtime_error)
    {
        this->SetDimension(A.rows(), A.cols());
        this->ThrowUnlessOutputSizeIsCorrect(X, W);

        this->XReference.SetRef(X);
        this->WReference.SetRef(W);

        AllocateOutputWorkspaces(false, true);

        this->AbMemory.SetSize(MMember, NMember + 1, VCT_COL_MAJOR);
    }
    //@}

    /*! Const reference to the result vector X.  This method must be
      called after the data has been computed by the nmrNNLS
      function. */
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & X(void) const {
        return XReference;
    }

    /*! Const reference to the result vector W (dual).  This method
      must be called after the data has been computed by the nmrNNLS
      function. */
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & W(void) const {
        return WReference;
    }
};




/*!
  \name Algorithm NNLS:


  --- update this doc ---

  These functions are wrappers around the LAPACK routine dgesvd,
  therefore they share some features with the LAPACK routine:
  <ol>
  <li>They return \f$ V^{T} \f$, not \f$ V \f$.
  <li>On exit, the content of A is altered.
  <li>The vectors and matrices must be compact, i.e. use a contiguous
  block of memory.
  </ol>

  The ::nmrNNLS functions add the following features:
  <ol>
  <li>A simplified interface to the cisstVector dynamic matrices.
  <li>The possibility to use any storage order, i.e. #VCT_ROW_MAJOR or
  #VCT_COL_MAJOR.  In the specific case of NNLS, the storage order can
  be treated as a transpose and the properties of the decomposition
  are preserved (::nmrNNLS will swap U and Vt pointers).  This is
  performed at no extra cost, i.e. no copy back and forth of the
  elements nor extra memory allocation.
  <li>Input validation checks are performed, i.e. an
  std::runtime_error exception will be thrown if the sizes or storage
  order don't match or if the containers are not compact.
  <li>Helper classes to allocate memory for the output and workspace:
  nmrNNLSFixedSizeData and nmrNNLSDynamicData.
  </ol>

  There are different ways to call this function to compute the NNLS of
  the matrix A.  These correspond to different overloaded ::nmrNNLS
  functions:
  <ol>
  <li>Using a preallocated data object.

  The user creates the input matrix A:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(12, 24 , VCT_ROW_MAJOR); // 12 x 24 matrix
  vctRandom(A, -10.0, 10.0);
  \endcode
  The user allocates a data object which could be of
  type nmrNNLSFixedSizeData or nmrNNLSDynamicData.
  corresponding to fixed size or dynamic matrix A:
  \code
  nmrNNLSDynamicData data(A);
  \endcode
  Call the nmrNNLS function:
  \code
  nmrNNLS(A, data);
  \endcode
  The content of input matrix A is modified by this routine.  The
  matrices U, Vt and vector S are available through the following
  methods
  \code
  std::cout << "U: " << data.U() << std::endl
            << "S: " << data.S() << std::endl
            << "V: " << data.Vt().Transpose() << std::endl;
  \endcode

  <li>The user provides the matrices U, Vt, and vector S.

  The User allocates memory for these matrices and vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> U(5, 5);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> Vt(4, 4);
  vctDynamicVector<CISSTNETLIB_DOUBLE> S(4);
  \endcode
  Call the NNLS routine:
  \code
  nmrNNLS(A, U, S, Vt);
  \endcode
  The NNLS function verifies that the size of the data objects
  matches the input, and allocates workspace memory, which is
  deallocated when the function ends.

  <li>The user provides the matrices U, Vt, and vector S along with
  workspace required by NNLS routine of LAPACK.

  Create matrices and vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> U(5, 5);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> Vt(4, 4);
  vctDynamicVector<CISSTNETLIB_DOUBLE> S(4);
  \endcode
  One also needs to allocate memory the for workspace. This approach is
  particularly useful when the user is using more than one numerical
  method from the library and is willing or need to share the workspace
  between them. In such as case, the user can allocate a block of memory
  greater than the minimum required by different methods.  To help the
  user determine the minimum workspace required the library provides
  the helper function nmrNNLSDynamicData::WorkspaceSize().
  \code
  vctDynamicVector<CISSTNETLIB_DOUBLE> workspace(nmrNNLSDynamicData::WorkspaceSize(A));
  \endcode
  Call the NNLS function:
  \code
  nmrNNLS(A, U, S, Vt, workspace);
  \endcode
  </ol>

  \note The NNLS functions make use of LAPACK routines.  To activate
  this code, set the CISST_HAS_CISSTNETLIB flag to ON during the
  configuration of cisst with CMake.

  \note The general rule for numerical functions which depend on
  LAPACK is that column major matrices should be used everywhere, and
  that all matrices should be compact.  In this case, both row major
  and column major are allowed but they must not be mixed in a
  data object.
 */
//@{


/*! This function solves the NNLS problem for a dynamic matrix using an
  nmrNNLSDynamicData.

  This function checks for valid input (size, storage order and
  compact) and calls the LAPACK function.  If the input doesn't match
  the data, an exception is thrown (\c std::runtime_error).

  This function modifies the input matrix A and stores the results in
  the data.  Each component of the result can be obtained via the
  const methods nmrNNLSDynamicData::U(), nmrNNLSDynamicData::S()
  and nmrNNLSDynamicData::Vt().

  \param A A matrix of size MxN, either vctDynamicMatrix or vctDynamicMatrixRef.
  \param data A data object corresponding to the input matrix.

  \test nmrNNLSTest::TestDynamicColumnMajor
        nmrNNLSTest::TestDynamicRowMajor
        nmrNNLSTest::TestDynamicColumnMajorUserAlloc
        nmrNNLSTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrNNLS(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> & A,
                                   nmrNNLSDynamicData & data)
    throw (std::runtime_error)
{
    typename nmrNNLSDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER Info;
    char m_Jobu = 'A';
    char m_Jobvt = 'A';
    CISSTNETLIB_INTEGER m_Lwork = static_cast<CISSTNETLIB_INTEGER>(nmrNNLSDynamicData::WorkspaceSize(dataFriend.M(),
                                                                                                     dataFriend.N()));
    /* check that storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrNNLS: Storage order used for Allocate was different"));
    }
    /* check sizes */
    if ((dataFriend.M() != A.rows()) || (dataFriend.N() != A.cols())) {
        cmnThrow(std::runtime_error("nmrNNLS: Size used for Allocate was different"));
    }
    /* check that the matrices are compact */
    if (! A.IsCompact()) {
        cmnThrow(std::runtime_error("nmrNNLS: Requires a compact matrix"));
    }

    /* Based on storage order, permute U and Vt as well as dimension */
    CISSTNETLIB_DOUBLE *UPtr, *VtPtr;
    CISSTNETLIB_INTEGER m_Lda, m_Ldu, m_Ldvt;

    if (A.IsColMajor()) {
        m_Lda = (1 > dataFriend.M()) ? 1 : dataFriend.M();
        m_Ldu = dataFriend.M();
        m_Ldvt = dataFriend.N();
        UPtr = dataFriend.U().Pointer();
        VtPtr = dataFriend.Vt().Pointer();
    } else if (A.IsRowMajor()) {
        m_Lda = (1 > dataFriend.N()) ? 1 : dataFriend.N();
        m_Ldu = dataFriend.N();
        m_Ldvt = dataFriend.M();
        UPtr = dataFriend.Vt().Pointer();
        VtPtr = dataFriend.U().Pointer();
    }

   // for versions based on gfortran/lapack, CISSTNETLIB_VERSION is
    // defined
#if defined(CISSTNETLIB_VERSION)
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgesvd_(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
                        A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
                        UPtr, &m_Ldu,
                        VtPtr, &m_Ldvt,
                        dataFriend.Workspace().Pointer(), &m_Lwork, &Info);
#endif
#else // no major version
    dgesvd_(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
            A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
            UPtr, &m_Ldu,
            VtPtr, &m_Ldvt,
            dataFriend.Workspace().Pointer(), &m_Lwork, &Info);
#endif // CISSTNETLIB_VERSION
#else
    ftnlen jobu_len = (ftnlen)1, jobvt_len = (ftnlen)1;
    la_dzlapack_MP_sgesvd_nat(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
                              A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
                              UPtr, &m_Ldu,
                              VtPtr, &m_Ldvt,
                              dataFriend.Workspace().Pointer(), &m_Lwork, &Info,
                              jobu_len, jobvt_len);
#endif

    return Info;
}

/*! This function solves the NNLS problem for a dynamic matrix using
  the storage provided by the user for both the output (U, S, Vt) and
  the workspace.

  Internally, a data is created using the storage provided by the
  user (see nmrNNLSDynamicData::SetRef).  While the data is
  being build, the consistency of the output and workspace is checked.
  Then, the nmrNNLS(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for NNLS
  \param Workspace The workspace for LAPACK.

  \test nmrNNLSTest::TestDynamicColumnMajorUserAlloc
        nmrNNLSTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypeU,
          class _vectorOwnerTypeS, class _matrixOwnerTypeVt,
          class _vectorOwnerTypeWorkspace>
inline CISSTNETLIB_INTEGER nmrNNLS(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                        vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U,
                        vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & S,
                        vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & Vt,
                        vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & Workspace)
{
    nmrNNLSDynamicData svdData(U, S, Vt, Workspace);
    CISSTNETLIB_INTEGER ret_value = nmrNNLS(A, svdData);
    return ret_value;
}

/*! This function solves the NNLS problem for a dynamic matrix using
  the storage provided by the user for the output (U, S, Vt).  A
  workspace will be dynamically allocated.

  Internally, a data is created using the storage provided by the
  user (see nmrNNLSDynamicData::SetRefOutput).  While the data
  is being build, the consistency of the output is checked.  Then, the
  nmrNNLS(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for NNLS

  \test nmrNNLSTest::TestDynamicColumnMajorUserAlloc
        nmrNNLSTest::TestDynamicRowMajorUserAlloc

  \warning Again, this method will dynamically allocate a workspace at
  each call.  This is somewhat inefficient if the method is to be
  called many times.  For a real-time task, the dynamic allocation
  might even break your application.
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypeU,
          class _vectorOwnerTypeS, class _matrixOwnerTypeVt>
inline CISSTNETLIB_INTEGER nmrNNLS(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                        vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U,
                        vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & S,
                        vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & Vt)
{
    nmrNNLSDynamicData svdData(U, S, Vt);
    CISSTNETLIB_INTEGER ret_value = nmrNNLS(A, svdData);
    return ret_value;
}


//@}


#endif

