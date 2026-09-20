cisstStereoVision Architecture
==============================

-  :doc:`Handling image overlays </libraries/cisstStereoVision/filter-image-overlay>`

Example 6 - Image Overlays
==========================

List of files
-------------

-  ``CMakeLists.txt``: CMake script for creating compiler dependent projects
-  ``CMyEventHandler2.h``: Image window event handler class declaration
-  ``CMyEventHandler2.cpp``: Image window event handler class definition
-  ``main.cpp``: Application entry point (``main()`` function)
-  ``xray.avi``: Sample video file
-  ``wall.bmp``: Sample image file

All files can be found under ``cisstStereoVision/examples/tutorial5``.

Source code
-----------

CMake
~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial5/CMakeLists.txt
   :language: cmake
   :start-after: # [doc-cmake-start]
   :end-before: # [doc-cmake-end]

Even handler declaration
~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial5/CMyEventHandler2.h
   :language: c++
   :start-after: // [doc-handler-h-start]
   :end-before: // [doc-handler-h-end]

Event handler implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial5/CMyEventHandler2.cpp
   :language: c++
   :start-after: // [doc-handler-cpp-start]
   :end-before: // [doc-handler-cpp-end]

Main program
~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial5/main.cpp
   :language: c++
   :start-after: // [doc-main-start]
   :end-before: // [doc-main-end]
