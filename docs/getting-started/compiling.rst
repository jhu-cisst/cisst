.. _compiling-cisst-and-saw:

Compiling cisst and SAW
=======================

The *cisst* libraries and *SAW* components use CMake for their build process, please check `www.cmake.org <http://www.cmake.org>`__. CMake is already available on most Linux distributions and can be installed using MacPort or Brew on Mac OS. A binary installer for Windows is available on CMake’s web site.

If you clone the ``cisst-saw.git`` repository, the main CMakeLists.txt is at the root of the source tree, i.e. at the same level as directories ``cisst`` and ``saw``.

To clone ``cisst`` and most of the ``saw`` components, use: ``sh git clone https://github.com/jhu-cisst/cisst-saw --recursive`` A new directory will be created (``cisst-saw``). This is the source directory CMake will need to start configuring cisst. CMake configures your build in multiple passes as some settings will change based on user preferences. Since the cisst libraries and saw components rely on numerous external libraries and device drivers, it is important to figure out what you really need. Trying to compile everything can be a very tedious task because you might have to download and install many external packages. Here are some important external packages and options.

.. _external-packages:

External packages
-----------------

-  **cisstNetlib** (recommended): this library is a set of numerical routines that can mostly be found on netlib.org. These routines are originally written in Fortran but an equivalent in C is available. On Linux, since it is easy to install a fortran compiler we use the Fortran version. On other OSs, we use the C version by default. You can compile the *cisst* libraries without cisstNetlib but it is recommended to keep ``CISST_HAS_CISSTNETLIB`` turned ``ON`` as many features depend on these numerical routines. cisstNetlib is one of the git sub-modules in the cisst-saw repository. You should build and install cisstNetlib before compiling cisst and the SAW components:

   .. code:: bash

      git clone https://github.com/jhu-cisst/cisst-saw --recursive
      mkdir build-cisstNetlib
      cd build-cisstNetlib 
      cmake ../cisst-saw/cisstNetlib -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release
      make -j install
      cd ..

-  **Qt** (recommended): some Qt widgets are provided with the cisst libraries as well as most examples with a GUI. If you don’t already have Qt installed and don’t intend to write a Qt application, you can leave ``CISST_HAS_QT`` as ``OFF``. On Linux, you should use your package manager to install Qt. On Windows and Mac OS, you should download the latest Qt version from their web site (see also :doc:`install FAQ </getting-started/faq>`)). We support both Qt 5 and 6.

-  **Python and numpy**: most features from cisst and SAW can be used from a Python interpreter. If you plan to use Python, you will need SWIG (Python wrapper generator) along with the Python development libraries and numpy. You can change the variable ``CISST_HAS_SWIG_PYTHON`` to set your preference. On Linux, you should use your package manager to install Python, SWIG and numpy. On Mac OS, Python is already installed but you will need SWIG. MacPort provides a package but you should check the different “variants” to avoid re-installing all possible interpreters (include Python). You can alternatively download SWIG’s source and compile it. For Windows, SWIG provides a very convenient binary distribution on their web page (see also :doc:`install FAQ </getting-started/faq>`).

-  **XML**: cisst provides an XML wrapper which can rely on either QtXML or LibXml2. LibXml2 is preferred since it supports reads and writes and doesn’t require a QApplication. You can use the variable ``CISST_XML_LIB`` to determine which underlying library to use (QtXML or LibXml2). On Linux, you should use your package manager to install either one. On MacOS, MacPort provides both packages. On Windows, QtXML comes with the Qt binary distribution, LibXml2 is a bit harder to find and compile as it has many external dependencies (see also :doc:`install FAQ </getting-started/faq>`).

-  **CppUnit**: we use CppUnit for our unit tests. If you want to run these tests, you will have to set ``CISST_BUILD_TESTS`` to ``ON``. On Linux you should use your package manager to install CppUnit. On Mac OS, MacPort provides a package. On Windows, you will need to download the source files and compile (http://sourceforge.net/apps/mediawiki/cppunit). Compiling CppUnit on Windows is fairly easy as this library doesn’t have any external dependency and comes with a Visual Studio “solution”. See also the :ref:`cisst install FAQ <getting-started-faq-cppunit>` and how to :doc:`run the tests </development/testing/unit-tests>`.

.. _cisst-cmake-options:

*cisst* CMake Options
---------------------

Assuming you're back in the directory where you cloned cisst-saw, create a build directory and start setting your options in CMake:

.. code:: bash

   mkdir build
   cd build 
   ccmake ../cisst-saw -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release

You can replace ``ccmake`` by ``cmake-gui`` if you prefer a GUI.

Once you have decided which external packages to use and configured CMake, you will have to decide which parts of the cisst/saw code you want to compile:

-  You can turn ``ON``/``OFF`` each library using ``CISST_<library_name>``

-  All examples can be turned ``ON/OFF`` using ``CISST_BUILD_EXAMPLES``. If ``CISST_BUILD_EXAMPLES`` is turned ``ON``, you can use the CMake advanced options to select which examples should be compiled, library-by-library. If examples are available for a given library, the option will be ``CISST_<library-name>_EXAMPLES``.

-  Tests programs are treated as the examples, i.e. you can use ``CISST_BUILD_TESTS`` to control if any test should be compiled and then use the CMake advanced options to select which tests should be compiled, library-by-library. If tests are available for a given library, the option will be ``CISST_<library-name>_TESTS``.

-  All ''saw'' components can be turned ``ON/OFF`` using the option ``CISST_BUILD_SAW``.

-  ``SAW_BUILD_<component_name>`` options control which SAW component should be built. Most components have their own external dependencies (e.g. vendor’s SDK).

.. _building-using-colcon-for-ros2:

Building using colcon for ROS 2
-------------------------------

If you are using a computer with ROS 2 installed, you can compile the cisst libraries and SAW components using the `colcon build tool <https://colcon.readthedocs.io/>`__.

Install ROS 2
~~~~~~~~~~~~~

Instructions can be found on `docs.ros.org <https://docs.ros.org/>`__. We recommend the following LTS distributions:

-  Ubuntu 24.04: Jazzy Jalisco, https://docs.ros.org/en/jazzy/Installation.html
-  Ubuntu 22.04: Humble Hawksbill, https://docs.ros.org/en/humble/Installation.html

You will need standard ROS 2 and vcstool/colcon packages:

.. code:: bash

   sudo apt install python-is-python3 python3-vcstool python3-colcon-common-extensions python3-pykdl

You will also need standard Ubuntu and development packages:

.. code:: bash

   sudo apt install libxml2-dev libraw1394-dev libncurses5-dev qtcreator swig sox espeak cmake-curses-gui cmake-qt-gui git subversion gfortran libcppunit-dev libbluetooth-dev libhidapi-dev python3-pyudev

Initializing your ROS 2 workspace
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create a workspace directory and retrieve the sources using `vcs` (vcstool) with the configuration files maintained in the `jhu-saw/vcs <https://github.com/jhu-saw/vcs>`__ repository. For example, for the dVRK:

.. code:: bash

   mkdir -p ~/ros2_ws/src
   cd ~/ros2_ws
   vcs import src --input https://raw.githubusercontent.com/jhu-saw/vcs/main/ros2-dvrk-main.vcs --recursive

Configure and build
~~~~~~~~~~~~~~~~~~~

For optimal performance, compile your code in Release mode using `colcon`:

.. code:: bash

   cd ~/ros2_ws
   # make sure you have sourced the system ROS 2 setup
   source /opt/ros/jazzy/setup.bash  # or /opt/ros/humble/setup.bash depending on your OS
   # build the workspace
   colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release

Sourcing your workspace
~~~~~~~~~~~~~~~~~~~~~~~

After a successful build, you must source the installation setup file to make the compiled packages available in your current terminal:

.. code:: bash

   source ~/ros2_ws/install/setup.bash

If you want to automatically source the workspace setup when you log in, add the following lines at the end of your ``~/.bashrc`` file:

.. code:: sh

   # Source ROS 2 setup
   if [ -f ~/ros2_ws/install/setup.bash ]; then
     source ~/ros2_ws/install/setup.bash
   fi

If you're new to Unix, you can launch the text editor ``gedit`` with:

.. code:: sh

   gedit ~/.bashrc

To verify that your environment variables are set correctly after opening a new terminal, run:

.. code:: bash

   env | grep ROS

Maintaining multiple build configurations (optional)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Unlike ROS 1/catkin, `colcon` does not have built-in profiles. To maintain multiple build configurations (e.g., debug vs. release), you can either:

1. **Use separate workspaces:** You can maintain separate workspaces for different build types (e.g. `~/ros2_ws` and `~/ros2_ws_debug`) and build each with the appropriate build type arguments:

   .. code:: bash

      mkdir -p ~/ros2_ws_debug/src
      # clone/vcs import as needed, then build:
      colcon build --cmake-args -DCMAKE_BUILD_TYPE=Debug

2. **Use custom build/install directories in the same workspace:** You can build to different target directories using the `--build-base` and `--install-base` options:

   .. code:: bash

      # Build debug configurations
      colcon build --build-base build_debug --install-base install_debug --cmake-args -DCMAKE_BUILD_TYPE=Debug
      # Source debug build
      source install_debug/setup.bash

      # Build release configurations
      colcon build --build-base build_release --install-base install_release --cmake-args -DCMAKE_BUILD_TYPE=Release
      # Source release build
      source install_release/setup.bash

.. _using-cisst-and-saw:

Using cisst and saw
===================

.. _compiling-against-the-cisst-libraries:

Compiling against the cisst libraries
-------------------------------------

We strongly encourage using CMake.  See :ref:`cmake-for-cisst-users` for the
recommended current workflow.  The examples below provide additional detail
for projects using cisst libraries.

When building against the cisst libraries, specify the libraries your project
needs as components (for example, ``cisstCommon``, ``cisstVector``, or
``cisstStereoVision``).  CMake loads the generated ``cisst-config.cmake``
configuration file using ``find_package``.  Require cisst 1.5 or newer and use
the imported ``cisst::`` targets to link your code.

``find_package (cisst 1.5 REQUIRED COMPONENTS cisstCommon cisstVector)``
checks that these components were compiled.  Linking against a ``cisst::``
target automatically propagates its include directories and nested/external
library dependencies.

.. _cmakeliststxt-examples:

CMakeLists.txt examples
-----------------------

Simple example
~~~~~~~~~~~~~~

.. code:: cmake

   cmake_minimum_required (VERSION 3.16)
   project (myApplication)

   find_package (cisst 1.5 REQUIRED COMPONENTS cisstCommon cisstVector)

   add_library (myLibrary libFile.cpp libFile.h)
   add_executable (myExecutable main.cpp)
   target_link_libraries (myExecutable PRIVATE
     myLibrary
     cisst::cisstCommon
     cisst::cisstVector)

Using ``REQUIRED/COMPONENTS``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example we demonstrate how to be a bit more specific and specify which libraries are needed.

.. code:: cmake

   # make a list of libraries and settings we will need
   set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector)

   find_package (cisst 1.5 REQUIRED COMPONENTS ${REQUIRED_CISST_LIBRARIES})

   add_library (myLibrary libFile.cpp libFile.h)
   add_executable (myExecutable main.cpp)
   target_link_libraries (myExecutable PRIVATE
     myLibrary
     ${cisst_LIBRARIES})

More examples
~~~~~~~~~~~~~

Please note that all ''cisst'' examples come with their own ``CMakeLists.txt`` and can be copied to be compiled outside the ''cisst'' source tree. See more complex examples:

-  An example with a test to make sure cisstNetlib has been compiled: `cisstNumerical tutorial example <https://github.com/jhu-cisst/cisst/blob/main/cisstNumerical/examples/tutorial/CMakeLists.txt>`__
-  An example with Qt: `cisstCommon logger Qt example <https://github.com/jhu-cisst/cisst/blob/main/cisstCommon/examples/LoggerQt/CMakeLists.txt>`__
-  An example with SWIG and Python: `cisstInteractive pythonEmbedded example <https://github.com/jhu-cisst/cisst/blob/main/cisstInteractive/examples/pythonEmbedded/CMakeLists.txt>`__ - please note the use of ``cisstPython`` and ``cisst_add_swig_module``

.. _compiling-against-the-saw-components:

Compiling against the saw components
------------------------------------

The *SAW* components use the conventional CMake approach:

-  Use ``find_package`` to find the CMake component configuration
-  Use ``<component-name>_INCLUDE_DIR`` with ``include_directories`` to set the include path
-  Use ``<component-name>_LIBRARY_DIR`` with ``link_directories`` to set the library path
-  Use ``<component-name>_LIBRARIES`` with ``target_link_libraries`` to make sure your target links against all required libraries.

For each component, the CMake settings should provide the paths and libraries for the wrapper itself as well as all its dependencies (except the cisst libraries). For example, since the component ``sawClaronMicronTracker`` requires some extra libraries provided by the vendor, the CMake variable ``sawClaronMicronTracker_LIBRARIES`` will list all vendor libraries. A typical CMakeLists.txt should first search for *cisst* and then all the required packages:

-  An example with a saw component and Qt: https://github.com/jhu-saw/sawNDITracker/blob/main/examples/CMakeLists.txt

.. _running-cisst-and-saw-programs:

Running *cisst* and *SAW* programs
==================================

Once you have compiled the *cisst* libraries and optionally some *SAW* components, you will very likely need to set some environment variables. This can be done manually or using one of the *cisst* provided option.

.. _environment-variables:

Environment variables
---------------------

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

.. _available-tools:

Available tools
---------------

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
