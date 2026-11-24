CMake for cisst
===============

.. note::

   This page is for cisst **library developers**. For users building against
   cisst, see :ref:`cmake-for-cisst-users`.

.. _cmake-for-cisst-users:

CMake for cisst Users
---------------------

Applications and external libraries should find the installed cisst package,
request the components they use, and link its imported targets.  Do not add
cisst include directories, library directories, or use cisst helper linking
macros manually; the imported targets propagate their required usage
requirements and dependencies.

.. code:: cmake

   cmake_minimum_required (VERSION 3.16)
   project (myApplication)

   find_package (cisst 1.5 REQUIRED COMPONENTS cisstCommon cisstVector)

   add_executable (myApplication main.cpp)
   target_link_libraries (myApplication PRIVATE
     cisst::cisstCommon
     cisst::cisstVector)

When using ``COMPONENTS``, CMake also populates ``cisst_LIBRARIES`` with the
required libraries.  It can be passed to ``target_link_libraries`` when a
component list is assembled dynamically:

.. code:: cmake

   target_link_libraries (myApplication PRIVATE ${cisst_LIBRARIES})

Set ``cisst_DIR`` to the directory containing ``cisst-config.cmake``, or add
the cisst installation prefix to ``CMAKE_PREFIX_PATH``, when CMake cannot find
the package.  See :ref:`compiling-against-the-cisst-libraries` for additional
examples and component-specific guidance.

CMake for cisst Developers
--------------------------

For the cisst libraries developers, we have a few important macros that should be used to maintain the consistency of the CMake configuration:

-  ``cisst_add_library``: this macro is used to add a new library. For example, a Qt library using EigenQt can declare its cisst dependencies separately from its external dependencies:

   .. code:: cmake

      cisst_add_library (
        LIBRARY cisstParameterTypesQt
        LIBRARY_DIR cisstParameterTypes
        DEPENDENCIES cisstCommon cisstMultiTask cisstParameterTypes
        SOURCE_FILES prmStateJointQtWidget.cpp
        HEADER_FILES prmStateJointQtWidget.h prmExportQt.h)
      target_link_libraries (cisstParameterTypesQt EigenQt::Widgets)

   In this example the following keywords are used:

   -  ``LIBRARY``: name of the library to be generated
   -  ``LIBRARY_DIR``: relative path to include files from the cisst source directory, i.e. ``cisst/``
   -  ``DEPENDENCIES``: list of cisst libraries required for this library
   -  ``SOURCE_FILES``: list of source files found in the current source directory, i.e. directory containing this CMakeLists.txt file
   -  ``HEADER_FILES``: list of header files found in the ``LIBRARY_DIR``. In this example, header files are found in ``cisst/cisstParameterTypes``
   -  ``ADDITIONAL_SOURCE_FILES`` (and ``ADDITIONAL_HEADER_FILES``): extra source (header) files not found in the default directories. These can be generated files, private header files, ...
