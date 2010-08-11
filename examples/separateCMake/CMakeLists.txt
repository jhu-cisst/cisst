# See license at http://www.cisst.org/cisst/license.txt
# Find the cisst settings

project (myProject)
cmake_minimum_required (VERSION 2.6)

find_package (cisst REQUIRED NO_MODULE)

if (cisst_FOUND)
  # Modify CMake configuration to use cisst
  include (${CISST_USE_FILE})

  # Add a new library
  add_library (newLibrary newLibrary.h newLibrary.cpp)

  # Add a new program and link against the new library
  add_executable (newProgram main.cpp)
  target_link_libraries (newProgram newLibrary)

  # cisst_target_link_libraries will check that the libraries are compiled
  # and set the correct link options
  cisst_target_link_libraries (newProgram cisstCommon cisstVector cisstNumerical)
endif (cisst_FOUND)
