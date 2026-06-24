Introduction
============

These examples provide a quick introduction to the features of cisstNumerical. The code can be found in the git repository under cisstNumerical/examples/tutorial. To compile your own code, remember to include ``cisstNumerical.h``.

cisstNumerical contains native functions as well as wrappers for existing numerical routines. As many algorithms can be found in FORTRAN, a significant part of cisstNumerical interfaces with FORTRAN routines. This has numerous consequences listed in section 5.

An illustrative example: SVD
============================

The Singular Value Decomposition is a common algorithm and the cisst implementation illustrates many features of the cisstNumerical FORTRAN wrappers. The goal of SVD is to find the decomposition of a matrix A such as ``A = U * Sigma * V`` where both U and V are orthonormal and S is a diagonal matrix composed of singular values.

Most of the FORTRAN routines we are using will not allocate any memory nor check that the parameters are valid (see also section 5). Our wrappers not only check the size of the parameters to verify that enough memory has been allocated but they also provide some flexible mechanisms to allocate the required memory.

For the SVD, the underlying FORTRAN routine requires an input matrix A, two matrices to store U and Vt, a vector for the singular values S and a workspace for temporary variables (a.k.a. scratch space). From now on, we will call the two matrices U and Vt and the vector S the output.

Finally, since cisstVector supports both fixed size and dynamic vectors and matrices, cisstNumerical provides different wrappers and classes for each type of memory allocation. The examples we are providing illustrate different possible configurations, i.e. who allocates memory for what and how.

+---------+------------+-------------------------------+--------------------------------+
| Example | Type       | Output allocation             | Workspace allocation           |
+=========+============+===============================+================================+
| 2.1     | Dynamic    | User, manually                |                                |
+---------+------------+-------------------------------+--------------------------------+
| 2.2     | Dynamic    | User, manually                | nmrSVD function                |
+---------+------------+-------------------------------+--------------------------------+
| 2.3     | Dynamic    | User, manually                | User with method WorkspaceSize |
+---------+------------+-------------------------------+--------------------------------+
| 2.4     | Dynamic    | User with nmrSVDDynamicData   |                                |
+---------+------------+-------------------------------+--------------------------------+
| 2.6     | Fixed size | User, manually                |                                |
+---------+------------+-------------------------------+--------------------------------+
| 2.7     | Fixed size | User with nmrSVDFixedSizeData |                                |
+---------+------------+-------------------------------+--------------------------------+

2.1 Using nmrSVD with user allocated containers

This example shows how to use the function ``nmrSVD`` with a dynamic matrix.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDUserOutputWorkspace.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In this example, we have used a workspace 10 times bigger than the initial matrix which is large enough. Since the size of the workspace can be determined automatically, cisstNumerical also provides an overloaded version of ``nmrSVD`` which doesn't require a workspace.

Using nmrSVD without specifying a workspace
-------------------------------------------

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDImplicitWorkspace.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

This is easier to use, but one has to remember that a workspace is created dynamically by ``nmrSVD``, i.e. every time the function is called some memory is allocated and released.

This behavior might not suit everyone, therefore cisstNumerical provides a couple of methods to ease the allocation of the workspace and the output matrices and vectors. All these methods are declared within the scope of a class called "data". For SVD, we have two different classes available, ``nmrSVDDynamicData`` and ``nmrSVDFixedSizeData``.

Using ``nmrSVDDynamicData::WorkspaceSize``
------------------------------------------

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDWorkspaceSize.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

This method simplifies the allocation of the workspace but doesn't solve another problem that we have ignored so far: If the input matrix is not square, the size of the different output containers are a bit trickier to determine, i.e. if the input matrix is m x n then U should be m x m, Vt n x n and the vector S should be ``min(m, n)`` long. This is not awfully difficult but still requires some extra attention from the caller. To facilitate the user's work, it is possible the use the class ``nmrSVDDynamicData`` to allocate not only the workspace but the output as well.

Using ``nmrSVDDynamicData`` to allocate everything
--------------------------------------------------

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDDynamicData.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In this example we have declared a data object based on the input matrix A. The constructor of ``nmrSVDDynamicData`` allocates the required memory for all the output containers (U, Vt and S) as well as the workspace. Another overloaded version ``nmrSVD`` takes the matrix A and the object ``svdData`` to perform the singular value decomposition.

This data object performs a one time memory allocation and can be used multiple times (in a loop for example) without performing any new memory allocation.

Using ``nmrSVDDynamicData::UpdateMatrixS``
------------------------------------------

Once the decomposition has been performed, ``nmrSVD`` stores all the singular values in decreasing order in a vector. This might be convenient for some but one might need a diagonal matrix instead. To update this matrix, the class ``nmrSVDDynamicData`` provides the method ``UpdateMatrixS``.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDUpdateMatrixS.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Note that the method ``UpdateMatrixS`` is a static method which can be called even if no data object has been created. Since the method is static, it needs the input matrix A to determine the correct size of the matrix S.

Using fixed size matrices without a data object
-----------------------------------------------

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDFixedSize.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

The example is very similar to the first one, i.e. the user has to create all the containers with the correct sizes before calling ``nmrSVD``.

As for the dynamic matrices, ``nmrSVD`` has an overloaded version which doesn't require a user allocated workspace. For the fixed size matrices, the function ``nmrSVD`` will create a workspace on the stack if none was provided (not a dynamic memory allocation as seen for dynamic matrices!).

Using fixed size matrices with ``nmrSVDFixedSizeData``
------------------------------------------------------

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleSVDFixedSizeData.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

The interface of the ``nmrSVDFixedSizeData`` is pretty much the same as ``nmrSVDDynamicData`` except that the size and storage order are now specified using template parameters. To simplify our example, we introduced a type for DataType. This approach is strongly recommended whenever one uses the cisst fixed size vectors and matrices.

Functions with data object
==========================

Besides the function ``nmrSVD``, cisstNumerical includes more numerical functions which can be used with either a data object or some vectors and matrices provided by the caller.

FORTRAN based functions
-----------------------

The cisstNumerical FORTRAN wrappers are all written using the approach used for ``nmrSVD`` and share the different properties listed for ``nmrSVD``.

``nmrInverse``
~~~~~~~~~~~~~~

This function computes the inverse of a matrix using an LU decomposition. It can be used for dynamic and fixed size matrices of any storage order. Nevertheless, for fixed size matrices of size 2, 3 or 4, we recommend to use ``nmrGaussJordanInverse``.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleInverse.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In this example, we used the overloaded version of ``nmrInverse`` which doesn't require a data object. This is possible since the data object doesn't provide any useful information or result.

As for most wrappers, using the function ``nmrInverse`` without a data object is not optimal if the function is going to be called multiple times. To optimize the memory allocation, one should use ``nmrInverseFixedSizeData`` or ``nmrInverseDynamicData``.

``nmrLU``
~~~~~~~~~

The goal of LU is to find the factorization of a matrix A such as A = L x U where U is an upper matrix and V is a lower matrix.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleLUDynamicData.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

It is important to notice that in this example we explicitly created the input using ``VCT_COL_MAJOR``. As it is, ``nmrLU`` doesn't support the row first storage order.

Besides this constraint, the LU decomposition routine provided by LAPACK stores the result in one single matrix, replacing the input. This is perfectly good for most applications but one can also use the helper methods of ``nmrLUDynamicData`` to determine the size and compute the matrices P, L and U.

``nmrPInverse``
~~~~~~~~~~~~~~~

This function actually relies on nmrSVD. The corresponding data object nmrPInverseDynamicData and nmrPInverseFixedSizeData allocate a workspace large enough for nmrSVD.

Native cisst functions
----------------------

The cisst native functions are more flexible than the FORTRAN wrappers mostly because the restrictions regarding the storage order and the compactness are lifted. The elements might also be different, i.e. one can use single precision floating point numbers if this makes any sense for his/her application.

``nmrIsOrthonormal``
~~~~~~~~~~~~~~~~~~~~

In this example, we are using ``nmrSVD`` to create a couple of orthonormal matrices.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleIsOrthonormal.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

This example demonstrates two different ways to use the function ``nmrIsOrthonormal``, one with a user defined workspace and one with no workspace at all (i.e. the function will allocate and free memory on the fly).

Please note in this example how we created a single workspace used by different routines. This is very convenient to avoid any unnecessary memory allocation but one must make sure that this workspace is not being used by two different threads.

It is also possible to create a data object for this problem (see ``nmrIsOrthonormalDynamicData`` and ``nmrIsOrthonormalFixedSizeData``).

Others
======

``nmrGaussJordanInverse``
-------------------------

The Gauss Jordan inverse methods are implemented for fixed size matrices 2 x 2 , 3 x 3 and 4 x 4. These functions, compiled in release mode, are faster than their FORTRAN counterparts.

.. literalinclude:: ../../../cisstNumerical/examples/tutorial/ExampleGaussJordanInverse.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Note that since these functions are fully implemented using the cisst package, any storage order or stride can be used (i.e. the matrices don't need to be compact).

FORTRAN specifics
=================

Compilation
-----------

Most of the FORTRAN routines we are using come from the on-line code repository netlib.org. There is no standard binary distribution of these routines therefore we decided to provide a binary version of these routines (library and header files). We have two different versions for historical reasons:

-  CNetlib: This is the oldest version, now deprecated. The main default of this version was that it is not thread safe.

-  cisstNetlib: This version is based on LAPACK routines and is thread safe. We strongly recommend to use this version.

The cisstNumerical API is the same (i.e. your code will be the same) for both binary distributions. Remember that you need to configure your build with the CMake option ``CISST_HAS_CISSTNETLIB`` turned ``ON``.

For more details and to download these libraries, see http://github.com/jhu-cisst/cisstNetlib.

Common properties
-----------------

All our wrappers for FORTRAN routines share the following properties:

The default storage order for matrices is column first in FORTRAN while it is row first in C/C++. Since cisstVector supports both formats, the user has to remember to create his matrices column first (using ``VCT_COL_MAJOR``). This is the default but there are some exceptions. For example, ``nmrSVD`` can be used with any storage order. In the case, cisstNumerical uses the fact that changing the storage order is similar to a transpose. For SVD, the problem :math:`A^t = (U * \Sigma *
V)^t = V^t * \Sigma^t * U^t` is basically the same as :math:`A = U *
\Sigma * T`. Whenever this is possible, classes and functions of cisstNumerical are implemented to support both storage orders (either row or column major).

Most FORTRAN routines were not written with the concept of stride in mind. This means that all matrices and vectors which are finally used by a FORTRAN routine must be compact (i.e. use a contiguous block of memory).

Most LAPACK routines, will modify the input to avoid unnecessary memory allocation. Since cisstNumerical has been designed to avoid implicit memory allocation and copies as well, it is the caller's responsibility to create a copy of the input for future use.

These functions can only operate on matrices and vectors of doubles. This is because this function is actually a wrapper to a LAPACK routine which requires double precision floating point numbers. For the integers (e.g. vector of pivot indices), FORTRAN uses the equivalent of a C/C++ long int. To enforce this and remind the caller of this subtlety, the cisstNumerical interface defines and uses F_INTEGER. If the matrices or vectors provided by the user are not correct (size, storage order, compact), an exception will occur (``std::runtime_error``). Since these exceptions are logged, the user might want to look at the file cisstLog.txt if his/her application quits unexpectedly.
