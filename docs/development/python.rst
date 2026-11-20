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
