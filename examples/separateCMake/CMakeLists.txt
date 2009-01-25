# See license at http://www.cisst.org/cisst/license.txt
# Find the cisst settings
FIND_FILE(CISST_CMAKE
          cisstBuild.cmake
          DOC "Location of your cisstBuild.cmake")

# Configure only if cisstBuild.cmake was found
IF(EXISTS ${CISST_CMAKE})
  INCLUDE("${CISST_CMAKE}")

  # Add the include and lib paths for cisst
  INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR})
  LINK_DIRECTORIES(${CISST_LIBRARY_DIR})

  # For Microsoft compilers, see if the libraries are DLLs
  # Useless and harmless with other compilers
  IF(CISST_BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DCISST_DLL)
  ENDIF(CISST_BUILD_SHARED_LIBS)

  # Add a new library
  ADD_LIBRARY(newLibrary newLibrary.h newLibrary.cpp)

  # Add a new program
  ADD_EXECUTABLE(newProgram main.cpp)
  TARGET_LINK_LIBRARIES(newProgram newLibrary)

  # CISST_REQUIRES will check that the libraries are compiled
  # and set the correct link options
  CISST_REQUIRES("newProgram" "cisstCommon;cisstVector;cisstNumerical")

  # This example uses cisstNetlib therefore we need the libraries
  INCLUDE_DIRECTORIES(${CISSTNETLIB_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(newProgram ${CISSTNETLIB_LIBRARIES})

ELSE(EXISTS ${CISST_CMAKE})
  # Warn that we really need the cisst.cmake file
  MESSAGE("Can not configure unless CISST_CMAKE is provided")
ENDIF(EXISTS ${CISST_CMAKE})
