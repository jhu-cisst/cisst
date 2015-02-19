Change log
==========

1.0.3 (2015-xx-xx)
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
