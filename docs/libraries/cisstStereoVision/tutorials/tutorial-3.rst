cisstStereoVision Architecture
==============================

-  [wiki:SVLTutorialFilterImageWindow Handling image window events (mouse and keyboard)]

Example 3 - Image Window Event Handling
=======================================

List of files
-------------

-  ``CMakeLists.txt``: CMake script for creating compiler dependent projects
-  ``CMyEventHandler.h``: Image window event handler class declaration
-  ``CMyEventHandler.cpp``: Image window event handler class definition
-  ``main.cpp``: Application entry point (``main()`` function)

All files can be found under ``cisstStereoVision/examples/tutorial2``.

Source code
-----------

CMake
~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial2/CMakeLists.txt
   :language: cmake
   :start-after: # [doc-cmake-start]
   :end-before: # [doc-cmake-end]

Event handler class declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial2/CMyEventHandler.h
   :language: c++
   :start-after: // [doc-handler-h-start]
   :end-before: // [doc-handler-h-end]

Event handler class implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial2/CMyEventHandler.cpp
   :language: c++
   :start-after: // [doc-handler-cpp-start]
   :end-before: // [doc-handler-cpp-end]

Main program
~~~~~~~~~~~~

.. literalinclude:: ../../../../cisstStereoVision/examples/tutorial2/main.cpp
   :language: c++
   :start-after: // [doc-main-start]
   :end-before: // [doc-main-end]
