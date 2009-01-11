#
# $Id: CMakeLists.txt 8 2009-01-04 21:13:48Z adeguet1 $
#
# CMakeLists for wrapping tests
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


CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisstWrappingTests)


# Check what seems available
FOREACH(lib ${CISST_LIBRARIES})

  # Make sure there is actually a directory of test case
  IF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})
    SUBDIRS(${lib})
  ENDIF(EXISTS ${cisstWrappingTests_SOURCE_DIR}/${lib})

ENDFOREACH(lib)


#
# $Log: CMakeLists.txt,v $
# Revision 1.10  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.9  2006/11/20 20:33:53  anton
# Licensing: Applied new license to tests.
#
# Revision 1.8  2005/12/21 21:03:17  anton
# CMake: Moved find package _PythonInterp_ to top level so that PYTHON_EXECUTABLE
# is defined in all sub directories.
#
# Revision 1.7  2005/12/07 23:11:48  anton
# Python tests: Use python executable to start tests (see #198).
#
# Revision 1.6  2005/07/14 20:55:09  anton
# wrapping tests: Added code to define PYTHON_EXECUTABLE and use it for
# CTest.  This is required for Windows and should be safer for Unix.
#
# Revision 1.5  2005/06/23 15:19:04  anton
# cisstVector wrapping tests: Added the possibility to wrap some C++ classes
# used for further Python testing.  This is useful to test the typemaps which
# convert fixed size vectors back and forth to dynamic vectors.
#
# Revision 1.4  2005/06/17 21:32:01  anton
# cisst CMake for tests: Major update to ease configuration.
#
# Revision 1.3  2005/06/14 17:38:42  anton
# Python wrapping tests: Major update based on Andy LaMora's work to integrate
# the Python tests with Dart2
#
# Revision 1.2  2005/04/09 03:48:03  anton
# tests wrapping: Bug in CMakeLists.txt, SET(LIB ${lib}) was missing.
#
# Revision 1.1  2005/04/08 16:37:24  anton
# Tests for Python wrapped libraries, creation.
#
#
