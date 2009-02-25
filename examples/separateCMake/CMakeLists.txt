# See license at http://www.cisst.org/cisst/license.txt
# Find the cisst settings
FIND_FILE(CISST_CMAKE
          cisstBuild.cmake
          DOC "Location of your cisstBuild.cmake")

# Configure only if cisstBuild.cmake was found
IF(EXISTS ${CISST_CMAKE})
  INCLUDE("${CISST_CMAKE}")

  # Extend CMake Module Path to find cisst defined Macros
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIR})
  INCLUDE(cisstMacros) # To use CISST_REQUIRES

  # Add the include and lib paths for cisst
  INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR})
  LINK_DIRECTORIES(${CISST_LIBRARY_DIR})

  # Add a new library
  ADD_LIBRARY(newLibrary newLibrary.h newLibrary.cpp)

  # Add a new program
  ADD_EXECUTABLE(newProgram main.cpp)
  TARGET_LINK_LIBRARIES(newProgram newLibrary)

  # CISST_REQUIRES will check that the libraries are compiled
  # and set the correct link options
  CISST_REQUIRES("newProgram" "cisstCommon;cisstVector;cisstNumerical")

ELSE(EXISTS ${CISST_CMAKE})
  # Warn that we really need the cisst.cmake file
  MESSAGE("Can not configure unless CISST_CMAKE is provided")
ENDIF(EXISTS ${CISST_CMAKE})
