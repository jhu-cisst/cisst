#
# $Id$
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
    nmrMultiIndexCounterTest.h
    nmrPolynomialBaseTest.h
    nmrPolynomialTermPowerIndexTest.h
    nmrStandardPolynomialTest.h
    )

# Added tests available for cisstNetlib
IF(CISST_HAS_CISSTNETLIB)
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      nmrInverseTest.cpp
      nmrIsOrthonormalTest.cpp
      nmrLDPSolverTest.cpp
#      nmrLSSolverTest.cpp
#      nmrLSISolverTest.cpp
      nmrLSqLinTest.cpp
      nmrLUTest.cpp
#      nmrLUSolverTest.cpp
      nmrNNLSSolverTest.cpp
      nmrPInverseTest.cpp
#      nmrPInverseSolverTest.cpp
      nmrSVDTest.cpp
#      nmrSVDSolverTest.cpp
      nmrSVDRSSolverTest.cpp
      )

  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrInverseTest.h
      nmrIsOrthonormalTest.h
      nmrLDPSolverTest.h
#      nmrLSSolverTest.h
#      nmrLSISolverTest.h
      nmrLSqLinTest.h
      nmrLUTest.h
#      nmrLUSolverTest.h
      nmrNNLSSolverTest.h
      nmrPInverseTest.h
#      nmrPInverseSolverTest.h
      nmrSVDTest.h
#      nmrSVDSolverTest.h
      nmrSVDRSSolverTest.h
      )

ENDIF(CISST_HAS_CISSTNETLIB)

# add files if cnetlib is used
# IF(CISST_HAS_CNETLIB)
#  SET(SOURCE_FILES
#      ${SOURCE_FILES}
#      nmrLSEISolverTest.cpp
#      nmrNonLinSolverTest.cpp
#      )

#  SET(HEADER_FILES
#      ${HEADER_FILES}
#      nmrLSEISolverTest.h
#      nmrNonLinSolverTest.h
#      )
#
# ENDIF(CISST_HAS_CNETLIB)

# Add executable for C++ tests
ADD_EXECUTABLE(cisstNumericalTests ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(cisstNumericalTests cisstTestsMain)

# If cisstOSAbstraction is compiled, compile multithreaded tests
# and link differently
IF(BUILD_LIBS_cisstOSAbstraction)
  ADD_DEFINITIONS(-DBUILD_MULTITHREADED_TESTS)
  CISST_REQUIRES("cisstNumericalTests" "cisstCommon;cisstVector;cisstNumerical;cisstOSAbstraction")
  TARGET_LINK_LIBRARIES(cisstNumericalTests
                        ${CMAKE_THREAD_LIBS_INIT})
ELSE(BUILD_LIBS_cisstOSAbstraction)
  CISST_REQUIRES("cisstNumericalTests" "cisstCommon;cisstVector;cisstNumerical")
ENDIF(BUILD_LIBS_cisstOSAbstraction)

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstNumericalTests)
