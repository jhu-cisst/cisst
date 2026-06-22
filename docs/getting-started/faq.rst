CISST Libraries
===============

On Linux (unix command line)
----------------------------

1. Do the following commands to get the source from the *cisst* github repository

.. code:: bash

   git clone https://github.com/jhu-cisst/cisst-saw.git --recursive 

1. Once you have downloaded the source, use CMake to configure *cisst* in a separate build directory

.. code:: bash

   mkdir build
   cd build
   ccmake ../cisst-saw

1. After creating your makefiles using CMake, build the *cisst* libraries

.. code:: bash

   make

On MAC
------

Using XCode on Mac OS X Leopard
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Notes:**

-  Installation on more recent Mac OS X should be similar.

-  For cisstStereoVision only: If you plan to use *OpenCV*, you need to install it. We recommend using MacPort

-  For cisstStereoVision only: If you plan to use the svlImageWindow filter or other modules with *X11* dependency, you need to install XQuartz (https://xquartz.macosforge.org).

-  You may use *XCode* for editing, compiling and debugging. However command line applications have to be executed from the *Terminal* because *XCode* does not open a command line for them by default. (Although there might be a way to force it somehow.)

1. Do the following commands to get the source from the *cisst* github repository

.. code:: bash

   git clone https://github.com/jhu-cisst/cisst-saw.git --recursive 

1. Once you have downloaded the source, use CMake to configure *cisst* in a separate build directory

.. code:: bash

   mkdir build
   cd build
   ccmake -G Xcode ../cisst-saw

1. After creating your makefiles using CMake, build the *cisst* libraries

.. code:: bash

   make

1. Open the project in *XCode* by double clicking on ``cisst-saw.xcodeproj`` in Mac OS Finder. When opening the project, *XCode* might ask for the directory to be used with the project. In that case just leave the default selection and continue.

On Windows
----------

Reminder, you need a C++ compiler (Visual Studio) and CMake (http://www.cmake.org).

1. Install a git client, see available clients from http://git-scm.com/downloads/guis (see also :doc:`cisst Download </getting-started/download>`)

2. Checkout cisst from https://github.com/jhu-cisst/cisst-saw.git, make sure you set your git client's flags to checkout recursively to get all the git submodules.

3. Once you have downloaded everything into source, use CMake to configure cisst in the build directory

4. Run CMake.

-  Where is the source code: cisst source directory
-  Where to build the binaries: custom build directory

1. Click *Configure* and turn ON the desired libraries; e.g.:

-  ``CISST_BUILD_cisstMultiTask = ON``
-  ``CISST_BUILD_cisstNumerical = ON``
-  ``CISST_BUILDcisstOSAbstraction = ON``
-  ``CISST_BUILD_cisstParameterTypes = ON``
-  ``CISST_HAS_CISSTNETLIB = ON``

1. Click *Configure* and set:

-  ``CISSTNETLIB_DOWNLOAD_NOW = ON``

1. Click *Configure* and click *Generate*
2. Open ``cisst-saw.sln`` under cisst build directory.
3. Build ``ALL_BUILD`` in debug and release configurations.

Optional External Dependencies
==============================

The following external dependencies are optional. The cisst libraries can be compiled without them, but with less functionality.

cisstNetLib
-----------

cisstNetLib is a customized binary distribution of some elements from the Netlib repository (which is Fortran code). The binary files can be downloaded and extracted (https://github.com/jhu-cisst/cisstNetlib), though it is more convenient to use CMake to download it, as follows (assuming you have an Internet connection):

1. Select the ``BUILD_LIBS_cisstNumerical`` option, then press ``Configure``
2. Two new CMake entries will appear: ``CISSTNETLIB_DIR`` and ``CISSTNETLIB_DOWNLOAD_NOW``
3. Select ``CISSTNETLIB_DOWNLOAD_NOW`` and press ``Configure`` again
4. Depending on your OS, you may be prompted for additional information, such as 32 vs. 64 bits
5. CMake will automatically download the cisstNetLib libraries to your build tree and set the paths


Python
------

To wrap the cisst libraries for Python, the following steps are needed:

1. Install Python on your system (Version 2.X, where X is >= 3). The cisst libraries have not been tested with Python 3.X.
2. Install numpy, because the cisst mathematical types (e.g., fixed size and dynamic vectors and matrices) are mapped to numpy arrays.
3. Install SWIG, which automatically wraps C++ code for Python (as well as other languages)

To use the Interactive Research Environment (IRE):

1. Select the ``CISST_BUILD_cisstInteractive`` CMake option
2. Install wxPython or iPython

XML
---

To use XML within cisst, you must turn ON the ``CISST_BUILD_cisstCommonXML`` option in CMake. There are two options for XML support: LibXML2 or Qt. CMake will attempt to find these packages on its own. If both are present, the default is to use libXML2. This can be changed in CMake by modifying the CISST_XML_LIB variable to be either LibXml2 or QtXML. Note, however, that to use the Qt XML, your code must have a ``QApplication`` (i.e., it is not enough to just link with the Qt libraries). Also, if you are getting linker errors in external projects, be sure that ``cisstCommonXML`` is added to your ``cisst_target_link_libraries``, i.e.:

``cisst_target_link_libraries (yourProgram cisstCommon cisstCommonXML ....)``

.. _windows-1:

Windows
~~~~~~~

-  Go to http://www.zlatkovic.com/pub/libxml/ and download the zlib, libxml2, and iconv zipped folders. Extract these.
-  Each folder has a lib, include, and bin subfolder. Put the contents of the lib, include, and bin files of the zlib and iconv into their respective places (subfolders of the same name) in the libxml2 folder. The end result should be that each folder within libxml2 should contain the following:
-  bin
-  ``libxml2.dll``
-  ``minigzip.exe``
-  ``zlib1.dll``
-  ``iconv.exe``
-  ``iconv.dll``
-  ``xmlcatalog.exe``
-  ``xmllint.exe``
-  include
-  ``libxml`` folder
-  ``iconv.h``
-  ``zconf.h``
-  ``zlib.h``
-  lib
-  ``iconv_a.lib``
-  ``zdll.lib``
-  ``zlib.lib``
-  ``iconv.lib``
-  ``libxml2.lib``
-  ``libxml2_a.lib``
-  ``libxml2_a_dll.lib``
-  You can discard the original zlib and iconv folders, which should now be empty.
-  Now that this is done, run cmake on your CISST folder and turn on CISST_HAS_XML, it will ask you the location of ``xmllint.exe``, include, and ``libxml2.lib``.
-  Remember to recompile CISST afterwards.


Qt
--

We only support Qt4 and Qt5. Qt5 on Windows can be compiled with OpenGL or ANGLE, make sure you download the OpenGL version. If you're compiling Qt from scratch make sure you compile it against OpenGL, not ANGLE. Please refer to https://www.qt.io/ for general installation and compilation instructions.

For Qt4, set the ``QT_QMAKE_EXECUTABLE`` option in CMake. For Qt5, set the Qt5XXX_DIR CMake variables to the appropriate CMake files provided by your Qt5 installation; for example set ``Qt5Core_DIR`` to the directory in your Qt5 installation, ``lib/cmake/Qt5Core``.

uDrawGraph
----------

The cisst libraries include a component viewer(mtsComponentViewer class in cisstMultiTask) that displays an interactive graphical visualization of the components in the system, whether a single process or distributed between different processes. This component viewer relies on the uDrawGraph package that can be obtained from http://www.informatik.uni-bremen.de/uDrawGraph/en/home.html

After installing it, be sure to add the binary directory to your path.

.. _linux-1:

Linux
~~~~~

-  Download LINUX package from http://www.informatik.uni-bremen.de/uDrawGraph/en/download/download.html

-  Extract uDraw: ``tar xfz uDrawGraph-3.1.1-0-linux-i386.tar.gz``

-  Copy to /usr/local/: ``sudo mv uDrawGraph-3.1 /usr/local/``

-  Add the binary directory to path and ``$UDG_HOME`` (add the following block to .bashrc file)

   .. code:: bash

       # uDrawGraph
       if [ -f /usr/local/uDrawGraph-3.1/README.txt ]; then
           export UDG_HOME=/usr/local/uDrawGraph-3.1
           export PATH=$PATH:/usr/local/uDrawGraph-3.1/bin
       fi

GLUT
----

The OpenGL Utility Toolkit (GLUT) is used by sawGLUTSimulator.

.. _windows-2:

Windows
~~~~~~~

-  Download and install GLUT (http://www.xmission.com/~nate/glut.html). For detailed installation process, please refer to README-win32.txt file (which is a bit dated) and/or the following:
-  Copy ``glut32.dll`` to ``C:\Windows\System32`` (on a 64-bit machine, this should be ``C:\Windows\SysWOW64``).
-  Copy ``glut.h`` to your C++ include directory; by convention this should be in a ``GL`` subdirectory
-  For Visual Studio 2008, this could be ``C:\Program Files\Microsoft Visual Studio 9.0\VC\include\GL`` (create the ``GL`` subdirectory if it does not exist, or put the file in ``C:\Program Files\Microsoft Visual Studio 9.0\VC\include``)
-  For Visual Studio 2003, this could be ``C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Include\gl``
-  Copy ``glut32.lib`` to your C++ library directory
-  For Visual Studio 2008, this could be ``C:\Program Files\Microsoft Visual Studio 9.0\VC\lib``
-  For Visual Studio 2003, this could be ``C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Lib``

.. _linux-2:

Linux
~~~~~

-  Install the freeglut package
-  On Fedora, "yum install glut-devel"
-  On Ubuntu, "apt-get install freeglut3-dev"

Hardware Drivers
----------------

Galil Motion Controller
~~~~~~~~~~~~~~~~~~~~~~~

-  Download and install from http://www.galilmc.com/support/software-downloads.php GalilTools 1.3.0.0.
-  Obtain and copy debug ``.dll`` and ``.lib`` files to corresponding folders under Program Files.

National Instruments Data Acquisition (DAQ) Hardware
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Download and install from http://joule.ni.com/nidu/cds/view/p/id/1278/lang/en NI-DAQmx 9.0.

.. _3dconnexion-spacenavigator-3d-mouse:

3Dconnexion SpaceNavigator 3D Mouse
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Download and install from http://www.3dconnexion.com/support/downloads.php SpaceNavigator SE.

.. _getting-started-faq-cppunit:

CppUnit
-------

The new way (automatic)
~~~~~~~~~~~~~~~~~~~~~~~

We now use CMake External Project to download and patch CppUnit. This is convenient on computers that don't already have CppUnit. For Linux and Mac OS, we still recommend to use the native package managers to install the development versions of CppUnit (using ``apt-get``, ``port``, ...). On other computers (Windows mostly), you can now use the cisst CMake configuration option ``CISST_USE_EXTERNAL``. When ``CISST_USE_EXTERNAL`` is turned on and you turn on the option to build the cisst unit tests (``CISST_BUILD_TESTS``), CppUnit sources will downloaded and compiled along cisst when you are building cisst. Please note that this requires a network connection for the first build.

The old way (manual)
~~~~~~~~~~~~~~~~~~~~

The latest stable version of CppUnit package is 1.12.1. There is a CMake patch available for cppunit, which can be obtained via the Tracker/Patches link in the `SourceForge cppunit page <http://sourceforge.net/projects/cppunit>`__. We have updated this patch and attached it here:

-  [attachment:cppunit-CMakeLists.txt]: rename to ``CMakeLists.txt`` and place in root of cppunit source tree
-  [attachment:cppunit-config.h.cmake]: rename to ``config.h.cmake`` and place in the ``/config`` subfolder in the cppunit source tree

Depending on the compiler (e.g., for MingW), it may also be necessary to patch the file ``TypeInfoHelper.cpp``, in the ``src/cppunit`` subfolder, by adding the following line:

.. code:: c++

   #include <malloc.h>

This ``CMakeLists.txt`` has been modified to build a static library, but can be edited to produce a dynamic library (e.g., DLL).

If CppUnit 1.12.0 is built from source code and you're using Microsoft Visual Studio C++ 2008, there are probably compilation errors such as the following (this does not seem to occur with 1.12.1):

::

   Compiling...
   MsDevCallerListCtrl.cpp
   .\MsDevCallerListCtrl.cpp(67) : error C3505: cannot load type library '{80CC9F66-E7D8-4DDD-85B6-D9E6CD0E93E2}'
   .\MsDevCallerListCtrl.cpp(67) : fatal error C1104: fatal error importing libid: '80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2'

This is because the CppUnit 1.12.0 currently supports up to MSVS 7 of IDE for automation (see `this link <http://cppunit.svn.sourceforge.net/viewvc/cppunit/trunk/cppunit/INSTALL-VS.Net2008.txt?view=markup>`__). In order to fix this error, you need to patch line 67 of ``MsDevCallerListCtrl.cpp`` as follows:

Before:

::

   #import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("7.0") lcid("0") raw_interfaces_only named_guids

After:

::

   #if (_MSC_VER < 1500)
   #import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("7.0") lcid("0") raw_interfaces_only named_guids
   #else
   #import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("9.0") lcid("0") raw_interfaces_only named_guids
   #endif

FAQ
===

This is a collection of known issues (e.g., compiler or linker errors) with solutions to fix the problems.

.. _windows-3:

Windows
-------

   cisstNetlibGCC.lib(\_chkstk.o) : error LNK2026: module unsafe for SAFESEH image.

This ``LNK2026 - SAFESEH image`` link error occurs because of the optional compiler/linker security feature in MSVS. If you don't need this increased security feature, you can disable ``SAFESEH`` option as follows:

-  Open project property pages of your project
-  Go to Linker > Advanced, and set ``Image Has Safe Exception Handlers`` to ``No (/SAFESEH:NO)``.

Using CMake:

::

   if (WIN32)
       SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /SAFESEH:NO")
       SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /SAFESEH:NO")
       SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /SAFESEH:NO")
   endif (WIN32)

This feature does not apply to x64. For more information, refer to `this <http://msdn.microsoft.com/en-us/library/9a89h429.aspx>`__.
