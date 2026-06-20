Tutorial
========

These examples provide a quick introduction to the features of cisstVector. The code can be found in the git repository under ``cisstVector/examples/tutorial``. To compile your own code, remember to include ``cisstVector.h``.

Manipulating fixed size vectors and frames
------------------------------------------

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleFrame.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In the example, we used some fixed size vector of 3 doubles (``vct3``) and some of the methods and operators available for this class (``Norm``, ``DifferenceOf``, ``CrossProductOf``, operators -, %, etc.).

We also introduced a rotation matrix and a frame which can be used with the cisst fixed size vectors (``vct3``, same as ``vctDouble3``). For more information related to transformations, see the cisstVector User Guide

Manipulating dynamic vectors and matrices
-----------------------------------------

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleDynamic.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In this example, we created a couple of dynamic vectors as well as a dynamic matrix. Dynamic containers are convenient for large collections of data, or when the number of elements is provided during runtime. Since the allocation is dynamic, it is important to check that the sizes of the operands are compatible. Our library throws exceptions, derived from the Standard Library ``std::exception`` class, on illegal operation arguments, such as unmatching vectors or out-of-range element access. This is why we use a try and catch structure.

The space allocated for a vector or a matrix can be changed in two ways. ``SetSize`` discards any old data and allocated memory in the specified size. ``resize`` preserves the old data by first allocating new space and then copying the elements from the old space to the new one.

The Diagonal method is a first example of manipulating container slices, or vector references. The concept is demonstrated in the next example code.

Container slices and vector references
--------------------------------------

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleReference.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

This example demonstrates the use of slices through a dynamic matrix. The term "slice" refers to a contiguous subregion of a larger vector or matrix. In the example, we directly address columns, rows, and a diagonal of the large matrix matrix through the methods ``Column``, ``Row`` and ``Diagonal``. We use the word ``Ref`` to indicate a vector or matrix object that doesn't allocate its own memory, but overlays another object's memory, such as a slice in a matrix. These slices have appropriately named types, which are ``ColumnRefType``, ``RowRefType``, and ``DiagonalRefType``.

Next, we define a submatrix ``slice``, using the type ``MatrixType::Submatrix::Type`` (the reason for this notation will be given soon). The constructor takes the location of the first element of the submatrix in the large matrix, and the dimensions of the submatrix. As we can see, we can operate on the submatrix just as we do on any matrix.

The next example shows how to use slices with fixed-size vectors and matrices. In the example, we allocate a 4 x 4 homogeneous transformation matrix, and relate to parts of it as a rotation component and a translation component.

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleReferenceFixedSize.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Here, we see that in fixed-size objects, the size of the submatrix has to be given in template parameters, though the location of its first element is still passed as a regular function argument. In C++ it is impossible to have templated typedef statements. Instead, we declare a templated inner class: ``MatrixType::Submatrix<3,3>``, and that inner class has an internal typedef of its own type as Type. Similarly, for the slice of the first three elements in the last column, we use the type ``MatrixType::ColumnRefType::Subvector<3>::Type``.

Note that the element type in this example is float, while the rotation matrix ``rot30`` is double. We can assign vectors and matrices of different element types to each other, but normally we do not allow other operations between them. Also note that we explicitly define literals of type float using the suffix f. This may reduce the number of compiler warnings. Also, we consider it safer to use explicit casting of the arguments whenever they are passed in a variable-length list (``va_arg``, or an ellipsis in the function declaration). This has to do with the mechanism used in C and C++ for handling variable-length argument list. So generally, if we have a long vector v of type double, the following code may generate an error:

.. code:: c++

       v.Assign(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

Instead, use explicit literal type:

.. code:: c++

       v.Assign(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);

For more information regarding the different type of references available, refer to the cisstVector User Guide.

Manipulating multidimensional arrays
------------------------------------

Multidimensional arrays can be used to represent volumes (medical imaging), images with multiple layers (separating RGB channels) or any dataset of high dimension. vctDynamicArray can also be used for datasets of dimension 1 or 2 but for these, vctDynamicVector and vctDynamicMatrix might be a better choice.

Multidimensional arrays can only be found in the dynamic allocation flavor, i.e. there is no vctFixedSizeNArray. A multidimensional array type is defined by the type of elements stored as well as by the dimension, i.e. both of these are defined at compilation time and cannot be changed at runtime. As for the vectors and matrices, it is recommended to define an nArray type to use in your code:

.. code:: c++

       typedef vctDynamicNArray<unsigned short, 3> InputVolumeType;

Since the dimension defines the number of indices required to randomly access an element as well as the number of sizes to resize an nArray, it is also very convenient to define both an index and size type:

.. code:: c++

       typedef InputVolumeType::nsize_type SizeType;
       typedef InputVolumeType::nindex_type IndexType;

The following code illustrates how to create, fill and operate on nArrays:

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleNArray.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Multidimensional arrays also provide ways to create slices and other references. It is possible to:

-  Use only a sub-array while keeping the dimension, i.e. create a window along each dimension Reduce the visible dimension, i.e. only consider n-1 dimensions

-  Use the data with a permutation of indices, i.e. permuting the dimensions

In this example, we reduced the dimension using the method ``SliceOf``. The type of a slice is defined using the same type of elements and subtracting one from the dimension. To ease the programmer's life, one can use ``NormalizedVolumeType::SliceRefType`` and ``NormalizedVolumeType::ConstSliceRefType``. A more subtle way to slice an nArray is to use the square brackets (``operator []``). This is similar to a matrix operator [] as both operators return a reference container with a lower dimension.

Finally, the method ``PermutationOf`` allows to view the nArray from a different "angle" by implicitly re-ordering the dimensions (please note that the data itself is not moved or copied as for all cisst "Ref" objects). This can be compared to the method ``TransposeOf`` for a matrix.

Using the C++ Standard Template Library
---------------------------------------

The different containers of cisstVector have been written to be compatible with the STL. They define different iterators as well as the methods required to manipulate these iterators.

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleSTL.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

In this example, we demonstrated the use of an STL generic algorithm (``std::sort``) on a cisstVector container.

Using cisstCommon
-----------------

This example requires to include ``cisstCommon.h``.

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleCommon.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

This example illustrates how to use the cisst ``cmnRandomSequence`` to fill a vector or matrix with random numbers.

The macro ``CMN_ISNAN`` allows to check if a variable is still a number. It is defined in ``cisstCommon/cmnPortability.h``.

The default tolerance is used in many methods of cisstVector (e.g. ``IsNormalized()`` for any transformation) and it might be useful to increase it for a given application. This should be used with caution. The class ``cmnTypeTraits`` contains some useful information per type (double, float, char, int, etc) such as ``HasNaN``, ``MinNegativeValue``, etc.

Writing your own functions
--------------------------

These examples are reserved to advanced programmers. They require a fairly good understanding of the C++ templates and the class hierarchy of cisstVector (refer to the cisstVector User Guide). The terms Const and Ref refer to the cisstVector classes (e.g. ``vctDynamicConstVectorRef``), not the C++ keyword ``const`` and symbol ``&``.

It is important to understand that the declaration of a new templated function or method is much more complex than the call to this function or method. When you will call the method, the compiler will infer (i.e. deduce) the template parameters based on the type of the objects used as function parameters. This allows to create a very generic method while preserving the ease of use.

Using dynamic containers
~~~~~~~~~~~~~~~~~~~~~~~~

The first four functions show different possible signatures to use for a function parameter. The fifth signature is for a function that takes two matrices containing the same type of elements, but each matrix can be either a Reference or not, Const or not.

The last example shows how to use a ``vctReturnDynamicVector`` to avoid a copy of all the elements of a vector when it is returned (this approach is valid for a matrix as well).

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleDynamicFunctions.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Using fixed size containers
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fixed size containers are similar to the dynamic ones with a major exception, the size(s) and stride(s) must be specified at compilation time. This requirement adds a fair amount of template parameters.

.. literalinclude:: ../../../cisstVector/examples/tutorial/ExampleFixedSizeFunctions.h
   :language: c++
   :start-after: // [doc-example-start]
   :end-before: // [doc-example-end]

Since the fixed size containers are designed for fairly small containers (up to approximately 10 elements) and they use stack memory, there is no specific return type (as opposed to ``vctReturnDynamicVector`` or ``vctReturnDynamicMatrix``).
