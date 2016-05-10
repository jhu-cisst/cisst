#
# CMakeLists for cisstNumerical tests
#
# (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights
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
set (SOURCE_FILES
     nmrBernsteinPolynomialTest.cpp
     nmrBernsteinPolynomialLineIntegralTest.cpp
     nmrDynAllocPolynomialContainerTest.cpp
     nmrGaussJordanInverseTest.cpp
     nmrLinearRegressionTest.cpp
     nmrMultiIndexCounterTest.cpp
     nmrPolynomialBaseTest.cpp
     nmrPolynomialTermPowerIndexTest.cpp
     nmrStandardPolynomialTest.cpp
     )

# all header files
set (HEADER_FILES
     nmrBernsteinPolynomialTest.h
     nmrBernsteinPolynomialLineIntegralTest.h
     nmrDynAllocPolynomialContainerTest.h
     nmrGaussJordanInverseTest.h
     nmrLinearRegressionTest.h
     nmrMultiIndexCounterTest.h
     nmrPolynomialBaseTest.h
     nmrPolynomialTermPowerIndexTest.h
     nmrStandardPolynomialTest.h
     )

# Added tests available for cisstNetlib
if (CISST_HAS_CISSTNETLIB)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       nmrIncludesTest.cpp
       nmrConstraintOptimizerTest.cpp
       nmrInverseTest.cpp
       nmrIsOrthonormalTest.cpp
       nmrLUTest.cpp
       # deprecated nmrLUSolverTest.cpp
       nmrPInverseTest.cpp
       # deprecated nmrPInverseSolverTest.cpp
       nmrSVDTest.cpp
       # deprecated nmrSVDSolverTest.cpp
       nmrLDPSolverTest.cpp
       nmrLSSolverTest.cpp
       # deprecated nmrLSISolverTest.cpp
       nmrLSqLinTest.cpp
       nmrNNLSSolverTest.cpp
       nmrSVDRSSolverTest.cpp
       )

  set (HEADER_FILES
       ${HEADER_FILES}
       nmrIncludesTest.h
       nmrConstraintOptimizerTest.h
       nmrInverseTest.h
       nmrIsOrthonormalTest.h
       nmrLUTest.h
       # deprecated nmrLUSolverTest.h
       nmrPInverseTest.h
       nmrPInverseSolverTest.h
       nmrSVDTest.h
       # deprecated nmrSVDSolverTest.h
       nmrLDPSolverTest.h
       nmrLSSolverTest.h
       # deprecated nmrLSISolverTest.h
       nmrLSqLinTest.h
       nmrNNLSSolverTest.h
       # deprecated nmrSVDRSSolverTest.h
       )
endif (CISST_HAS_CISSTNETLIB)

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

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstVector cisstOSAbstraction cisstNumerical cisstTestsDriver)

# Add executable for C++ tests
add_executable (cisstNumericalTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstNumericalTests PROPERTY FOLDER "cisstNumerical/tests")
target_link_libraries (cisstNumericalTests cisstTestsDriver)

# If cisstOSAbstraction is compiled, compile multithreaded tests
# and link differently
if (CISST_cisstOSAbstraction)
  add_definitions (-DBUILD_MULTITHREADED_TESTS)
  cisst_target_link_libraries (cisstNumericalTests cisstCommon cisstVector cisstNumerical cisstOSAbstraction cisstTestsDriver)
  target_link_libraries (cisstNumericalTests
                         ${CMAKE_THREAD_LIBS_INIT})
else (CISST_cisstOSAbstraction)
  cisst_target_link_libraries (cisstNumericalTests cisstCommon cisstVector cisstNumerical cisstTestsDriver)
endif (CISST_cisstOSAbstraction)

# To generate a CTest list of tests
cisst_add_test (cisstNumericalTests ITERATIONS 10 INSTANCES 2)
