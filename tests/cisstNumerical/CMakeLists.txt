#
# $Id: CMakeLists.txt,v 1.31 2007/04/26 20:12:05 anton Exp $
#
# CMakeLists for cisstNumerical tests
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


# all source files
SET(SOURCE_FILES
    nmrBernsteinPolynomialTest.cpp
    nmrBernsteinPolynomialLineIntegralTest.cpp
    nmrDynAllocPolynomialContainerTest.cpp
    nmrGaussJordanInverseTest.cpp
    nmrIsOrthonormalTest.cpp
    nmrMultiIndexCounterTest.cpp
    nmrPolynomialBaseTest.cpp
    nmrPolynomialTermPowerIndexTest.cpp
    nmrStandardPolynomialTest.cpp
    )

# all header files
SET(HEADER_FILES
    nmrBernsteinPolynomialTest.h
    nmrBernsteinPolynomialLineIntegralTest.h
    nmrDynAllocPolynomialContainerTest.h
    nmrGaussJordanInverseTest.h
    nmrIsOrthonormalTest.h
    nmrMultiIndexCounterTest.h
    nmrPolynomialBaseTest.h
    nmrPolynomialTermPowerIndexTest.h
    nmrStandardPolynomialTest.h
    )

# Added tests available for all netlib distributions (cnetlib and cisstNetlib)
IF(CISST_HAS_NETLIB)
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      nmrInverseTest.cpp
      nmrLDPSolverTest.cpp
      nmrLSSolverTest.cpp
      nmrLSISolverTest.cpp
      nmrLSqLinTest.cpp
      nmrLUTest.cpp
      nmrLUSolverTest.cpp
      nmrNNLSSolverTest.cpp
      nmrPInverseTest.cpp
      nmrPInverseSolverTest.cpp
      nmrSVDTest.cpp
      nmrSVDSolverTest.cpp
      nmrSVDRSSolverTest.cpp
      )

  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrInverseTest.h
      nmrLDPSolverTest.h
      nmrLSSolverTest.h
      nmrLSISolverTest.h
      nmrLSqLinTest.h
      nmrLUTest.h
      nmrLUSolverTest.h
      nmrNNLSSolverTest.h
      nmrPInverseTest.h
      nmrPInverseSolverTest.h
      nmrSVDTest.h
      nmrSVDSolverTest.h
      nmrSVDRSSolverTest.h
      )

ENDIF(CISST_HAS_NETLIB)

# add files if cnetlib is used
IF(CISST_HAS_CNETLIB)
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      nmrLSEISolverTest.cpp
      nmrNonLinSolverTest.cpp
      )

  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrLSEISolverTest.h
      nmrNonLinSolverTest.h
      )

ENDIF(CISST_HAS_CNETLIB)

# Add executable for C++ tests
ADD_EXECUTABLE(cisstNumericalTests ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(cisstNumericalTests cisstTestsMain)
CISST_REQUIRES("cisstNumericalTests" "cisstCommon;cisstVector;cisstNumerical")

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstNumericalTests)


IF(CISST_HAS_CNETLIB)
  TARGET_LINK_LIBRARIES(cisstNumericalTests ${CNETLIB_LIBRARIES})
ENDIF(CISST_HAS_CNETLIB)

IF(CISST_HAS_CISSTNETLIB)
  TARGET_LINK_LIBRARIES(cisstNumericalTests
                        ${CISSTNETLIB_LIBRARIES}
                        ${CISSTNETLIB_F95_LIBRARIES}
                        ${CISSTNETLIB_GCC_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})
  # If cisstOSAbstraction is compiled, compile multithreaded tests
  IF(BUILD_LIBS_cisstOSAbstraction)
    ADD_DEFINITIONS(-DBUILD_MULTITHREADED_TESTS)
    CISST_REQUIRES("cisstNumericalTests" "cisstOSAbstraction")
    TARGET_LINK_LIBRARIES(cisstNumericalTests
                          ${CMAKE_THREAD_LIBS_INIT})
  ENDIF(BUILD_LIBS_cisstOSAbstraction)
ENDIF(CISST_HAS_CISSTNETLIB)


#
# $Log: CMakeLists.txt,v $
# Revision 1.31  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.30  2006/11/20 20:33:52  anton
# Licensing: Applied new license to tests.
#
# Revision 1.29  2006/01/28 06:16:25  anton
# cisstNumerical: Added nmrInverse (see #207).
#
# Revision 1.28  2006/01/11 20:49:18  anton
# cisstNumerical CMake: Added a variable to indicate that either netlib is
# available and factor most of the CMake code (see CISST_HAS_NETLIB).
#
# Revision 1.27  2006/01/10 23:25:33  anton
# cisstNumerical: Incomplete but somewhat working version of nmrLU.
#
# Revision 1.26  2006/01/05 03:54:40  anton
# cisstNumerical tests: Compile multithreaded tests only if cisstOSAbstraction
# has been compiled instead of requiring cisstOSAbstraction when cisstNetlib
# is used.  See ticket #200.
#
# Revision 1.25  2006/01/04 22:51:54  ofri
# Added tests for the nmrGaussJordanInverseNxN to the cisstNumerical test
# suite.
#
# Revision 1.24  2005/11/30 16:38:50  anton
# CMakeLists: Added support for libcisstNetlibGCC on Windows.
#
# Revision 1.23  2005/11/30 14:54:52  anton
# CMakeLists: Use CMake thread functions to use native threads.
#
# Revision 1.22  2005/11/30 06:03:32  kapoor
# cisstNumerical : Added LSqLin tests when cisstNetlib is used.
#
# Revision 1.21  2005/11/29 03:09:31  anton
# cisstNumerical tests CMakeLists.txt: Added more tests when cisstNetlib is used.
#
# Revision 1.20  2005/11/28 22:04:44  kapoor
# cisstNumerical: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
#
# Revision 1.19  2005/11/20 21:35:18  kapoor
# LAPACK: Added CISST_HAS_CISSTCNETLIB flag to cmake, which can be used to
# compile code using the newer LAPACK3E. CLAPACK is still supported. See
# ticket #193.
#
# Revision 1.18  2005/10/21 23:14:31  anton
# nmrSVD: Added tests from Ankur.
#
# Revision 1.17  2005/09/06 15:41:37  anton
# cisstNumerical tests: Added license.
#
# Revision 1.16  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.15  2005/08/17 20:05:01  kapoor
# cisstNumerical: Added LSEI (a more recent version of Hansons' Code)
#
# Revision 1.14  2005/08/04 19:25:46  anton
# cisstNumericalTest: Removed double nmrLSISolverTest.h from header list.
#
# Revision 1.13  2005/08/04 18:56:44  anton
# cisstNumericalTests: Added nmrLUSolverTest.
#
# Revision 1.12  2005/07/29 00:02:18  kapoor
# Numerical Test: Some simple sanity checks for Least Squares (LS),
# pseudo-inverse and SVD with RHS matrix. Tests compare a matrix with
# results from MATLAB(r).
#
# Revision 1.11  2005/07/27 22:40:33  anton
# cisstNumerical tests: Added some basic tests for nmrIsOthonormal.
#
# Revision 1.10  2005/07/22 22:30:10  anton
# cisstNumerical Tests: Added basic test for nmrSVDSolver.
#
# Revision 1.9  2005/06/17 21:32:01  anton
# cisst CMake for tests: Major update to ease configuration.
#
# Revision 1.8  2005/06/03 01:22:03  anton
# Tests: Preliminary support for Dart2.
#
# Revision 1.7  2004/11/30 22:44:24  anton
# cisstNumerical tests: Renamed executable from cisstNumericalTest to
# cisstNumericalTests to be consistent with other test programs.
#
# Revision 1.6  2004/11/03 22:28:34  anton
# cisstNumerical Tests: Added sanity check tests for LDP and NNLS.  Updated
# other tests to use VCT_COL_MAJOR.
#
# Revision 1.5  2004/11/02 17:02:04  anton
# cisstNumerical Tests: Added a sanity check based on data from the Lawson
# and Hanson book and result from Matlab.
#
# Revision 1.4  2004/10/27 17:57:21  ofri
# cisstNumericalTest: Following ticket #82, I erased the use of valarray, and
# instead I am using vctDynamicVector.  Huh!
#
# Revision 1.3  2004/10/24 03:51:03  ofri
# Renamed cisstNumerical test classes and files.  Tests run but fail for some
# cases.
#
# Revision 1.2  2004/10/22 02:53:52  ofri
# Cleanup nmrMultiIndexCounterTest and add it to the test suite.
#
# Revision 1.1  2004/10/21 20:34:24  ofri
# I Added the test modules for the polynomials in cisstNumerical to the
# repository.  The current CMake build is empty. The reason is that some renaming
# and adaptation are required to match the cisst convention.  This will be done
# over time.
# See ticket #78.
#
#
