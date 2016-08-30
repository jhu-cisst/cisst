Change log
==========

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
