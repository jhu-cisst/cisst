CMake for cisst Developers
==========================

.. note::

   This page is for cisst **library developers**. For users building against
   cisst, see :doc:`Compiling cisst and SAW with CMake </getting-started/compiling>`.



For the cisst libraries developers, we have a few important macros that should be used to maintain the consistency of the CMake configuration:

-  ``cisst_set_package_settings``: this macro allows to save some specific settings associated to a given cisst library. For example, in the ``cisstCommon`` CMakeLists.txt configuration file, ``cisst_set_package_settings (cisstCommon LibXml2 INCLUDE_DIRECTORIES LIBXML2_INCLUDE_DIR)`` indicates that any library compiled against ``cisstCommon`` will need to use some extra include directories (keyword ``INCLUDE_DIRECTORIES``) because ``cisstCommon`` has been compiled with LibXml2 support. The extra include directories are defined by the CMake variable ``LIBXML2_INCLUDE_DIR`` defined by ``FindLibXml2.cmake``. The following settings can be saved:

   -  ``INCLUDE_DIRECTORIES``: will be used with CMake ``include_directories`` in the cisst use file (``include (${CISST_USE_FILE})``

   -  ``LIBRARIES``: will be used by ``cisst_target_link_libraries``

   -  ``LINK_DIRECTORIES``: will be used with CMake ``link_directories`` in the cisst use file (``include (${CISST_USE_FILE})``

   -  ``PACKAGES``, ``PACKAGE_COMPONENTS`` and ``CMAKE_FILES``: will be used by CMake ``find_package`` and ``include`` in the cisst use file (``include (${CISST_USE_FILE})``

-  ``cisst_unset_all_package_settings``: this macro is used to remove all settings. For example ``cisst_unset_all_package_settings (cisstCommon LibXml2)`` tells that ``cisstCommon`` doesn't use LibXml2 anymore. For all external dependencies, the CMake configuration should have an ``if else endif`` to make sure settings are removed when an external dependency is either not found or turned off by the user.

-  ``cisst_add_library``: this macro is used to add a new library. For example, the \`cisstVectorQt CMakeLists.txt CMake configuration file] contains:

   .. code:: cmake

      cisst_add_library (
        LIBRARY cisstVectorQt
        LIBRARY_DIR cisstVector
        DEPENDENCIES cisstCommon cisstVector
        SOURCE_FILES vctPlot2DOpenGLQtWidget.cpp
        HEADER_FILES vctPlot2DOpenGLQtWidget.h vctExportQt.h
        ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

   In this example the following keywords are used:

   -  ``LIBRARY``: name of the library to be generated
   -  ``LIBRARY_DIR``: relative path to include files from the cisst source directory, i.e. ``trunk/cisst/``
   -  ``DEPENDENCIES``: list of cisst libraries required for this library
   -  ``SOURCE_FILES``: list of source files found in the current source directory, i.e. directory containing this CMakeLists.txt file
   -  ``HEADER_FILES``: list of header files found in the ``LIBRARY_DIR``. In this example, header files are found in ``trunk/cisst/cisstVector``
   -  ``ADDITIONAL_SOURCE_FILES`` (and ``ADDITIONAL_HEADER_FILES``): extra source (header) files not found in the default directories. These can be generated files, private header files, ...
