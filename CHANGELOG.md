Change log
==========

1.0.11 (2019-04-09)
===================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * General:
    * Updated jsoncpp version when using external project
    * By default, look for jsoncpp installed with OS on Linux
    * Added Ubuntu 16.04 for travis
  * cisstCommon:
    * cisstConfig: added CISST_EXECUTABLE_SUFFIX
    * cmnPath::Executable to create OS dependent executable name
    * cmnPath: added DeleteFile static method
    * cmnLogger: added method to set/get default log file name
  * cisstVector:
    * vctMatrixRotation3: constructor from BaseType is now implicit
    * Rotation and vector widgets: added key press to reset orientation
    * Dynamic vector and matrices: exception message for size mismatch now includes expected and received sizes
  * cisstMultiTask:
    * mtsManagerLocal: added RemoveAllUserComponents
    * mtsManagerLocal: added ConfigureJSON(filename)
    * mtsManagerLocal: better error messages if dynamic creation fails
    * mtsTask: State tables now advance once in Startup method (if automatic)
  * cisstRobot:
    * robManipulator: added methods Set/GetJointLimits using vectors
  * cisstParameterTypes:
    * prmForceCartesianGet: added MovingFrame string member
    * prmOperatingState: added type and widget for crtk compatibility
    * prmInputDataQtWidget: new widget
* Bug fixes:
  * cisstCommon:
    * Tests: fixed CMake dependency on JSON libraries
    * Tests: fixed strings for newer Windows compilers and OSs
    * cisstDataGenerator: fixed human readable output
  * cisstOSAbstraction:
    * Tests: use PATH variable to locate osaPipeExec test program
    * osaPipeExec: new Windows implementation for noWindow case
    * osaPipeExec: fixed resource leak
    * osaSocketServer: fix for Windows (based on feedback from dlrdave)
  * cisstMultiTask:
    * mtsCommand: fixed infinite recursion in Execute methods
    * mtsIntervalStatistics: set as valid
    * Tests: multiple fixes to better control creation of component manager and clear component manager
  * cisstParameterTypes:
    * prmEventTypeQtWidget: use switch statement to handle all possible event types

1.0.10 (2018-05-16)
===================

* API changes:
  * cisstCommon:
    * cmnPrintf: removed downcast to boolean for cmnPrintfParser, this was not used and required different implementations for different compilers
    * cmnAssert: from now on, CMN_ASSERT is not defined when NDEBUG is defined.  This behavior mimics assert().
  * cisstMultiTask: mtsIntervalStatistics, renamed accessors and added statistics for computing time
* Deprecated features:
  * None
* New features:
  * cisstCommon:
    * cmnPortability: added CISST_THROW to dynamically specify exception or skip since this is being deprecated in newer C++ standards.  Added unit test for python wrappers to make sure CISST_THROW specify exception for swig.
    * cmnPortability: use std::isnan and std::isfinite when available (see also cisstConfig.h)
    * cmnPrintf: added unit test, removed downcast to boolean for cmnPrintfParser
    * cmnAssert: CMN_ASSERT not defined when NDEBUG is defined, fixed usage accross cisst libraries
    * cmnQt::QApplicationExistOnCtrlC helper function to quit QApplication on ctrl+c
  * cisstVector:
    * vctVector3DQtWidget: widget to display a single 3D vector (used for 3D force/torque)
    * vctForceTorqueQtWidget: widget to display 3D force/torque as text, 2D graph or 3D vector
    * Fixed Qt widgets to display frame/3D vector to display reference frame, user can change orientation using mouse.  Added `SetPrismaticRevoluteFactors` to display in mm/degrees
  * cisstDataGenerator:
    * Added "deprecated" keyword for data members
  * cisstOSAbstraction
    * osaPipeExec: added `noWindow` parameter to `Open` method. The default setting is `false`, which is backward-compatible. On Microsoft Windows, setting it to `true` prevents the display of a console window.  This parameter is ignored on other operating systems
  * cisstMultiTask:
    * mtsIntervalStatistics: added statistics re. average computation time and overruns wrt previous time window, updated Qt widget and added conversion method to ROS messages (cisst-ros)
    * Fixed SWIG code for Python 3
  * cisstRobot:
  * cisstParameterTypes:
    * prmPositionJointSet: use data generator
    * prmVelocityCartesianGet: use data generator, added moving/reference frame
    * Added prmJointType, prismatic or revolute
    * prmStateJoint: added vector of joint types.  In Qt widget, added `SetPrismaticRevoluteFactors` used to display in mm/degrees
    * prmPositionCartesianGetQtWidget: displays frame using cisstVector frame widget, timestamp, valid flag and moving/reference frame names.  Added `SetPrismaticRevoluteFactors` to display in mm/degrees.
* Bug fixes:
  * Re-ordered function declaration in CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL for clang 9.0/MacOS
  * Fixed some variable initialization based on valgrind warnings

1.0.9 (2017-11-07)
==================

* API changes:
  * cisstMultiTask: mtsQtWidgetIntervalStatistics renamed mtsIntervalStatisticsQtWidget
  * cisstParameterTypes:
    * prmFixtureGainCartesianSet renamed prmCartesianImpedanceGains
    * changed reference/moving frame from pointer to strings for cartesian set/get payloads
  * CMake defaults:
    * cisstStereoVision off by default
    * cisstMultiTask ICE off by default
    * cisstJSON on by default
* Deprecated features:
  * None
* New features:
  * CMake: can now bypass the FindCisstNetlib provided with cisst.  Used to find local cisstNetlib config file.
  * cisstCommon:
    * cmnPath: added method SharedLibrary to generate dynamic library name based on OS
    * cmnStrings: added cmnStringsReplaceAll
    * XML: added method Exists(path) and fix for compilation with Qt5/Windows
  * cisstVector:
    * added SetSize method with default value for all elements
  * cisstDataGenerator:
    * added support to serialize/de-serialize from JSON
    * changed internal variable names to avoid possible conflicts with user provided names
  * cisstNumerical: CMake option to force using a local install with config.cmake file
  * cisstMultiTask:
    * added methods to configure component manager using JSON values
    * use cisstDataGenerator for component constructor arguments (allows JSON)
    * fixed incorrect error messages when optional interface/command was not found
    * added mtsMessage type and methods to emit status/warning/error events from provided interface
    * added mtsMessage, mtsIntervalStatistics and mtsSystem Qt Widget/Component
    * updated Qt widget to save space
  * cisstRobot: added wrapper for Reflexxes RML, builds Reflexxes as CMake External project
  * cisstParameterTypes:
    * added type prmInputData for DAQ
    * added Qt widget for prmStateJoint
    * actuator coupling message now only requires the actuator to joint position matrix
  * External projects: created https://github.com/jhu-cisst-external to host clones of Reflexxes, jsoncpp and cppunit
* Bug fixes:
  * cisstCommon: cmnRandomSequence, fixed ExtractRandomPermutation
  * Qt: updated OpenGL base widget when using Qt5 (now using QOpenGLWidget)
  * CMake: fixed search for QtXml when using Qt5

1.0.8 (2016-08-30)
==================

* API changes:
  * cisstDataGenerator: use std::vector instead of std::list for possible enum values/strings
* Deprecated features:
  * None
* New features:
  * cisstNumerical: nmrRegistrationRigid Python wrapper
* Bug fixes:
  * cisstVector: fixed bug in RowPointers methods for dynamic matrices
  * cisstOSAbstraction: fix serial port name on Windows when number greater than 9
  * CMake: fixed configure_file @ONLY
  * CMake: set messages as STATUS for informational messages
  * CMake: cisstStereoVision looks explicitly for OpenCV 2

1.0.7 (2016-05-16)
==================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * CMake: added -fPIC for external projects, now compiles with clang on Ubuntu 12.04 and 14.04
  * CMake: modifications to use find_package instead of cisst_find_saw_components
  * cisstDataGenerator:
    * Added methods to generate lists of enum values/strings
    * Destructor can now be virtual to avoid warnings if user adds virtual methods
  * cisstNumerical: added nmrLinearRegression
  * cisstRobot:
    * Added accessor to modify position offsets in robJoint
    * Now partially wrapped using SWIG
* Bug fixes:
  * cisstCommon: fix major bug in CMN_LOG macros (#45)
  * cisstCommon: cmnTypeTraits, added some missing specializations for unsigned long


1.0.6 (2016-01-08)
==================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * Added prmActuatorJointCoupling
  * mtsComponent: added GetIndexReader for default state table
* Bug fixes:
  * robManipulator: initialize data members Jn and Js
  * cmnAccessorMacros: added missing const on Get method


1.0.5 (2015-10-18)
==================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * Added prmStateJoint data type
  * cmnPath now checks if file exists using absolute path
* Bug fixes:
  * CMake: added FORCE_CISST_NETLIB_CONFIG to force use of CisstNetlibConfig.cmake (see 47379dbb16024726963d799b6a7a1c8a04097ff0)
  * CMake: now use github to download jsoncpp (see 9bd108a022585fdedf3bcccb0f1e0e37e3a1d62c)
  * cisstCommon: CMN_LOG use if statement instead of ?: to support recent compilers (see #34)
  * cisstOSAbstraction: osaSocket updated to support latest OSX (see #28, #33)
  * cisstVectorQt: translation widget always converts to mm for display (see 946cb0341ec01b284f1ef41567dc43ae1465db51)
  * cisstMultiTask: removed sleep on connect when manager is in standalone mode (i.e. not distributed)
  * cisstMultiTask python: add optional argument to reconnect for # of attempts to AddInterfaceRequiredAndConnect() (see bbb69bc7bbdb7add304f3f2f36dd226540aea5ee)
  * cisstNumerical: fixed nmrLSqLin, solver for underdetermined case (underconstrained) and added unit tests (see 8d736e626d8c8b8971699a7579787332ba24e796 and #32)


1.0.4 (2015-04-28)
==================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * More libraries compiled for Travis, now run some unit tests
* Bug fixes:
  * cisstVector: more types supported in Python
  * cisstMultiTask: more proxy declared for cisstVector types
  * Fixed propagation of CMake settings for cisstStereoVision
  * robManipulator: fixed normalization modulo 2 pi for rotations only, fixed memory allocation for under actuated robots
  * robLSPB: only require non-zero velocity/accel when joint is moving

1.0.3 (2015-02-19)
==================

* API changes:
  * None
* Deprecated features:
  * None
* New features:
  * Initial support to build using catkin/ROS (with catkin python build tools, doesn't work with catkin_make)
  * CMake option CISST_USE_SI_UNITS (advanced option, default is OFF), see cmnUnits.h.  Log settings in cisstLog.txt
  * Methods Ref for cisstVector vectors and matrices, both fixed size and dynamic
  * Added human readable cisst revision using git describe, see cisstRevision.h.in.  Log full version in cisstLog.txt
* Bug fixes:
  * Memory leaks in cmnCommandLineOptions, cmnXMLPath, vctPlot2D
  * Propagation of CMake flags -fPIC, -Wall, ... in cisstSettings.cmake to external projects
  * CMake for cisstCommon tests when XML is found
  * Shared directory improperly defined in cmnPath (was using major.minor.patch instead of major.minor)
  * Fixed some LaTeX math errors in documentation for Doxygen

1.0.2 (2015-01-02)
==================

* Change log file created
