cisst Libraries overview
========================

+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| Library              | Description                                                                                                     | Status   |
+======================+=================================================================================================================+==========+
| cisstCommon          | Common infrastructure such as logging, class and object registries, serialization and de-serialization, etc.    | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstVector          | Basic linear algebra and spatial transformations in two dimensions (2D) and three dimensions (3D)               | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstNumerical       | Thread-safe numerical methods (relies on LAPACK)                                                                | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstInteractive     | C++ classes and Python scripts for the Interactive Research Environment (IRE)                                   | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstOSAbstraction   | Operating system services (e.g., threads, mutual exclusion, etc.) for Windows, Linux, Mac OS X, RTAI/Linux, ... | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstMultiTask       | Component-based framework for defining tasks and devices, provided and required interfaces, and command objects | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstMesh            | Mesh and point cloud classes for 2D and 3D datasets                                                             | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstStereoVision    | Mono and stereo video acquisition, processing, and display                                                      | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstParameterTypes  | Standard data types used by command objects in component-based framework                                        | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisstRobot           | Robot control elements, including cooperative control, trajectory control, etc.                                 | Stable   |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+
| cisst3DUserInterface | Support for 3D user interfaces (3D input and 3D display)                                                        | Obsolete |
+----------------------+-----------------------------------------------------------------------------------------------------------------+----------+

Most libraries also come with examples and test programs.

SAW components overview
=======================

The SAW package currently contains the following implemented components:

+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| Component                                                       | Description                                                                                                                       | Status   |
+=================================================================+===================================================================================================================================+==========+
| saw3Dconnexion                                                  | Interface to 3D Connexion Space Navigator 3D mouse                                                                                | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawATIForceSensor                                               | Interface to ATI force sensor (Ethernet)                                                                                          | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawAtracsysFusionTrack </jhu-saw/sawAtracsysFusionTrack>`__    | Interface to Atracsys fusionTrack                                                                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawBarrett                                                      | Interface to Barrett Technology Whole Arm Manipulator (WAM) robot and components                                                  | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawCANBus                                                       | Generic Controller Area Network (CAN) bus interfaces                                                                              | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawClaronMicronTracker                                          | Interface to Claron Technology Micron optical tracking system                                                                     | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawCMUSphinx4                                                   | Interface to Sphinx4 speech recognition package                                                                                   | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawConstraintController                                         | Constraint based controller                                                                                                       | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawConstraintOptimizer                                          | Constraint optimizer                                                                                                              | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawControllers </jhu-saw/sawControllers>`__                    | Various control components, including PD (with gravity comp.), PID, teleoperation                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawCopleyController </jhu-saw/sawCopleyController>`__          | Interface to Copley Xenus motor controller                                                                                        | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawDataPlayer                                                   | Application to replay recorded multi-media data                                                                                   | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawDATAQSerial                                                  | DATAQ serial port                                                                                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawForceDimensionSDK </jhu-saw/sawForceDimensionSDK>`__        | Interface to the ForceDimension SDK, supports haptic devices Omega, Sigma and Novint Falcons                                      | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawGalilController </jhu-saw/sawGalilController>`__            | Interface to Galil motor controller                                                                                               | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawGLUTSimulator                                                | GLUT-based simulator (simpler than sawOpenSceneGraph simulator)                                                                   | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawIntuitiveDaVinci </jhu-saw/sawIntuitiveDaVinci>`__          | Interface to da Vinci surgical robot via read-only research interface (requires license and library from Intuitive Surgical)      | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawIntuitiveResearchKit </jhu-dvrk/sawIntuitiveResearchKit>`__ | Components for the da Vinci Research Kit                                                                                          | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawJoystick                                                     | Interface to joysticks                                                                                                            | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawJR3ForceSensor                                               | Interface to JR3 force sensor using Comedi                                                                                        | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawKeyboard </jhu-saw/sawKeyboard>`__                          | Keyboard interface (generate commands or events from key presses)                                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawMaxonEPOS </jhu-saw/sawMaxonEPOS>`__                        | Interface to Maxon controllers using the EPOS Command Library over USB                                                            | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawMedtronicStealthlink                                         | Interface to Medtronic Stealthstation via Stealthlink research interface (requires license and library from Medtronic Navigation) | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawMicroScribeDigitizer                                         | Interface to `Revware <http://www.revware.net>`__ MicroScribe digitizer device (requires Arm32Dll SDK from Revware)               | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawNDITracker </jhu-saw/sawNDITracker>`__                      | Interface to Northern Digital Inc. (NDI) tracking systems that use a serial port; includes Polaris and Aurora                     | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawOpenAL                                                       | Interface to Open Audio Library                                                                                                   | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawOpenDynamicsEngine                                           | Interface to Open Dynamics Engine (ODE) physics-based simulator; requires sawOpenSceneGraph                                       | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawOpenIGTLink </jhu-saw/sawOpenIGTLink>`__                    | Bridge between ''cisst'' interfaces and OpenIGTLink                                                                               | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawOpenNI                                                       | Interface to Microsoft Kinect via OpenNI                                                                                          | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawOpenSceneGraph                                               | Interface to Open Scene Graph (OSG) visualization software (used for simulations)                                                 | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawOptoforceSensor </jhu-saw/sawOptoforceSensor>`__            | Interface to OptoForce force sensors using a serial port over USB                                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawRobotIO1394 </jhu-saw/sawRobotIO1394>`__                    | Interface to custom `IEEE-1394 (Firewire) motor controller </jhu-cisst/mechatronics>`__                                           | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawSartoriusScale                                               | Interface to high-precision scale                                                                                                 | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawSensablePhantom </jhu-saw/sawSensablePhantom>`__            | Interface to Sensable Phantom haptic device (e.g., Phantom Omni)                                                                  | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawSocketStreamer </jhu-saw/sawSocketStreamer>`__              | Component to stream data from another component over a socket                                                                     | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawTextToSpeech </jhu-saw/sawTextToSpeech>`__                  | Component to convert text to speech output (Windows, Linux, Mac)                                                                  | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| sawTrajectories                                                 | Various trajectory planners and generators (no hardware dependency)                                                               | Inactive |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+
| `sawUniversalRobot </jhu-saw/sawUniversalRobot>`__              | Interface to Universal Robots via real-time scripting interface                                                                   | Active   |
+-----------------------------------------------------------------+-----------------------------------------------------------------------------------------------------------------------------------+----------+

cisst libraries description
===========================

Following are descriptions of some of the *cisst* libraries:

cisstCommon
-----------

(:doc:`FAQ </libraries/cisstCommon/faq>`) Common infrastructure for the rest of the package such as logging, error and exception handling, class and object registries, serialization and de-serialization, ...

cisstVector
-----------

(:doc:`Tutorial </libraries/cisstVector/tutorial>`, :doc:`Concepts </libraries/cisstVector/concepts>`) cisstVector is a library for basic linear algebra and spatial transformations in two dimensions (2D) and three dimensions (3D). It contains classes for vectors and matrices, whose sizes can be pre-specified (fixed size) or dynamic. All containers can be defined as memory allocating or overlaid. The former provide user control over matrix storage order (row-major or column-major). The latter facilitate wrapping any memory block with a vector or matrix interface, as well as defining slices of existing containers, such as row- and column-vectors in a matrix, or submatrices in a large matrix. The library provides a large collection of algebraic operations (e.g., sums, products, etc.) with uniform interfaces across all container types and efficient implementations, such as template metaprogramming. The structure of the library enables rapid extensions (scalability) across container-element types and container sizes through templated classes, and a simple addition of new methods. The spatial transformations, with generic interfaces, includes a diverse collection of rotation representations, with a uniform syntax for converting from one to another. Besides the broad internal API, the cisstVector library can easily and efficiently interface external software packages, such as LAPACK, VNL and OpenGL, and provides the underlying data types used in the cisstNumerical library. The interfaces can also be exported to a Python environment through Swig wrapping -- typemaps are used to convert between cisstVector vector/matrix types and standard Python ``numpy`` arrays.

cisstNumerical
--------------

(:doc:`Tutorial </libraries/cisstNumerical/tutorial>`) cisstNumerical is a library of numerical routines. The majority of cisstNumerical features rely on well established and tested routines, written in Fortran (such as Singular Value Decomposition, LU decomposition, LU based matrix inverse, Hanson and Haskell least squares with or without constraints...). cisstNumerical adds a user-friendly C++ interface based on the cisstVector fixed size and dynamic vectors and matrices. While this interface greatly eases the task of a novice user, it retains the flexibility required for advanced users by allowing them to overlay their input over pre-allocated memory. Furthermore, the cisstNumerical wrappers ensure that the format of the input is correct (size, storage order and compactness), and, whenever possible, enable the use of any storage order (either Fortran-like column major or C-like row major). Finally, since cisstNumerical uses LAPACK3E, it is thread safe as opposed to most libraries based on LAPACK or CLAPACK. Besides the Fortran wrappers, cisstNumerical contains some optimized functions for specific cases (such as Gauss-Jordan inverse of small fixed size matrices) and some basic algebraic utilities (test orthonormality).

cisstInteractive
----------------

C++ classes and Python scripts for the cisst Interactive Research Environment (IRE).

cisstOSAbstraction
------------------

Wrappers for Operating System services such as threads, mutual exclusion, time, dynamic loading, ...

cisstMultiTask
--------------

(:doc:`Tutorial </libraries/cisstMultiTask/tutorial>`, :doc:`Concepts </libraries/cisstMultiTask/concepts>`) cisstMultiTask supports multi-threaded programming with efficient, lock-free, and thread-safe data exchange. The task class contains a (periodic) thread, state table, and mechanisms for interaction. The design utilizes component-based software engineering concepts, where all interaction occur via interfaces. Each interface contains multiple commands that implement the functionalities provided by that interface.

SAW components description
==========================

SAW components can be:

-  wrappers for existing devices (e.g., haptic device, 3D tracker, IMU, robot...)
-  wrappers for communication libraries (e.g., ROS, OpenIGTLink...)
-  logic components, e.g., low level controllers like PID

A SAW component is derived from ``mtsComponent`` and is defined by its *interfaces*, both *provided* and *required*. See cisstMultiTask :doc:`tutorial </libraries/cisstMultiTask/tutorial>` and :doc:`concepts </libraries/cisstMultiTask/concepts>`.

A well defined component can be:

-  dynamically loaded at runtime using simple configurations files
-  interfaced to ROS 1 with little or no code (see https://github.com/jhu-cisst/cisst-ros)
-  interfaced to OpenIGTLink with little or no code (see https://github.com/jhu-saw/sawOpenIGTLink)
-  integrated in a Qt application using cisst Qt widgets

To make sure components are inter-changeable, interfaces should be as standardized as possible and follow the `CRTK API </collaborative-robotics/documentation/wiki/Robot-API>`__.

Directories
-----------

Components are separated in different git repositories (see https://github.com/jhu-saw). Each repository will contain the following directories:

-  ``component``: this is the main directory for the component's code. The directory itself should have the following sub-directories:

   -  ``include/<component_name>``: component's declaration files, i.e. headers (``.h``)
   -  ``code``: component's implementation files (``.c``, ``.cpp``)

-  ``share`` (optional): examples of data or configurations files for the components
-  ``applications`` (optional): applications using or related to the component
-  ``examples`` (optional): examples using the component, usually just used to test or demonstrate the component's features
-  ``ros`` (optional): ROS 1 node for the component
-  ``igtl`` (optional): `OpenIGTLink <https://www.openigtlink.org>`__ based server to communicate with the component (using `sawOpenIGTLink <https://github.com/jhu-saw/sawOpenIGTLink>`__)
-  ``tests`` (optional): CppUnit based unit tests (see :doc:`cisst unit tests </development/testing/unit-tests>`)

Using a SAW component
---------------------

SAW components are all using *cisstMultiTask* interfaces. They need to be connected to other components, for example a top level component or application component. Such component can just be a GUI component. Many of the SAW components come with a simple example (see ``examples`` directory in the component's repository), usually just the device component and a GUI component (using `Qt <https://www.qt.io>`__).

Some components also have a ROS and/or OpenIGTLink "bridge". These executables usually instantiate a device component and connect it to a bridge. The bridge function is to send and convert data back and forth between the device's component and the corresponding middleware (i.e., ROS topics/services/tf2 or igtl sockets). Your application can then communicate with the device using the middleware and doesn't require to be compiled and linked against cisst/SAW.

Components can also be dynamically loaded and connected. There are two main ways to do this:

C++ dynamic loading
~~~~~~~~~~~~~~~~~~~

You can use the ``mtsComponentManager`` to load the component's shared library, dynamically create and configure an instance of the component's class and finally connect it to other components. This can also be performed using a JSON configuration file. The following example can be found in ```sawSensablePhantom`` <https://github.com/jhu-saw/sawSensablePhantom>`__\ ``/share/igtl``. The file ``manager-igtl-default.json`` contains the name of the shared library to load, the class name and parameter to use when instantiating the class (i.e., constructor parameter):

.. code:: json

   {
       "components":
       [
           {
               "shared-library": "sawOpenIGTLink",
               "class-name": "mtsIGTLCRTKBridge",
               "constructor-arg": {
                   "Name": "igtlBridge",
                   "Period": 0.01
               },
               "configure-parameter": "igtl-default.json"
           }
       ]
   }

The last parameter (``configure-parameter``) is the file used to configure the bridge. In this case, we are creating an OpenIGTLBridge for our device, a Sensable Phantom haptic device. Once the components are connected, the cisst/SAW program will be able to send data and receive commands over OpenIGTLink (mostly likely with `Slicer <https://www.slicer.org>`__). The configuration file format depends on the SAW component (see ``igtl-default.json`` in ```sawSensablePhatom`` <https://github.com/jhu-saw/sawSensablePhantom>`__\ ``/share``)

Python
~~~~~~

Using the *cisst* Python wrappers, it is also possible to dynamically load the component's library, and create and configure an instance of the component. Once the component is created, you can then create a Python proxy (acts like a bridge). The proxy will be created with an interface (*required interface*) that will match the component's interface (*provided interface*). Once the proxy is connected to the device's component, you can use Python to receive data and send commands to the device. You can find a *cisstMultiTask* Python example in ```sawNDITracker`` <https://github.com/jhu-saw/sawNDITracker/>`__\ ``/examples/mainPython.py``. The main commands used are:

-  ``services.Load``: load the component's shared library
-  ``services.ComponentCreate``: dynamically create the component
-  ``component.Configure``: configure the component
-  ``controller = proxy.AddInterfaceRequiredAndConnect``: create the matching interface and connect it. Once this is done, the Python object ``controller`` can use the commands provided by the device's component (in this example, ``Connect``, ``Beep``, ``ToggleTracking``...)
