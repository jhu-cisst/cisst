Example 4 - Creating custom filters
===================================

List of files
-------------

-  ``CMakeLists.txt``: CMake script for creating compiler dependent projects
-  ``CMyFilter1.h``: Custom filter 1 class declaration
-  ``CMyFilter1.cpp``: Custom filter 1 class definition
-  ``main.cpp``: Application entry point (``main()`` function)

Source code
-----------

All files can be found under ``cisstStereoVision/examples/tutorial3``.

CMake
~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial3/CMakeLists.txt
   :language: cmake
   :start-after: # [doc-cmake-start]
   :end-before: # [doc-cmake-end]

Custom filter 1 class declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial3/CMyFilter1.h
   :language: c++
   :start-after: // [doc-filter-h-start]
   :end-before: // [doc-filter-h-end]

Custom filter 1 class implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial3/CMyFilter1.cpp
   :language: c++
   :start-after: // [doc-filter-cpp-start]
   :end-before: // [doc-filter-cpp-end]

Main program
~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial3/main.cpp
   :language: c++
   :start-after: // [doc-main-start]
   :end-before: // [doc-main-end]
