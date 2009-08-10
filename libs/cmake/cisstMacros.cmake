#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2004-01-22
#
# (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# The macro adds a library to a CISST-related project by processing the 
# externally defined variables listed below:
#
# - LIBRARY is the name of the library, e.g. cisstVector
# - DEPENDENCIES is a list of dependencies, for cisstVector, set it to cisstCommon
# - SOURCE_FILES is a list of files, without any path (absolute or relative)
# - HEADER_FILES is a list of files, without any path (absolute or relative)
# - ADDITIONAL_SOURCE_FILES is a list of source files with a full path (e.g. generated source)
# - ADDITIONAL_HEADER_FILES is a list of header files with a full path (e.g. configured/generated header)
#
# Invoke this macro from within a library's CMakeLists.txt to add that library
# to a larger project.  The name of the project is given as a macro argument.
#

# The macro performs the following:
# -- create the source and header lists of files with the right path
# -- check the dependencies
# -- add the link options based on the dependencies
# -- add the library 
# -- create the install targets for the headers as well as the library

MACRO(CISST_ADD_LIBRARY_TO_PROJECT PROJECT_NAME)

# Make sure this lib should be compiled
IF(BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

  # Build source list with full path
  FOREACH(file ${SOURCE_FILES})
    SET(SOURCES ${SOURCES} ${${PROJECT_NAME}_SOURCE_DIR}/code/${LIBRARY}/${file})
  ENDFOREACH(file)


  # Build header list with full path and generate a main header file for the library
  STRING(ASCII 35 CISST_STRING_POUND)
  SET(LIBRARY_MAIN_HEADER ${${PROJECT_NAME}_BINARY_DIR}/include/${LIBRARY}.h)
  SET(LIBRARY_MAIN_HEADER_TMP ${${PROJECT_NAME}_BINARY_DIR}/include/${LIBRARY}.h.tmp)

  SET(FILE_CONTENT "/* This file is generated automatically by CMake, DO NOT EDIT\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "   CMake: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "   System: ${CMAKE_SYSTEM}\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "   Source: ${CMAKE_SOURCE_DIR} */\n\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}ifndef _${LIBRARY}_h\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}define _${LIBRARY}_h\n\n")
  FOREACH(file ${HEADER_FILES})
    SET(HEADERS ${HEADERS} ${${PROJECT_NAME}_SOURCE_DIR}/include/${LIBRARY}/${file})
    SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}include <${LIBRARY}/${file}>\n")
  ENDFOREACH(file)
  SET(FILE_CONTENT ${FILE_CONTENT} "\n${CISST_STRING_POUND}endif // _${LIBRARY}_h\n")
  FILE(WRITE ${LIBRARY_MAIN_HEADER_TMP} ${FILE_CONTENT})

  EXEC_PROGRAM(${CMAKE_COMMAND}
               ARGS -E copy_if_different
               \"${LIBRARY_MAIN_HEADER_TMP}\"
               \"${LIBRARY_MAIN_HEADER}\") 

  EXEC_PROGRAM(${CMAKE_COMMAND}
               ARGS -E remove
               \"${LIBRARY_MAIN_HEADER_TMP}\")

  # Add the main header to the library, for IDEs
  SET(HEADERS ${HEADERS} ${LIBRARY_MAIN_HEADER})

  # Use the additional include path
  INCLUDE_DIRECTORIES(${CISST_ADDITIONAL_INCLUDE_DIRECTORIES})

  # Add the library
  ADD_LIBRARY(${LIBRARY}
              ${IS_SHARED}
              ${SOURCES}
              ${ADDITIONAL_SOURCE_FILES}
              ${HEADERS}
              ${ADDITIONAL_HEADER_FILES}
              )
  INSTALL_TARGETS(/lib ${LIBRARY})

  # Add dependencies for linking, also check BUILD_xxx for dependencies
  IF(DEPENDENCIES)
    # Check that dependencies are build
    FOREACH(dependency ${DEPENDENCIES})
      SET(BUILD_DEPENDENCIES ${BUILD_DEPENDENCIES} BUILD_LIBS_${dependency})
    ENDFOREACH(dependency)
    VARIABLE_REQUIRES(BUILD_LIBS_${LIBRARY} BUILD_LIBS_${LIBRARY} ${BUILD_DEPENDENCIES})
    # Set the link flags
    TARGET_LINK_LIBRARIES(${LIBRARY} ${DEPENDENCIES})
    # Keep a trace of dependencies for main CMake level
    SET(${LIBRARY}_DEPENDENCIES "${DEPENDENCIES}" CACHE STRING "Required libraries for ${LIBRARY}" FORCE)
    MARK_AS_ADVANCED(${LIBRARY}_DEPENDENCIES)
  ENDIF(DEPENDENCIES)

  # Link to cisst additional libraries
  TARGET_LINK_LIBRARIES(${LIBRARY} ${CISST_ADDITIONAL_LIBRARIES})

  # Install all header files
  INSTALL_FILES(/include/${LIBRARY}
                ".h"
                ${HEADERS})
  INSTALL_FILES(/include/
                ".h"
                ${LIBRARY_MAIN_HEADER})

ENDIF(BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

ENDMACRO(CISST_ADD_LIBRARY_TO_PROJECT)



# Macro used to compare required libraries for a given target with
# libraries actually compiled.  This macro adds the required link
# options.
MACRO(CISST_REQUIRES WHO_REQUIRES REQUIRED_CISST_LIBRARIES)

   IF(CISST_BUILD_SHARED_LIBS)
     ADD_DEFINITIONS(-DCISST_DLL)
   ENDIF(CISST_BUILD_SHARED_LIBS)

   # First test that all libraries should have been compiled
   FOREACH(required ${REQUIRED_CISST_LIBRARIES})
     IF("${CISST_LIBRARIES}"  MATCHES ${required})
     ELSE("${CISST_LIBRARIES}"  MATCHES ${required})     
       MESSAGE("${WHO_REQUIRES} requires ${required} which doesn't exist or hasn't been compiled")
     ENDIF("${CISST_LIBRARIES}"  MATCHES ${required})
   ENDFOREACH(required)

   # Second, create a list of libraries in the right order
   FOREACH(existing ${CISST_LIBRARIES})
     IF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
       SET(CISST_LIBRARIES_TO_USE ${CISST_LIBRARIES_TO_USE} ${existing})
     ENDIF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
   ENDFOREACH(existing)

   # Finally, link with the required libraries
   TARGET_LINK_LIBRARIES(${WHO_REQUIRES} ${CISST_LIBRARIES_TO_USE} ${CISST_ADDITIONAL_LIBRARIES})

ENDMACRO(CISST_REQUIRES)



# Macro to add all the available tests
MACRO(CISST_ADD_TESTS testProgram)
  # Once the test program is compiled, run it to create a list of available tests
  ADD_CUSTOM_COMMAND(TARGET ${testProgram}
                     POST_BUILD
                     COMMAND ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${testProgram}
                     ARGS -d > ${CMAKE_CURRENT_BINARY_DIR}/DartTestfile-${testProgram}.txt
                     COMMENT "Generating DartTestfile-${testProgram}.txt")

  # Add the custom build list
  SET_DIRECTORY_PROPERTIES(PROPERTIES TEST_INCLUDE_FILE
                           "${CMAKE_CURRENT_BINARY_DIR}/DartTestfile-${testProgram}.txt")
ENDMACRO(CISST_ADD_TESTS)

