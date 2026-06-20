.. _1-compiling-cisst-and-saw:

1. Compiling cisst and SAW
==========================

The *cisst* libraries and *SAW* components use CMake for their build process, please check `www.cmake.org <http://www.cmake.org>`__. CMake is already available on most Linux distributions and can be installed using MacPort or Brew on Mac OS. A binary installer for Windows is available on CMake’s web site.

If you clone the ``cisst-saw.git`` repository, the main CMakeLists.txt is at the root of the source tree, i.e. at the same level as directories ``cisst`` and ``saw``.

To clone ``cisst`` and most of the ``saw`` components, use: ``sh git clone https://github.com/jhu-cisst/cisst-saw --recursive`` A new directory will be created (``cisst-saw``). This is the source directory CMake will need to start configuring cisst. CMake configures your build in multiple passes as some settings will change based on user preferences. Since the cisst libraries and saw components rely on numerous external libraries and device drivers, it is important to figure out what you really need. Trying to compile everything can be a very tedious task because you might have to download and install many external packages. Here are some important external packages and options.

.. _11-external-packages:

1.1. External packages
----------------------

-  **cisstNetlib** (recommended): this library is a set of numerical routines that can mostly be found on netlib.org. These routines are originally written in Fortran but an equivalent in C is available. On Linux, since it is easy to install a fortran compiler we use the Fortran version. On other OSs, we use the C version by default. You can compile the *cisst* libraries without cisstNetlib but it is recommended to keep ``CISST_HAS_CISSTNETLIB`` turned ``ON`` as many features depend on these numerical routines. cisstNetlib is one of the git sub-modules in the cisst-saw repository. You should build and install cisstNetlib before compiling cisst and the SAW components:

   .. code:: bash

      git clone https://github.com/jhu-cisst/cisst-saw --recursive
      mkdir build-cisstNetlib
      cd build-cisstNetlib 
      cmake ../cisst-saw/cisstNetlib -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release
      make -j install
      cd ..

-  **Qt** (recommended): some Qt widgets are provided with the cisst libraries as well as most examples with a GUI. If you don’t already have Qt installed and don’t intend to write a Qt application, you can leave ``CISST_HAS_QT`` as ``OFF``. On Linux, you should use your package manager to install Qt. On Windows and Mac OS, you should download the latest Qt version from their web site (see also :doc:`install FAQ </getting-started/faq>`)). We support both Qt 4 and 5. For Qt5 on Windows, make sure you use the "OpenGL" version, not the "ANGLE" version of Qt.

-  **FLTK** (rarely used): some examples use FLTK. If you don’t already have FLTK installed and don’t intend to write an FLTK application, you can leave ``CISST_HAS_FLTK`` as ``OFF`` (majority of users). On Linux, you should use your package manager to install FLTK. On Mac OS, you can use !MacPort’s package ``fltk-dev``. On Windows, you have two options. The first one is to turn on ``CISST_USE_EXTERNAL`` in CMake. FLTK will be downloaded and built along the cisst/SAW libraries. The second option is to download FLTK yourself (http://www.fltk.org), compile it and then configure cisst/SAW to use your local installation.

-  **Python and numpy**: most features from cisst and SAW can be used from a Python interpreter. If you plan to use Python, you will need SWIG (Python wrapper generator) along with the Python development libraries and numpy. You can change the variable ``CISST_HAS_SWIG_PYTHON`` to set your preference. On Linux, you should use your package manager to install Python, SWIG and numpy. On Mac OS, Python is already installed but you will need SWIG. MacPort provides a package but you should check the different “variants” to avoid re-installing all possible interpreters (include Python). You can alternatively download SWIG’s source and compile it. For Windows, SWIG provides a very convenient binary distribution on their web page (see also :doc:`install FAQ </getting-started/faq>`).

-  **XML**: cisst provides an XML wrapper which can rely on either QtXML or LibXml2. LibXml2 is preferred since it supports reads and writes and doesn’t require a QApplication. You can use the variable ``CISST_XML_LIB`` to determine which underlying library to use (QtXML or LibXml2). On Linux, you should use your package manager to install either one. On MacOS, MacPort provides both packages. On Windows, QtXML comes with the Qt binary distribution, LibXml2 is a bit harder to find and compile as it has many external dependencies (see also :doc:`install FAQ </getting-started/faq>`).

-  **CppUnit**: we use CppUnit for our unit tests. If you want to run these tests, you will have to set ``CISST_BUILD_TESTS`` to ``ON``. On Linux you should use your package manager to install CppUnit. On Mac OS, MacPort provides a package. On Windows, you will need to download the source files and compile (http://sourceforge.net/apps/mediawiki/cppunit). Compiling CppUnit on Windows is fairly easy as this library doesn’t have any external dependency and comes with a Visual Studio “solution”. See also the cisst `install FAQ <DownloadAndInstallationFAQ#CppUnit>`__ and how to `run the tests <Unit-Tests>`__.

.. _12-cisst-cmake-options:

1.2. *cisst* CMake Options
--------------------------

Assuming you're back in the directory where you cloned cisst-saw, create a build directory and start setting your options in CMake:

.. code:: bash

   mkdir build
   cd build 
   ccmake ../cisst-saw -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release

You can replace ``ccmake`` by ``cmake-gui`` if you prefer a GUI.

Once you have decided which external packages to use and configured CMake, you will have to decide which parts of the cisst/saw code you want to compile:

-  You can turn ``ON``/``OFF`` each library using ``CISST_BUILD_<library_name>``

-  All examples can be turned ``ON/OFF`` using ``CISST_BUILD_EXAMPLES``. If ``CISST_BUILD_EXAMPLES`` is turned ``ON``, you can use the CMake advanced options to select which examples should be compiled, library-by-library. If examples are available for a given library, the option will be ``CISST_BUILD_<library-name>_EXAMPLES``.

-  Tests programs are treated as the examples, i.e. you can use ``CISST_BUILD_TESTS`` to control if any test should be compiled and then use the CMake advanced options to select which tests should be compiled, library-by-library. If tests are available for a given library, the option will be ``CISST_BUILD_<library-name>_TESTS``.

-  All ''saw'' components can be turned ``ON/OFF`` using the option ``CISST_BUILD_SAW``.

-  ``SAW_BUILD_<component_name>`` options control which SAW component should be built. Most components have their own external dependencies (e.g. vendor’s SDK).

.. _13-building-using-catkin-build-tools-for-ros:

1.3. Building using catkin build tools for ROS
----------------------------------------------

If you are using a computer with catkin/ROS installed, you can compile most of the cisst libraries and SAW components using the `catkin build tools <http://catkin-tools.readthedocs.org/en/latest/index.html>`__ (please note that this process works with ``catkin build``, not ``catkin_make``).

Install ROS
~~~~~~~~~~~

Instructions can be found on `www.ros.org <http://www.ros.org>`__. ROS has multiple releases, named in alphabetical order (Hydro, Indigo, Jade...). We recommend the following releases:

-  Ubuntu 16.04: Kinetic, http://wiki.ros.org/kinetic/Installation/Ubuntu
-  Ubuntu 18.04: Melodic, http://wiki.ros.org/melodic/Installation/Ubuntu
-  Ubuntu 20.04: Noetic, http://wiki.ros.org/noetic/Installation/Ubuntu (still somewhat experimental)

In the following directions, make sure you replace ``kinetic`` by whatever ROS release you have installed.

You will also need some standard Ubuntu packages.

For **Ubuntu 14.04**, ``sudo apt-get install libxml2-dev libraw1394-dev libncurses5-dev qtcreator swig flite sox espeak cmake-curses-gui cmake-qt-gui libopencv-dev git subversion gfortran libcppunit-dev``. If you want Qt5 instead of Qt4, add: ``sudo apt-get install qt5-default libqt5xmlpatterns5-dev``

For **Ubuntu 16.04**, add: ``sudo apt-get install libxml2-dev libraw1394-dev libncurses5-dev qtcreator swig flite sox espeak cmake-curses-gui cmake-qt-gui libopencv-dev git subversion gfortran libcppunit-dev qt5-default``

For **Ubunty 18.04** and **20.04**, all the packages to install (once ROS Full Desktop is installed) are: ``sudo apt-get install libxml2-dev libraw1394-dev libncurses5-dev qtcreator swig sox espeak cmake-curses-gui cmake-qt-gui git subversion gfortran libcppunit-dev libqt5xmlpatterns5-dev``

Initializing your catkin workspace to work with catkin tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Assuming you have already installed ROS and you're using Ubuntu, you can install the catkin build tools using.

For **Ubuntu up-to 18.04**:

.. code:: bash

   sudo apt-get install python-catkin-tools

For **Ubuntu 20.04**:

.. code:: bash

   sudo apt-get install python3-catkin-tools python3-osrf-pycommon

To make sure your path includes the ROS directories:

.. code:: bash

   source /opt/ros/kinetic/setup.bash   # replace kinetic by whatever your ROS distribution is

If you're using a different ROS version, you'll need a different path, e.g. ``/opt/ros/melodic/setup.bash``.

Then you should be able to create your catkin workspace. Please note that existing workspaces used in combination with ``catkin_make`` are not compatible with the catkin build tools.

.. code:: bash

   mkdir -p ~/catkin_ws/src
   cd ~/catkin_ws
   catkin init

Get the sources
~~~~~~~~~~~~~~~

.. code:: bash

   cd ~/catkin_ws/src
   git clone https://github.com/jhu-cisst/cisst-saw --recursive

Configure and build
~~~~~~~~~~~~~~~~~~~

For (significantly) better performances, you should compile your code in CMake Release mode. Using the catkin build tools, this can be done using profile configuration. Then you can build and update your environment variables:

.. code:: bash

   # make sure we are in the right place
   cd ~/catkin_ws
   # make sure you have the proper ROS environment variables
   source /opt/ros/kinetic/setup.bash  # or whatever your ROS distribution is
   # set default CMake build type
   catkin config --cmake-args -DCMAKE_BUILD_TYPE=Release
   # build
   catkin build --summary
   # set environment variables - MAKE SURE YOU SOURCE THE RIGHT FILE
   source devel/setup.bash

Setting up your environment variables for ROS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have a single catkin workspace, you might want to automatically ``source`` the ``setup.bash`` when you log in. To do so, you should add the following lines at the end of your ``~/.bashrc`` (hidden file in your home directory):

.. code:: sh

   # for ROS
   if [ -f ~/catkin_ws/devel/setup.bash ]; then
     . ~/catkin_ws/devel/setup.bash
   fi
   # for cisst (optional)
   if [ -f ~/catkin_ws/devel/cisstvars.sh ]; then
     . ~/catkin_ws/devel/cisstvars.sh
   fi

If you're new to Unix, you can launch the text editor ``gedit`` with:

.. code:: sh

   gedit ~/.bashrc

Then close all your terminals and re-open one terminal. To make sure your environment variables are set, use:

::

   env | grep ROS

Maintaining multiple Catkin profiles (optional)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For **advanced users**, you can also create multiple profiles and have different compilation options in each profile. To create a release profile, use the following sequence of commands:

.. code:: bash

   # make sure we are in the right place
   cd ~/catkin_ws
   # make sure you have the proper ROS environment variables
   source /opt/ros/kinetic/setup.bash  # or whatever your ROS distribution is
   # create a profile named release with _release extension
   catkin config --profile release -x _release
   # switch to newly created release profile
   catkin profile set release
   # set default CMake build type
   catkin config --cmake-args -DCMAKE_BUILD_TYPE=Release
   # build
   catkin build --summary
   # set environment variables - MAKE SURE YOU SOURCE THE RIGHT FILE (debug vs. release)
   source devel_release/setup.bash

To create another profile (e.g. debug), the safest way is to '''open a new shell''' without any environment variables specific to the current profile and then:

.. code:: bash

   # make sure we are in the right place
   cd ~/catkin_ws
   # make sure you have the proper ROS environment variables
   source /opt/ros/kinetic/setup.bash
   # create a profile named debug with _debug extension
   catkin config --profile debug -x _debug
   # switch to newly created debug profile
   catkin profile set debug
   # set default CMake build type
   catkin config --cmake-args -DCMAKE_BUILD_TYPE=Debug
   # build
   catkin build --summary
   # set environment variables
   source devel_debug/setup.bash

Every time you toggle between profiles, make sure you change two things:

-  Set active profile, e.g. to switch to release:``catkin profile set release``
-  Set environment variables, e.g. to switch to release: ``source ~/catkin_ws/devel_release/setup.bash``

.. _2-using-cisst-and-saw:

2. Using cisst and saw
======================

.. _21-compiling-against-the-cisst-libraries:

2.1. Compiling against the cisst libraries
------------------------------------------

We strongly encourage using CMake. The following section describes the main commands you will need to write a ``CMakeLists.txt`` file for your own code.

*cisst* libraries and “settings” One of the difficulties with CMake is to propagate the different settings used for a library to the library user. For example, if ``cisstCommonQt`` requires Qt, an application using ``cisstCommonQt`` will also need to link against some Qt libraries. During the cisst configuration, the user already had to specify which cisst libraries were compiled and which external packages were used. The cisst CMake scripts store this information in files so it can be used later (all files are in the build tree in ``cisst/CMakeFiles``. When building against the cisst libraries, the user can now specify what is needed using either the library name (e.g. ``cisstCommon``, ``cisstVector``, ``cisstStereoVision``, …) or general settings (e.g. ``cisstQt``, ``cisstFLTK``, ``cisstPython``, ``cisstOpenGL``, …).

``find_package (cisst)`` When ''cisst'' is being configured, a configuration file is created: ``cisst-config.cmake``. In your ``CMakeLists.txt``, using ``find_package (cisst)``, you will need to load this file. Your ``CMakeLists.txt`` should contain one of the following: \* ``find_package (cisst)``: you are just trying to find cisst and don’t care about the configuration. Your script should then check that ``cisst_FOUND`` is true. \* ``find_package (cisst REQUIRED cisstCommon cisstVector cisstPython)``: you want to find cisst and make sure the following components have been compiled. Your script should then check that ``cisst_FOUND_AS_REQUIRED`` is true. This syntax is a bit more complicated but it is recommended.

``include (${CISST_USE_FILE})`` It is important to note that finding ''cisst'' doesn’t change your CMake project settings. To start using ''cisst'' you must ``include`` the file defined by the variable ``${CISST_USE_FILE}``. Doing so will set the include and link directories for the libraries and settings used in ``find_package``. In CMake, this must happen before the target is created (either ``add_library``, ``add_executable`` or ``add_swig_module``) If you haven’t specified any library or setting, all libraries and settings will be used. If you have used the ``COMPONENT`` option for ``find_package``, the include and link directories will be restricted to your settings. The second case is preferred as it tends to reduce the number of options passed to your compiler and linker.

``cisst_target_link_libraries (<target_name> cisstCommon cisstVector …)`` Once you have defined your target you will need to specify which ''cisst'' libraries and settings should be used by the linker. In CMake, one can find the command ``target_link_libraries``. For cisst, we also defined the macro ``cisst_target_link_libraries``. This macro should only be used to link against ''cisst'' libraries (e.g. ``cisstMultiTask``) or cisst settings (e.g. ``cisstQt``). On top of a regular ``target_link_libraries``, the macro checks that the required ''cisst'' libraries and settings have been compiled. It also adds all the external dependencies for each ''cisst'' library (e.g. if ``cisstMultiTask`` has been compiled with ZeroC ICE, it will add the required ICE libraries).

.. _22-cmakeliststxt-examples:

2.2. CMakeLists.txt examples
----------------------------

Simple example
~~~~~~~~~~~~~~

.. code:: cmake

   # in CMake we will have to set cisst_DIR to <cisst-build-tree>/cisst
   find_package (cisst)

   # make sure cisst is found
   if (cisst_FOUND)

     # this will set the include and link directories
     # using all cisst settings
     include (${CISST_USE_FILE})
     
     # add a library
     add_library (myLibrary libFile.cpp libFile.h)
     # add a program
     add_executable (myExecutable main.cpp)
     # usual CMake link
     target_link_libraries (myExecutable myLibrary)

     # using cisst libraries and settings
     # if any of these has not be compiled/set, you will get a CMake error
     cisst_target_link_libraries (myExecutable cisstCommon cisstVector cisstQt)

   else (cisst_FOUND)
     message (SEND_ERROR "Oops, cisst not found")
   endif (cisst_FOUND) 

Using ``REQUIRED/COMPONENTS``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example we demonstrate how to be a bit more specific and specify which libraries are needed.

.. code:: bash

   # make a list of libraries and settings we will need
   set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstQt)

   # in CMake we will have to set cisst_DIR to <cisst-build-tree>/cisst
   find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

   # make sure cisst is found AS REQUIRED
   # this checks that all required components are found
   if (cisst_FOUND_AS_REQUIRED)

     # this will set the include and link directories
     # using only the settings for cisstCommon, cisstVector and cisstQt
     include (${CISST_USE_FILE})
     
     # same as above
     add_library (myLibrary libFile.cpp libFile.h)
     add_executable (myExecutable main.cpp)
     target_link_libraries (myExecutable myLibrary)

     # using cisst libraries and settings as defined before
     cisst_target_link_libraries (myExecutable ${REQUIRED_CISST_LIBRARIES})

   else (cisst_FOUND_AS_REQUIRED)
     message (SEND_ERROR "Oops, cisst ${REQUIRED_CISST_LIBRARIES} not found")
   endif (cisst_FOUND_AS_REQUIRED) 

More examples
~~~~~~~~~~~~~

Please note that all ''cisst'' examples come with their own ``CMakeLists.txt`` and can be copied to be compiled outside the ''cisst'' source tree. See more complex examples:

-  An example with a test to make sure cisstNetlib has been compiled: [source:trunk/cisst/cisstNumerical/examples/tutorial/CMakeLists.txt cisstNumerical tutorial example]
-  An example with Qt: [source:trunk/cisst/cisstCommon/examples/LoggerQt/CMakeLists.txt cisstCommon logger Qt example]
-  An example with SWIG and Python: [source:trunk/cisst/cisstInteractive/examples/pythonEmbedded/CMakeLists.txt cisstInteractive pythonEmbedded example] - please note the use of ``cisstPython`` and ``cisst_add_swig_module``

.. _23-compiling-against-the-saw-components:

2.3. Compiling against the saw components
-----------------------------------------

The *SAW* components use the conventional CMake approach:

-  Use ``find_package`` to find the CMake component configuration
-  Use ``<component-name>_INCLUDE_DIR`` with ``include_directories`` to set the include path
-  Use ``<component-name>_LIBRARY_DIR`` with ``link_directories`` to set the library path
-  Use ``<component-name>_LIBRARIES`` with ``target_link_libraries`` to make sure your target links against all required libraries.

For each component, the CMake settings should provide the paths and libraries for the wrapper itself as well as all its dependencies (except the cisst libraries). For example, since the component ``sawClaronMicronTracker`` requires some extra libraries provided by the vendor, the CMake variable ``sawClaronMicronTracker_LIBRARIES`` will list all vendor libraries. A typical CMakeLists.txt should first search for *cisst* and then all the required packages:

-  An example with a saw component and Qt: https://github.com/jhu-saw/sawNDITracker/blob/master/examples/CMakeLists.txt
-  An example with some extra checks, testing if ``cisstMultiTask`` uses ZeroC Ice: [source:trunk/saw/components/sawMedtronicStealthlink/examples/CMakeLists.txt sawMedtronicStealthlink example]

.. _24-edit-cmakeliststxt-with-emacs:

2.4. Edit CMakeLists.txt with Emacs
-----------------------------------

CMake mode is available for Emacs:

-  Download: http://www.cmake.org/CMakeDocs/cmake-mode.el
-  Documentation: http://www.cmake.org/Wiki/CMake_Editors_Support

.. _3-running-cisst-and-saw-programs:

3. Running *cisst* and *SAW* programs
=====================================

Once you have compiled the *cisst* libraries and optionally some *SAW* components, you will very likely need to set some environment variables. This can be done manually or using one of the *cisst* provided option.

.. _31-environment-variables:

3.1. Environment variables
--------------------------

The following environment variables might be needed to run your *cisst* applications properly:

-  ``CISST_ROOT``: this environment variable is required for most *cisst* based application to run properly. It is used in combination with ``cmnPath`` to locate resource files at runtime (e.g. images, configuration files, loadable modules, ...). If you are running your programs against the build tree, this variable should contain the path ``<your_build_tree>/cisst``. If you are running your programs against an installed version of ``cisst``, ``CISST_ROOT`` should be the installation root (e.g. ``/usr/local``, ``c:/program files/cisst-1.0``, ...).
-  Path to executables: this variable is optional but convenient to start any *cisst* or *SAW* program.
-  If you are using a *cisst* build tree, it should contain ``<your_build_tree>/cisst/bin``. On Windows, you need to append the configuration name, i.e. ``Debug``, ``Release``, ``RelWithDebInfo`` or ``RelMinSize`` based on the configuration you have compiled and intend to use.
-  If you are using an installed version of *cisst*, the standard path should work for most Unix systems. For Windows, the path should contains ``<your_install_root>/bin``.
-  Path to libraries: this is important if you have compiled *cisst* using shared libraries. Different operating systems use different environment variables:
-  Windows uses ``PATH``
-  Unix in general uses ``LD_LIBRARY_PATH``
-  Mac OS uses ``DYLD_LIBRARY_PATH``
-  Python path: ``PYTHONPATH``, path to find generated ``.py`` and ``.pyd`` or ``.so`` generated by SWIG.

.. _32-available-tools:

3.2. Available tools
--------------------

Unix
~~~~

All generated scripts are found in the *cisst* build tree.

-  sh and bash: load the script ``cisstvars.sh`` using the command "``. cisstvars.sh``". Note the dot before ``cisstvars.sh``, this is how one executes the script content in the current shell as opposed to ``sh cisstvars.sh`` which would create a new shell, set the proper variables and then kill the newly created shell.
-  csh and tcsh: load the script ``cisstvars.csh`` using "``source cisstvars.csh``". Note that ``source`` is not equivalent to ``csh cisstvars.csh`` which would set the variables in a new shell instead of the current one.

Both scripts will set ``CISST_ROOT`` and update the paths to find executables, dynamic libraries and Python files.

Windows
~~~~~~~

On Windows, we offer two different approaches:

-  Using batch scripts, namely ``cisstvars.bat`` and optionally ``visual-studio-<config>.bat``. The first script (``cisstvars.bat``) requires a parameter to indicate which build the user wants to use, i.e. ``Debug``, ``Release``, ... For example, if one plans to use the debug version, use ``cisstvars.bat Debug``. During the configuration, we also generated four scripts if the CMake generator is Visual Studio, ``visual-studio-<config>.bat``. These scripts will do the following:
-  Set the environment variables for Visual Studio
-  Set the environment variables for *cisst* for a given configuration.
-  Start Visual Studio in this shell. Once Visual Studio is started, it is **important** to set the configuration to match the script name, i.e. if the script used was ``visual-studio-Release.bat``, use the build configuration ``Release``.
-  Add some environment variables using the registry. To do so, in CMake, you can turn the flag ``CISST_PATH_REGISTER``. This is an advanced variable. Please note that once this flag is set, it will modify your user settings and these settings will apply system wide. If you are using multiple copies of ''cisst'' this might lead to some confusion! When this option is turned on, it will create the following environment variables:
-  ``CISST_PATH``, path to executables and dynamic libraries. This variable can then be used to extend your own path, i.e. set ``PATH`` to whatever it contains followed by ``;%CISST_PATH%``
-  ``CISST_PYTHONPATH``, path to Python files. As for ``CISST_PATH``, this variable can be added to your existing ``PYTHONPATH`` by appending ``;%CISST_PYTHONPATH%``
-  ``CISST_ROOT``

To summarize, the first method is recommended if you are likely to use different versions of *cisst* on a single computer while the second method is recommended if you plan to use a single version of *cisst*.
