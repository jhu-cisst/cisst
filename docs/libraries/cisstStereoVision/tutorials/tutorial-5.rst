cisstStereoVision Architecture
==============================

-  Multithreaded processing, see :doc:`Thread Synchronization Macros </libraries/cisstStereoVision/thread-sync>`
-  :doc:`svlFilterSplitter </libraries/cisstStereoVision/filter-splitter>`

Example 5 - Filters with multithreaded processing
=================================================

List of files
-------------

-  ``CMakeLists.txt``: CMake script for creating compiler dependent projects
-  ``CMyFilter2.h``: Custom filter 2 class declaration
-  ``CMyFilter2.cpp``: Custom filter 2 class definition
-  ``CMyFilter3.h``: Custom filter 3 class declaration
-  ``CMyFilter3.cpp``: Custom filter 3 class definition
-  ``main.cpp``: Application entry point (``main()`` function)

All files can be found under ``cisstStereoVision/examples/tutorial4``.

Source code
-----------

CMake
~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/CMakeLists.txt
   :language: cmake
   :start-after: # [doc-cmake-start]
   :end-before: # [doc-cmake-end]

Custom filter 2 class declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/CMyFilter2.h
   :language: c++
   :start-after: // [doc-filter2-h-start]
   :end-before: // [doc-filter2-h-end]

Custom filter 2 class implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/CMyFilter2.cpp
   :language: c++
   :start-after: // [doc-filter2-cpp-start]
   :end-before: // [doc-filter2-cpp-end]

Custom filter 3 class declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/CMyFilter3.h
   :language: c++
   :start-after: // [doc-filter3-h-start]
   :end-before: // [doc-filter3-h-end]

Custom filter 3 class implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/CMyFilter3.cpp
   :language: c++
   :start-after: // [doc-filter3-cpp-start]
   :end-before: // [doc-filter3-cpp-end]

Main program
~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial4/main.cpp
   :language: c++
   :start-after: // [doc-main-start]
   :end-before: // [doc-main-end]
