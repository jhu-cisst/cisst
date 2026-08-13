Configuring *cisst* for Python
==============================

To use the *cisst/SAW* Python wrappers, you will need both SWIG and Python installed. If you plan to use the *cisstVector* data types as well you will need numpy.

Once you've installed all the prerequisites, set the following options during the *cisst/SAW* CMake configuration

-  ``CISST_BUILD_SHARED_LIBS``: ``ON``
-  ``CISST_HAS_SWIG_PYTHON``: ``ON``

When you're done configuring *cisst/SAW* you should have the following variables set properly:

-  ``SWIG_DIR``
-  ``SWIG_EXECUTABLE``
-  ``PYTHON_EXECUTABLE``
-  ``PYTHON_INCLUDE_DIR``
-  ``PYTHON_LIBRARY``
-  ``PYTHON_NUMPY_INCLUDE_DIR``

Using the *cisst* Python wrappers
=================================

Most cisst libraries come with some Python wrappers generated using SWIG. To see if a library has been wrapped, check if the interface file ``cisstXyz/cisstXyz.i`` exists (e.g. ``cisstVector/cisstVector.i``).

Before you can load any *cisst* Python wrappers, you should make sure your paths are set properly. Do to so, see :doc:`compiling and running cisst/SAW </getting-started/compiling>`.

Once your paths are set, you should be able to start your Python interpreter (e.g. ``ipython``):

.. code:: sh

   ipython

In Python:

.. code:: python

   import cisstCommonPython
   dir(cisstCommonPython)
   print(cisstCommonPython.cmnClassRegister_ToString())

*cisstVector* typemaps
======================

This section is an introduction to writing functions that will use the *cisstVector* typemaps.

Who needs to read this section?
-------------------------------

If you are writing C++ software that needs to be accessible by Python users and uses the *cisstVector* library, this document is for you. To use your C++ classes in Python, you will still need to use SWIG to wrap your class and methods. If your methods use any *cisstVector* types as parameters, the typemaps provided along the *cisstVector* library will convert the C++ vectors and matrices into Python numpy arrays. You will then be able to use the fast collection of numerical tools built on top of numpy.

In C++, there are many ways to pass or return data (by copy, reference, pointer … const or not, …) and not all of them make sense when using *cisstVector* containers. Furthermore, SWIG potentially requires a different typemap for each possible signature. As a consequence, there is a limited number of signatures supported by the cisst libraries.

Quick reference
---------------

Quickly find the recommended function signature to use

+----------------------------------------------+----------------------------+-------------------------------------------------+
| My data is a ...                             | I want to ... my container | Use this function signature                     |
+==============================================+============================+=================================================+
| ``vctDynamicContainer<elementType>``         | Read                       | ``f(vctDynamicConstContainerRef<elementType>)`` |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Read/write                 | ``f(vctDynamicContainerRef<elementType>)``      |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Resize                     | ``f(vctDynamicContainer<elementType> &)``       |
+----------------------------------------------+----------------------------+-------------------------------------------------+
| ``vctDynamicContainerRef<elementType>``      | Read                       | ``f(vctDynamicConstContainerRef<elementType>)`` |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Read/write                 | ``f(vctDynamicContainerRef<elementType>)``      |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Resize                     | Not allowed                                     |
+----------------------------------------------+----------------------------+-------------------------------------------------+
| ``vctDynamicConstContainerRef<elementType>`` | Read                       | ``f(vctDynamicConstContainerRef<elementType>)`` |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Read/write                 | Not allowed                                     |
+----------------------------------------------+----------------------------+-------------------------------------------------+
|                                              | Resize                     | Not allowed                                     |
+----------------------------------------------+----------------------------+-------------------------------------------------+

The three recommended function/method signatures
------------------------------------------------

Functions that use a *cisstVector* container as an input parameter fall into three categories: those that have read-only access to the container, those that can read and write to the container (but not resize it), and those that can read, write, or resize the container. The latter one requires a dynamic allocation of memory.

Functions that have read-only access to the container
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The recommended function signature to use for read-only access is ``function(vctDynamicConstContainerRef<elementType>);`` Functions of this signature take a “CISST const reference” (denoted by the “…Const…Ref” naming convention, as opposed to a C++ const reference, denoted by “const … &”) as input. The CISST const reference is essentially a container with a const pointer to an existing block of memory. When used in combination with NumPy, this allows the typemap to directly reference the existing NumPy array data; i.e. the function runs without the overhead of copying the data to a new container. This is by far the recommended signature if the method only needs to read the container’s data.

Note that a ``vctDynamicConstContainerRef`` object has implicit constructors to convert from a ``vctDynamicContainerRef`` or a ``vctDynamicContainer`` object, so any of these three containers can be passed as parameters to a function or method with this signature. Also note that since this signature provides read-only access, the NumPy array passed to this function can be writable or non-writable.

Example
^^^^^^^

Say you want to write a function that takes a vector of doubles and returns the sum of the vectors’ elements. You should give the function read-only access to the container’s data by declaring the signature as:

.. code:: c++

   double SumOfElements(vctDynamicConstVectorRef<double> v);

After defining the function, you would use it in C++ like the following:

.. code:: c++

   vctDoubleVec v(10);
   vctRandom(v, 0.0, 1.0); 
   double sum = SumOfElements(v); // a vctDynamicConstVectorRef is created implicitly

Via the cisstVector typemaps, you could also use this function in Python:

.. code:: python

   v = numpy.ones(10)
   print(SumOfElements(v)) # no copy of data performed, the typemap creates a ConstRef

Functions that can read and write to the container (but not resize)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The recommended function signature to use for read and write access is ``function(vctDynamicContainerRef<elementType>);`` Functions of this signature require a CISST non-const reference, denoted by “…Ref”. Similar to the read-only signature parameter, a ``vctDynamicContainerRef`` object is essentially a container with a non-const pointer to an existing block of memory. The typemap will create a Ref to the NumPy array data; this allows the function to run without the overhead of copying the data to a new container.

Note that a ``vctDynamicContainerRef`` object has an implicit constructor to convert from a ``vctDynamicContainer`` object (see Table 2). Also note that since this function signature provides read and write access, the NumPy array passed to this function must be writable.

.. _example-1:

Example
^^^^^^^

Say you need to write a function that normalizes a vector of doubles. You would give the function write access to the container’s data by declaring the signature as:

.. code:: c++

   void Normalize(vctDynamicVectorRef<double> v);

After defining the function, you would use it in C++ like the following:

.. code:: c++

   vctDoubleVec v(10);
   vctRandom(v, 0.0, 1.0);
   Normalize(v);

To use this function in Python:

.. code:: python

   v = numpy.ones(10)
   Normalize(v)

Note that the following would not work:

.. code:: python

   v = numpy.ones(10)
   v.setflags(write=False)
   Normalize(v)    # exception; v must be writable

Functions that can resize the container
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The recommended function signature to use for resizing a container is ``function(vctDynamicContainer<elementType> &);`` The parameter in this signature is a C++ reference (as opposed to a CISST reference) to a CISST container. The parameter must be a memory-owning container (as opposed to a container reference), since only memory-owning containers can be resized (using the methods resize or SetSize). Unlike the other two signatures, a ``vctDynamicContainer`` does not have implicit constructors to convert from another container type (see Table 2). Also, the NumPy array passed to this function must be a writable array that owns its data and has no references on it, because if we resize an array that is being referenced, we free the array’s memory, resulting in a pointer to invalid memory for all existing references. NumPy has the same limitation, i.e. one cannot resize a non-data owning array and a data owning array must have no reference on it.

Please note that this signature requires multiple copies in the typemap provided even though the C++ version doesn’t require any. The typemap will first copy the NumPy array to an equivalent cisst container which can then be passed to the C++ function. Once the function returns, the content of the container has to be copied back to the NumPy array. Furthermore, if the typemap detects a memory re-allocation when the function returns, it will have to free the initial NumPy array and re-allocate a new one. Needless to say that if you are using this signature just to read or modify the data itself, this is far less efficient than the two previous signatures.

.. _example-2:

Example
^^^^^^^

Say you need to write a function that resizes a vector to an arbitrary size. The function would need to have resize access to the container; its signature would be:

.. code:: c++

   void Resize(vctDynamicVector<int> &v, int size);

To call this function in C++:

.. code:: c++

   vctDynamicVector<int> v(10); 
   Resize(v, 15);

To use this function in Python:

.. code:: python

   v = numpy.ones(10) 
   Resize(v, 15)

Note that none of the following cases would work:

.. code:: python

   u = numpy.ones(10)
   u.setflags(write=False)
   Resize(u, 15)    # exception; u must be writable

   v = u[0:10]
   Resize(v, 15)    # exception; v must own its data

   u.setflags(write=True)
   Resize(u, 15)    # exception; v is referencing u's data

Conversions between container types
-----------------------------------

+-----------------------------------------+--------------------------------------------------------------------------------------+
| If the function’s parameter type is ... | In C++, it accepts objects of type ...                                               |
+=========================================+======================================================================================+
| ``vctDynamicConstContainerRef``         | ``vctDynamicConstContainerRef``, ``vctDynamicContainerRef``, ``vctDynamicContainer`` |
+-----------------------------------------+--------------------------------------------------------------------------------------+
| ``vctDynamicContainerRef``              | ``vctDynamicContainerRef``, ``vctDynamicContainer``                                  |
+-----------------------------------------+--------------------------------------------------------------------------------------+
| ``vctDynamicContainer``                 | ``vctDynamicContainer``                                                              |
+-----------------------------------------+--------------------------------------------------------------------------------------+

Additional function signatures
------------------------------

The *cisstVector* typemaps support four additional function signatures for read-only access to a container. Although they are of comparable or worse efficiency than the recommended read-only signature given above, they are provided for convenience. They all accomplish the exact same task as the recommended signature. When in doubt, use the recommended signature.

-  The first is: ``function(const vctDynamicConstContainerRef<elementType> &);`` In this signature, the CISST const reference is passed as a C++ const reference. This may be slightly more efficient than the recommended signature (since only a reference and not a copy of the vctDynamicConstContainerRef object is passed), but passing a CISST reference by C++ reference can become confusing. Also, the CISST reference containers are very lightweight, making the performance gain for this signature minimal.
-  The next signature is: ``function(const vctDynamicContainerRef<elementType> &);`` In this signature, a CISST non-const reference is passed by C++ const reference. The vctDynamicContainerRef class contains non-const methods, but as it is passed as a C++ const reference, the compiler will forbid their use; the end result is that the function will not be able to modify the container. Use of this signature is discouraged, as the parameter name is somewhat ambiguous.
-  The third signature is: ``function(vctDynamicContainer<elementType>);`` The entire container is passed by copy in this signature; as a result, the initial version will not be modified, although the function can internally modify the copied data. From a performance point of view, this means that the vector is copied at each function call. This signature also is not as flexible as the others (it doesn’t support calls such as function(myMatrix.Diagonal()) for instance, as the call to Diagonal(), like calls to many other container functions, returns a CISST reference; see Table 2 above). This signature should be used only when needed on rare occasions. [Note]
-  The final signature is: ``function(const vctDynamicContainer<elementType> &)`` This signature specifies that the function receives a container passed by C++ const reference. Functions with this signature won’t be as flexible as those written with the recommended signature, since CISST references can’t be used directly with this signature (see Table 2). Also, like the previous signature, SWIG copies the NumPy vector passed to this function at each call, so performance will take a hit.

Summary
-------

In total, there are seven supported function signatures: five for read-only access, one for read and write access, and one for resize access. They are summarized below. Recommended signatures are in bold.

+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
| Access level               | Supported function signatures                           | Allowed cisstVector types | Disallowed cisstVector types | Allowed NumPy types | Disallowed NumPy types |
+============================+=========================================================+===========================+==============================+=====================+========================+
| Read-only access           | **``f(vctDynamicConstContainerRef<elementType>)``**     | 1, 2, 3                   | None                         | 4, 5                | None                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
|                            | ``f(const vctDynamicConstContainerRef<elementType> &)`` | 1, 2, 3                   | None                         | 4, 5                | None                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
|                            | ``f(const vctDynamicContainerRef<elementType> &)``      | 2, 3                      | 1                            | 4, 5                | None                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
|                            | ``f(vctDynamicContainer<elementType>)``                 | 3                         | 1, 2                         | 4, 5                | None                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
|                            | ``f(const vctDynamicContainer<elementType> &)``         | 3                         | 1, 2                         | 4, 5                | None                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
| Write access (no resizing) | **``f(vctDynamicContainerRef<elementType>)``**          | 2, 3                      | 1                            | 5                   | 4                      |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+
| Resize                     | **``f(vctDynamicContainer<elementType> &)``**           | 3                         | 1, 2                         | 5’                  | 4, 5                   |
+----------------------------+---------------------------------------------------------+---------------------------+------------------------------+---------------------+------------------------+

Keys:

-  1: ``vctDynamicConstContainerRef<elementType>``
-  2: ``vctDynamicContainerRef<elementType>``
-  3: ``vctDynamicContainer<elementType>``
-  4: Non-writable NumPy array
-  5: Writable NumPy array
-  5’: Writable NumPy array that owns it data and has no references on it

Note: Function signatures that are not supported
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Two function signatures, ``function(vctDynamicConstContainerRef<elementType> &)`` and ``function(vctDynamicContainerRef<elementType> &)``, are explicitly not supported because they allow the user to change the container’s reference to point to arbitrary blocks of data. In particular, the reference could be changed to point to Python memory allocated without a reference count; if that memory is subsequently released, we would have a pointer to freed memory. We chose to avoid this case altogether by not supporting these two signatures.

Note: Safety checks performed by typemaps
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The different typemaps provided perform a number of safety checks to make sure the input parameters (Python objects) can be used as cisst containers. If the checks fail, an exception is thrown.

-  Verify that the input is a NumPy array
-  Verify the dimension, i.e. 1 if a vector is expected, 2 for a matrix, etc …
-  Verify the type of elements, i.e. doubles, int, …
-  Verify the size (for fixed size containers only)
-  Verify if the container is writable when needed
-  Verify that the container owns its memory or has no reference on it
