Concepts
========

This document is not complete yet. It should ultimately include most of the information found in our previous documentation:

-  [http://unittest.lcsr.jhu.edu/cisst/downloads/cisst/current/doc/latex/vector-user-guide.html cisstVector User Guide (html)], also available as [http://unittest.lcsr.jhu.edu/cisst/downloads/cisst/current/doc/latex/vector-user-guide.pdf pdf]

cisstVector is a collection of classes used for linear algebra. This includes vectors, matrices, multi-dimensional arrays and some classes to represent 2D and 3D transformations. For both vectors and matrices, cisstVector supports fixed size containers (i.e. stack allocated and using a size known at compilation time) and dynamic size containers (which can be resized at any time using runtime memory allocation). Fixed size containers are ideal for small objects (tens of elements). If you are using large datasets, dynamic size containers are strongly recommended. Multi-dimensional arrays use dynamic memory allocation but their dimension must be set at compilation time.

All containers in cisstVector have been designed to be flexible while remaining efficient. To do so the library strongly relies on C++ templates. The most obvious template parametrization is the type of element contained. For example, the code used for a dynamic matrix of ``float``\ s is not much different from the code for a matrix of ``double``\ s.

.. code:: c++

   // declare a matrix of floats
   vctDynamicMatrix<float> matrixOfFloats;
   // declare a matrix of doubles
   vctDynamicMatrix<double> matrixOfDoubles;

The second template parameter is the size for fixed size containers:

.. code:: c++

   // declare a fixed size vector of 4 floats
   vctFixedSizeVector<float, 4> vector4OfFloats;
   // declare a fixed size vector of 3 ints
   vctFixedSizeVector<int, 3> vector3OfInts;

cisstVector also relies on a template parameter to define the dimension of multi-dimensional arrays:

.. code:: c++

   // a 3D array of doubles
   vctDynamicNArray<double, 3> array3DOfDoubles;

The cisstVector containers also use templates to parametrize how memory is managed. This is used extensively to create "references" on a container or part of a container such as a column of a matrix, a slice of a multi-dimensional array, ...

Main types and "shortcuts" (typedefs)
-------------------------------------

Most cisstVector classes rely on templates. Since C++ template declaration can be fairly cumbersome, cisstVector provides some "shortcuts" in the form of typedefs for the most commonly used types. For example, one can use ``vctInt4`` to declare a vector of 4 integers instead of ``vctFixedSizeVector<int 4>``. Since most computations are performed using ``double``\ s, cisstVector provides an even shorter notation, dropping the ``Double`` part of the type name. For example, ``vctFixedSizeVector<double, 3>`` can also be written as ``vctDouble3`` or even simpler ``vct3``.

Vectors, matrices and multi-dimensional arrays
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Fixed size vector: ``vctFixedSizeVector<type, size>``. For example, one can declare a fixed size vector of 2 unsigned integers using ``vctFixedSizeVector<unsigned int, 2> myVector``.

   -  ``vctDouble2``, ``vctDouble3``, ``vctDouble4``, ...
   -  ``vct2``, ``vct3``, ``vct4``, ... (vectors of ``double``\ s)
   -  ``vctInt2``, ``vctInt3``, ``vctInt4``, ...
   -  Supported types include ``Int``, ``UInt``, ``Char``, ``UChar``, ``Bool``, ...
   -  Supported sizes go from 1 to 9 (as of 2012, more might be added later).
   -  All types are defined in file ``vctFixedSizeVectorTypes.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctFixedSizeVectorTypes.h>`__)

-  Fixed size matrix: ``vctFixedSizeMatrix<type, rows, cols, storageOrder>``. For example, one can declare a 2 by 2 fixed size matrix of unsigned integers using ``vctFixedSizeMatrix<unsigned int, 2, 2> myMatrix``.

   -  The last template parameter defines the storage order, i.e. are the elements stored by row or by column. Options are ``VCT_ROW_MAJOR`` (implicit default across cisstVector) or ``VCT_COL_MAJOR``.
   -  ``vctDouble2x2``, ``vctDouble3x3``, ``vctDouble4x4``, ...
   -  ``vct2x2``, ``vct3x3,``\ vct4x4\ ``, ... (matrices of``\ double`s)
   -  ``vctInt2x2``, ``vctInt3x3``, ``vctInt4x4``, ...
   -  Supported types include ``Int``, ``Char``, ``Bool``, ...
   -  Supported sizes go from 1 to 4 for either rows and columns (as of 2012, more might be added later).
   -  All types are defined in file ``vctFixedSizeMatrixTypes.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctFixedSizeMatrixTypes.h>`__)

-  Dynamic vector: ``vctDynamicVector<type>``. For example, one can declare a dynamic vector of unsigned integers using ``vctDynamicVector<unsigned int> myVector``.

   -  ``vctDoubleVec`` also known as ``vctVec``
   -  ``vctIntVec``, ...
   -  Supported types include ``Int``, ``UInt``, ``Char``, ``UChar``, ``Bool``, ...
   -  All types are defined in file ``vctDynamicVectorTypes.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctDynamicVectorTypes.h>`__)

-  Dynamic matrix: ``vctDynamicMatrix<type>``. For example, one can declare a dynamic matrix of unsigned integers using ``vctDynamicMatrix<unsigned int> myMatrix``.

   -  ``vctDoubleMat`` also known as ``vctMat``
   -  ``vctIntMat``, ...
   -  Supported types include ``Int``, ``UInt``, ``Char``, ``UChar``, ``Bool``, ...
   -  All types are defined in file ``vctDynamicMatrixTypes.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctDynamicMatrixTypes.h>`__)

-  Dynamic multi-dimensional arrays: ``vctDynamicNArray<type>``. For example one can declare a 4D array of unsigned integers using ``vctDynamicNArray<unsigned int, 4> myArray``.

Transformations
~~~~~~~~~~~~~~~

All types are defined in file ``vctTransformationTypes.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctTransformationTypes.h>`__)

-  3D rotations:

   -  3D rotation matrix: ``vctMatrixRotation3<type, storageOrder>``. For example, one can declare a rotation matrix using single precision floating point numbers with ``vctMatrixRotation3<float> myRotation``.

      -  ``vctDoubleMatRot3`` or without the ``Double``, ``vctMatRot3``
      -  ``vctFloatMatRot3``
      -  Since rotations are often represented using matrices, cisstVector also provide the following "shortcut", ``vctDoubleMatRot3`` can be written as ``vctDoubleRot3`` or even shorter ``vctRot3``.

   -  3D rotation using unit quaternion: ``vctQuaternionRotation3<type>``. For example, one can declare a rotation quaternion using doubles with ``vctQuaternionRotation3<double> myRotation``.

      -  ``vctDoubleQuatRot3`` or ``vctQuatRot3``
      -  ``vctFloatQuatRot3``

   -  3D rotation using axis and angle: ``vctAxisAngleRotation3<type>``. The angle is always represented using double precision floating point. The type provided is for the axis representation.

      -  ``vctDoubleAxAnRot3`` or ``vctAxAnRot3``
      -  ``vctFloatAxAnRot3``

   -  3D rotation using Rodriguez ``vctRodriguezRotation3<type>``. This class uses a 3 elements vector. The vector's direction represent the axis of rotation and it's norm represents the angle (in radians).

      -  ``vctDoubleRodRot3`` or ``vctRodRot3``
      -  ``vctFloatRodRot3``

   -  3D rotation using Euler angles ``vctEulerRotation3<order>``.

      -  ``vctEulerZYZRotation3`` (same as ``vctEulerRotation3<vctEulerRotation3Order::ZYZ>``)
      -  ``vctEulerZYXRotation3`` (same as ``vctEulerRotation3<vctEulerRotation3Order::ZYX>``)
      -  Euler rotation types are defined in file ``vctEulerRotation3.h`` (see `current version <https://github.com/jhu-cisst/cisst/blob/main/cisstVector/vctEulerRotation3.h>`__)

-  3D transformations -- to be documented

-  2D rotations -- to be documented

-  2D transformations -- to be documented

Memory layout
-------------

The following section is important to understand one of the key features of the cisstVector library but it is not required for casual users.

One of the key design trait of the cisstVector containers is the ability to own or reference an existing block of memory. For example, one can see a column of a matrix as a vector. One naive implementation would be to copy the column elements to a new vector but this would first create unnecessary copies and second complicate all write commands. To avoid this, the vector class is templated by a parameter representing either a block of memory owned by the vector or a reference (internal a pointer) to an existing block of memory. It is important to note at that point that the block of memory can be owned/managed by another cisst container or an external package. This allows us to create cisstVector references to an C array, OpenCV image, VTK data block, ...

To reference an existing block of memory, one first needs to get a base pointer, i.e. a pointer on the first element. The second parameter is the offset between elements, also known as '''stride'''. For a compact block of memory, this stride is set to one, i.e. one just has to increment the element pointer by 1 to move from one element to the next. Now, if we consider the matrix example and assuming the elements of the matrix are stored contiguously and row-by-row, a column can be represented using reference with a stride equal to the number of columns. Considering a 3x3 matrix:

\| m(0,0) @ ``p[0]`` \| m(0,1) @ ``p[1]`` \| m(0,2) @ ``p[2]`` \| \| m(1,0) @ ``p[3]`` \| m(1,1) @ ``p[4]`` \| m(1,2) @ ``p[5]`` \| \| m(2,0) @ ``p[6]`` \| m(2,1) @ ``p[7]`` \| m(2,2) @ ``p[8]`` \| \| m(3,0) @ ``p[9]`` \| m(3,1) @ ``p[10]`` \| m(3,2) @ ``p[11]`` \|

Using a reference one can now create multiple "views" of the data:

-  The first column is represented by a vector reference; pointer is ``p[0]``, stride to next element is 3 (i.e. ``p[0]``, ``p[3]``, ``p[6]``, ...) and size is 4:

   \| m(0,0) @ ``p[0]`` \| m(1,0) @ ``p[3]`` \| m(2,0) @ ``p[6]`` \| m(3,0) @ ``p[9]`` \|

   -  For other columns, just change the pointer.

   -  For rows, the stride is 1.

   -  For the diagonal, one can use a reference vector; pointer is ``p[0]``, stride is 4 (i.e. ``p[0]``, ``p[4]``, ``p[8]``) and size is 3 (minimum of number of rows and columns)

      \| m(0,0) @ ``p[0]`` \| m(1,1) @ ``p[4]`` \| m(2,2) @ ``p[8]`` \|

What applies to vectors also applies to matrices and N-arrays. For a matrix, one needs two strides, a row stride which represents the memory offset to go from one element to the next in a column and the column stride which represents the offset to go from one element to the next in a row. For a multi-dimensional array, the number of strides is equal to the array dimension.

For example, the above matrix is stored in a contiguous block of memory and row-by-row so it's row stride is 3 (i.e. number of columns) and it's column stride is 1. Considering the row stride ``rs = 3``, the previous matrix elements can be seen as follow (in the example the column stride is implicitly 1):

\| m(0,0) @ ``p[0 * rs + 0]`` \| m(0,1) @ ``p[0 * rs + 1]`` \| m(0,2) @ ``p[0 * rs + 2]`` \| \| m(1,0) @ ``p[1 * rs + 0]`` \| m(1,1) @ ``p[1 * rs + 1]`` \| m(1,2) @ ``p[1 * rs + 2]`` \| \| m(2,0) @ ``p[2 * rs + 0]`` \| m(2,1) @ ``p[2 * rs + 1]`` \| m(2,2) @ ``p[2 * rs + 2]`` \| \| m(3,0) @ ``p[3 * rs + 0]`` \| m(3,1) @ ``p[3 * rs + 1]`` \| m(3,2) @ ``p[3 * rs + 2]`` \|

Storing the matrix elements row-by-row is often the default in C/C++ while in Fortran it is common to store matrix elements column-by-column. This is called the '''storage order''', and it can either be '''row major''' or '''column major'''. The cisst matrices can use either but the default storage order is row major.

Using matrix strides also allows to create different views of the data:

-  The transpose of the matrix can be obtained by swapping the row and column strides as well as the row and column numbers:

   \| t(0,0) @ ``p[0]`` \| t(0,1) @ ``p[3]`` \| t(0,2) @ ``p[6]`` \| t(0,3) @ ``p[9]`` \| \| t(1,0) @ ``p[1]`` \| t(1,1) @ ``p[4]`` \| t(1,2) @ ``p[7]`` \| t(0,3) @ ``p[10]`` \| \| t(2,0) @ ``p[2]`` \| t(2,1) @ ``p[5]`` \| t(2,2) @ ``p[8]`` \| t(0,3) @ ``p[11]`` \|

   Note that the addresses of the diagonal elements (0, 4 and 8) are unchanged as expected.

-  A sub-matrix, using the same strides but a different starting pointer and different sizes:

   \| s(0,0) @ ``p[4]`` \| s(0,1) @ ``p[7]`` \| \| s(1,0) @ ``p[5]`` \| s(1,1) @ ``p[8]`` \|

In this example, we are considering a 2 by 2 sub-matrix starting at m(1,1).

Examples
--------

Generic containers
------------------

For all examples, we are using the following naming convention: ``s`` stands for scalar, either integer, float, ... ``v`` stands for vector, ``m`` for matrix, ``a`` for N-array and ``c`` can be any cisstVector container (vector, matrix or N-array).

Constructors
~~~~~~~~~~~~

As expected, all cisstVector containers have a default and copy constructor implemented. It is important to note that:

-  Default constructors for vectors, matrices and arrays don't initialize the content, their content is undefined. If you run into different behaviors with some code compiled in either "debug" or "release" mode, you might want to make sure the content of all your containers is initialized properly. One solution is to use the constructor with a default value (e.g. ``vct4 v(1.0);``)
-  For dynamic containers (vectors, matrices and arrays), the default size is 0, i.e. no memory is allocated. One can use the ``SetSize`` or ``resize`` later or the constructor with size. For example ``vctDoubleVec v(10);`` creates a vector of 10 elements. As mentioned above, the vector elements are still not defined so one can use ``vctDoubleVec v(10, 1.234)`` to both set the size and initialize all elements.
-  For all transformations, the default constructor does initialize the content to identity.
-  Copy constructors attempt to perform a deep copy for all cisstVector containers, i.e. the copy constructor will call the assignment operator for each element (``c1[i] = c2[i]``).

.. code:: c++

   // fixed size vector of 3 elements
   vct3 v1;
   v1.SetAll(1.0); // set all elements, by default content is uninitialized!
   // copy of v1
   vct3 v2(v1);
   // create a matrix, zero size
   vctDoubleMat m1;
   // create a matrix, 3 rows, 4 columns and initialized with 5.0.
   vctDoubleMat m2(3, 4, 5.0);

We also provide constructors between types to simplify the users life, i.e. mostly conversions between fixed size and dynamic containers.

.. code:: c++

   // create a fixed size vector and then assign to a dynamic vector
   vctInt4 v1(5); // vector content is 5, 5, 5, 5
   vctIntVec v2(v1); // set the vector's size to 4 and then assign the content from v1
   // create a dynamic vector and then assign it's content to a fixed size vector
   vctIntVec v3(3, 0); // vector content is 0, 0, 0
   vctInt3 v4(v3); // size is predefined to 3, content will be set to 0, 0, 0
   vctInt4 v4(v3); // this call will FAIL at runtime (std::runtime_exception thrown) because size don't match

Finally one can use the constructor based on the ellipsis (``...``). These constructors should be use with extreme caution as the code relies on the ellipsis and can't perform type matching. So make sure the type of elements match the container's element type.

.. code:: c++

   vctInt3 v1(1, 2, 3); // content is 1, 2, 3
   vctDouble3x3 m1(1.0, 0.0, 0.0,
                   0.0, 1.0, 0.0,
                   0.0, 0.0, 1.0); // content is set to identity
   // note that in previous example we used "1.0", not "1" to indicate a double is used.
   vctFloat6 v2(0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f);
   // we have to use "0.0f", not just "0.0" which would be doubles

Failure to provide the right number of arguments and/or provide arguments of the wrong type will likely lead to stack corruptions or at least undesired results.

Memory management
~~~~~~~~~~~~~~~~~

Setting size
^^^^^^^^^^^^

This section only applies to dynamic containers which own their memory, i.e. ``vctDynamicVector``, ``vctDynamicMatrix`` and ``vctDynamicNArray``. The following two methods are available to resize a container after it has been instantiated:

-  ``SetSize``: this methods doesn't guarantee that data is preserved.
-  ``resize``: this methods attempts to preserve the data, i.e. if the new size if smaller that the previous size, part of the data is copied. If the new size if larger than the previous size, all the data is copied. Newly added elements are uninitialized!

Both methods check if the new size is different from the previous. If both sizes are equal, memory is not freed nor allocated and the containers content remains unchanged.

.. code:: c++

   vctDoubleMat m1;
   m1.SetSize(4, 3); // 4 rows, 3 columns
   m1.resize(4, 3); // does nothing, same size

   // multiple dimension arrays are a bit different
   // sizes are provided using a vector of values (using fixed size vectors)
   typedef vctDynamicNArray<short, 3> ArrayType;
   typedef ArrayType::nsize_type SizeType;
   // create an array and allocate memory
   SizeType a1size(128, 256, 256);
   ArrayType a1(a1Size);
   // alternative to set size and allocate
   ArrayType a2;
   a2.SetSize(SizeType(100, 100, 100));

Storage order
^^^^^^^^^^^^^

Constructors, SetSize VCT_DEFAULT_STORAGE, VCT_ROW_MAJOR, VCT_COL_MAJOR, VCT_FORTRAN_ORDER

Memory layout queries
^^^^^^^^^^^^^^^^^^^^^

-- IsCompact, IsRowMajor, IsColumnMajor, IsFortran, StorageOrder, stride, ...

Assigning data
^^^^^^^^^^^^^^

-- Assign and operator =, ForceAssign used set size ...

Accessing elements
~~~~~~~~~~~~~~~~~~

-- random access -- iterators

Operations
~~~~~~~~~~

Using references
~~~~~~~~~~~~~~~~

Writing functions using cisstVector types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- to be documented, 2 strategies, templated or refs with implicit constructors

Optimization
~~~~~~~~~~~~

-- operators, note on vctDynamicVectorReturn ...

-- create local variables, setsize vs Resize -- random access versus iterative access (note on pointer arithmetic)

.. _transformations-1:

Transformations
---------------

Common methods
~~~~~~~~~~~~~~

Conversion methods
^^^^^^^^^^^^^^^^^^

cisstVector uses the following convention to convert from a type of transformation to another, e.g. convert from a 3D rotation represented by a matrix to and from a unit quaternion. Let ``vctTypeA`` and ``vctTypeB`` be two types of equivalent transformations. The following methods should be used to convert back and forth:

-  ``transformA.From(transformB)``: this method will convert from type B to A but an exception will be thrown if the input is not valid, i.e. not normalized. The goal is to make sure one can detect issues at runtime using a ``try``/``catch`` block. If the conversion failed, the state of ``transformA`` is undefined (i.e. it might have been partially modified).
-  ``transformA.FromNormalized(transformB)``: this method will convert from type B to A while making sure the result if valid, i.e. it will attempt to normalize the input or result. This method is the safest but can have a significant cost since normalization is performed for each conversion.
-  ``transformA.FromRaw(transformB)``: this method will convert from type B to A without any normalization test not attempt to normalize the input or result. This method can be used when the user is confident all transformations are valid but some numerical rounding might occur. These numerical errors would trigger an exception if the ``From`` method was used.

One can also use constructors to perform similar conversions:

-  ``vctTypeA transformA(transformB)``: construct ``transformA`` from ``transformB``. This constructor relies on the ``From`` method and will check if the input is normalized.
-  ``vctTypeA transformA(transformB, VCT_NORMALIZE)``: construct ``transformA`` from ``transformB``. This constructor relies on the ``FromNormalized`` method.
-  ``vctTypeA transformA(transformB, VCT_DO_NOT_NORMALIZE)``: construct ``transformA`` from ``transformB``. This constructor relies on the ``FromRaw`` method.

The assignment operator (``=``) is sometimes overloaded and relies on the ``From`` method.

Applying transformations
^^^^^^^^^^^^^^^^^^^^^^^^

-- to be documented: ``*``, ``ApplyTo``, ``ApplyInverseTo``.

Other methods
^^^^^^^^^^^^^

-- to be documented: ``Inverse``, ``Normalized`` self and of, ``AlmostEqual``, ``AlmostEquivalent``.

Rotations
~~~~~~~~~

-- to be documented, specific constructors, access to data

Frames
~~~~~~

-- to be documented: ``vctFrameBase``, ``vctFrame4x4``
