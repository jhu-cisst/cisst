#
# $Id$
#
# CMakeLists for cisstNumerical
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


# set the library name
SET(LIBRARY cisstNumerical)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector)

# all source files
SET(SOURCE_FILES
    nmrBernsteinPolynomial.cpp
    nmrBernsteinPolynomialLineIntegral.cpp
    nmrGaussJordanInverse.cpp
    nmrMultiIndexCounter.cpp
    nmrMultiVariablePowerBasis.cpp
    nmrPolynomialBase.cpp
    nmrPolynomialTermPowerIndex.cpp
    nmrSingleVariablePowerBasis.cpp
    nmrStandardPolynomial.cpp
    )


# all header files
SET(HEADER_FILES
    nmrBernsteinPolynomial.h
    nmrBernsteinPolynomialLineIntegral.h
    nmrDynAllocPolynomialContainer.h
    nmrExport.h
    nmrGaussJordanInverse.h
    nmrIsOrthonormal.h
    nmrMultiIndexCounter.h
    nmrMultiVariablePowerBasis.h
    nmrPolynomialBase.h
    nmrPolynomialContainer.h
    nmrPolynomialTermPowerIndex.h
    nmrSingleVariablePowerBasis.h
    nmrStandardPolynomial.h
    )

IF(CISST_HAS_CISSTNETLIB)
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      nmrLDPSolver.cpp
      nmrLSISolver.cpp
      nmrLSqLin.cpp
      nmrPInverse.cpp
      nmrPInverseEconomy.cpp
      nmrSVD.cpp
      nmrSVDEconomy.cpp
      )
  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrNetlib.h
      nmrInverse.h
      nmrLDPSolver.h
      nmrLSSolver.h
      nmrLSISolver.h
      nmrLSqLin.h
      nmrLU.h
      nmrLUSolver.h
      nmrHFTISolver.h
      nmrNNLSSolver.h
      nmrCallBack.h
      nmrPInverse.h
      nmrPInverseEconomy.h
      nmrSVD.h
      nmrSVDEconomy.h
      nmrSVDSolver.h
      nmrSVDRSSolver.h
      )
ENDIF(CISST_HAS_CISSTNETLIB)

# add files if cnetlib is used
# IF(CISST_HAS_CNETLIB)
#
#  SET(HEADER_FILES
#      ${HEADER_FILES}
#      nmrFnSolver.h
#      nmrFminSolver.h
#      nmrFnJacobianSolver.h
#      nmrLSEISolver.h
#      nmrLSNonLinSolver.h
#      nmrLSNonLinJacobianSolver.h
#      nmrPInverseSolver.h
#      )
#
#ENDIF(CISST_HAS_CNETLIB)


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Windows Dll require to link      
IF(CISST_HAS_CISSTNETLIB)
  TARGET_LINK_LIBRARIES(cisstNumerical
                        ${CISSTNETLIB_LIBRARIES}
                        ${CISSTNETLIB_F95_LIBRARIES}
                        ${CISSTNETLIB_GCC_LIBRARIES})
ENDIF(CISST_HAS_CISSTNETLIB)

