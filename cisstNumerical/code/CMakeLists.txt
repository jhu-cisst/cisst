#
# CMakeLists for cisstNumerical
#
# (C) Copyright 2003-2021 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstNumerical)

# all source files
set (SOURCE_FILES
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
set (HEADER_FILES
     nmrBernsteinPolynomial.h
     nmrBernsteinPolynomialLineIntegral.h
     nmrDynAllocPolynomialContainer.h
     nmrExport.h
     nmrGaussJordanInverse.h
     nmrIsOrthonormal.h
     nmrLinearRegression.h
     nmrMultiIndexCounter.h
     nmrMultiVariablePowerBasis.h
     nmrPolynomialBase.h
     nmrPolynomialContainer.h
     nmrPolynomialTermPowerIndex.h
     nmrSingleVariablePowerBasis.h
     nmrStandardPolynomial.h
     )

if (CISST_HAS_CISSTNETLIB)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       nmrConstraintOptimizer.cpp
       nmrInverseSPD.cpp
       nmrLSMinNorm.cpp
       nmrPInverse.cpp
       nmrPInverseEconomy.cpp
       nmrRegistrationRigid.cpp
       nmrSVD.cpp
       nmrSVDEconomy.cpp
       nmrLDPSolver.cpp
       # deprecated nmrLSISolver.cpp
       nmrLSqLin.cpp
       nmrSymmetricEigenProblem.cpp
       nmrSavitzkyGolay.cpp
      )
  set (HEADER_FILES
       ${HEADER_FILES}
       nmrNetlib.h
       nmrConstraintOptimizer.h
       nmrInverse.h
       nmrInverseSPD.h
       nmrLU.h
       nmrLSMinNorm.h
       # deprecated: nmrLUSolver.h
       nmrPInverse.h
       nmrPInverseEconomy.h
       nmrRegistrationRigid.h
       nmrSVD.h
       nmrSVDEconomy.h
       # deprecated nmrSVDSolver.h
       nmrLDPSolver.h
       nmrLSSolver.h
       # deprecated nmrLSISolver.h
       nmrLSEISolver.h
       nmrLSqLin.h
       nmrHFTISolver.h
       nmrNNLSSolver.h
       nmrCallBack.h
       nmrSVDRSSolver.h
       nmrSymmetricEigenProblem.h
       nmrSavitzkyGolay.h
       )
endif (CISST_HAS_CISSTNETLIB)

# Create the config file
set (CISST_NMR_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstNumerical/nmrConfig.h)
configure_file (${cisstNumericalLibs_SOURCE_DIR}/nmrConfig.h.in
                ${CISST_NMR_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_NMR_CONFIG_FILE}
         DESTINATION include/cisstNumerical
         COMPONENT cisstNumerical)
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CISST_NMR_CONFIG_FILE}")

# Add the config file to the project
set_source_files_properties ("${CISST_NMR_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_NMR_CONFIG_FILE})

# Finally, create main library
cisst_add_library (LIBRARY cisstNumerical
                   FOLDER cisstNumerical
                   DEPENDENCIES cisstCommon cisstVector
                   HEADER_FILES ${HEADER_FILES}
                   SOURCE_FILES ${SOURCE_FILES}
                   ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES})

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstNumericalBuild.cmake.in
                ${CISST_CMAKE_BINARY_DIR}/cisstNumericalBuild.cmake
                @ONLY)
install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstNumericalBuild.cmake
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         COMPONENT cisstNumerical)
