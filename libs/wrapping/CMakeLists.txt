#
# $Id: CMakeLists.txt 8 2009-01-04 21:13:48Z adeguet1 $
#
# CMakeLists for wrapping
#
# (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---
#

PROJECT(cisstWrapping)

# SWIG macro has been updated to handle outdir correctly
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

# We define some new inline classes for the wrappers only
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Wrap each library
FOREACH(lib ${CISST_LIBRARIES})
  IF(BUILD_LIBS_${lib})
    IF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
      SUBDIRS(${lib})
    ENDIF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)


#
# $Log: CMakeLists.txt,v $
# Revision 1.24  2008/01/16 22:49:11  anton
# libs Python wrapping: Updated CMakeLists.txt to handle CMake 2.4.7 (now required
# when wrappers on compiled) and added useless copy to avoid bug added in 2.4.7.
#
# Revision 1.23  2007/04/26 19:33:58  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.22  2006/11/20 20:33:20  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.21  2005/09/06 13:41:09  anton
# wrapping CMakeLists.txt: Test to wrap only compiled libraries.  See ticket
# #170 (to be closed).
#
# Revision 1.20  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.19  2005/09/01 06:24:26  anton
# cisst wrapping:
# *: Reorganized files so that each library has its own CMakeLists.txt
# *: Added preliminary wrapping for some transformations.  This compiles, loads
# but is not tested.
#
# Revision 1.18  2005/08/19 05:29:14  anton
# wrapping CMakeLists: Added license.
#
# Revision 1.17  2005/06/17 23:04:46  anton
# cisst CMake code: Propagation of Python settings to tests and examples.
#
# Revision 1.16  2005/06/17 21:30:24  anton
# cisst CMake code:  Major update to ease the configuration of tests and
# examples.
#
# Revision 1.15  2005/06/15 15:06:20  anton
# Python wrapping: Split the cisstVector wrapping code in smaller files to
# increase readability.
#
# Revision 1.14  2005/05/04 13:25:46  anton
# TCL wrapping: Removed all the TCL wrapping code since it was only
# experimental, never worked and we don't have enough time to support it.
#
# Revision 1.13  2005/01/25 19:08:35  anton
# Python wrapping: Added -modern for SWIG (generates less code)
#
# Revision 1.12  2005/01/20 14:35:29  anton
# cisst Wrapping: Minor bug in CMakeLists.txt, missing " in COMMENT.
#
# Revision 1.11  2005/01/19 21:05:43  anton
# cisst wrapping:  Port to SWIG 1.3.24 (see ticket #122).  Mostly consists in
# removing the runtime specific options for the compilation.
#
# Revision 1.10  2005/01/18 22:17:08  anton
# Wrapping:  Updated CMakeLists.txt to correct issue raised in ticket #121.
# The files are not cleaned correctly, but this is a CMake bug.
#
# Revision 1.9  2005/01/07 20:01:06  anton
# Wrapping: Updated CMakelists.txt for take into account python_d (Windows)
# and force to compile the libraries as DLL (Windows, Linux seems to work
# fine with static libraries).
#
# Revision 1.8  2004/10/28 16:30:46  anton
# CMake:  Corrected a bug which prevented to clean all additional files.
#
# Revision 1.7  2004/10/28 04:49:37  anton
# wrapping: Added code for Tcl wrappers of cisstCommon and cisstVector.
# The %extend Get doesn't work yet (for embedding).
#
# Revision 1.6  2004/10/14 16:18:02  anton
# wrapping: changed the names of the modules from cisstXyz.i to
# cisstXyzPython.i to avoid dynamic loading issues with Windows/Python.
# Updated the __str__ method to use ToString().  For the register, used %extend
# to add the Get() method used in Python to get the right object.
#
# Revision 1.5  2004/10/08 22:53:29  anton
# Wrapping: I'm having a terrible time with ADD_CUSTOM_COMMAND between
# Linux and Windows but his version seems to work.
#
# Revision 1.4  2004/10/08 19:38:28  anton
# Wrapping: Error in install rule (* is not required)
#
# Revision 1.3  2004/10/07 18:29:27  anton
# SWIG Wrapping: Added a single SWIG runtime library used by all other SWIG
# Python modules.
#
# Revision 1.2  2004/08/31 14:08:31  anton
# Wrapping: After some discussions on the CMake mailing list, found a way to
# specify the output directory so that the .py files are directly generated
# in the same directory as the .so.
#
# Revision 1.1  2004/08/20 20:07:28  anton
# CMake Config: Update the config files to force the use of CMake 2.0.x or
# more (see #40) and cleaned-up the SWIG wrapping process thanks to the new
# CMake macros (see #55).
#
#
