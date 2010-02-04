#
# $Id$
#
# CMakeLists for cisstNumerical
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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
#      lsi solver is included in nmrLSqLin
#      nmrLSISolver.cpp
      nmrLSqLin.cpp
      nmrPInverse.cpp
      nmrPInverseEconomy.cpp
      nmrRegistrationRigid.cpp
      nmrSVD.cpp
      nmrSVDEconomy.cpp
      )
  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrNetlib.h
      nmrInverse.h
      nmrLDPSolver.h
#     dgels solver is include in nmrLSqLin
#      nmrLSSolver.h
#     lsi solver is include in nmrLSqLin
#      nmrLSISolver.h
      nmrLSqLin.h
      nmrLU.h
#      nmrLUSolver.h
      nmrHFTISolver.h
      nmrNNLSSolver.h
      nmrCallBack.h
      nmrPInverse.h
      nmrPInverseEconomy.h
      nmrRegistrationRigid.h
      nmrSVD.h
      nmrSVDEconomy.h
#      nmrSVDSolver.h
      nmrSVDRSSolver.h
      )
ENDIF(CISST_HAS_CISSTNETLIB)


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)
