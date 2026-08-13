## -*- Mode: CMAKE; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

#
# Author(s):  Anton Deguet
# Created on: 2004-01-22
#
# (C) Copyright 2004-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

include(CMakePackageConfigHelpers)

# function used to determine if some extra configuration messages
# should be displayed
function (cisst_cmake_debug ...)
  if (CISST_HAS_CMAKE_DEBUG)
    message ("cisst CMake debug: ${ARGV}")
  endif (CISST_HAS_CMAKE_DEBUG)
endfunction (cisst_cmake_debug)


# The macro adds a library to a CISST-related project by processing the
# following parameters
#
# - PROJECT (cisst by default)
# - FOLDER empty by default, used only to organize projects in IDE when supported by CMake
# - LIBRARY is the name of the library, e.g. cisstVector
# - LIBRARY_DIR, by default uses ${LIBRARY}, can be specified for special cases (e.g. cisstCommonQt)
# - DEPENDENCIES is a list of dependencies, for cisstVector, set it to cisstCommon
# - SOURCE_FILES is a list of files, without any path (absolute or relative)
# - HEADER_FILES is a list of files, without any path (absolute or relative)
# - ADDITIONAL_SOURCE_FILES is a list of source files with a full path (e.g. generated source)
# - ADDITIONAL_HEADER_FILES is a list of header files with a full path (e.g. configured/generated header)
#
# The macro performs the following:
# -- create the source and header lists of files with the right path
# -- check the dependencies
# -- add the link options based on the dependencies
# -- add the library
# -- create the install targets for the headers as well as the library

macro (cisst_add_library ...)
  # debug
  cisst_cmake_debug ("cisst_add_library called with: ${ARGV}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       LIBRARY
       LIBRARY_DIR
       PROJECT
       FOLDER
       DEPENDENCIES
       SOURCE_FILES
       HEADER_FILES
       ADDITIONAL_SOURCE_FILES
       ADDITIONAL_HEADER_FILES
       ADDITIONAL_HEADER_FILES_RELATIVE)

  # reset local variables
  foreach(keyword ${FUNCTION_KEYWORDS})
    set (${keyword} "")
  endforeach(keyword)

  # parse input
  foreach (arg ${ARGV})
    list (FIND FUNCTION_KEYWORDS ${arg} ARGUMENT_IS_A_KEYWORD)
    if (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (CURRENT_PARAMETER ${arg})
      set (${CURRENT_PARAMETER} "")
    else (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (${CURRENT_PARAMETER} ${${CURRENT_PARAMETER}} ${arg})
    endif (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
  endforeach (arg)

  # fill defaults
  if (PROJECT STREQUAL "")
    set (PROJECT "cisst")
  endif (PROJECT STREQUAL "")
  if (LIBRARY_DIR STREQUAL "")
    set (LIBRARY_DIR ${LIBRARY})
  endif (LIBRARY_DIR STREQUAL "")

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_add_library: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  # Build header list with full path and generate a main header file for the library
  set (HEADERS "")
  string (ASCII 35 CISST_STRING_POUND)
  set (LIBRARY_MAIN_HEADER ${${PROJECT}_BINARY_DIR}/include/${LIBRARY}.h)

  set (FILE_CONTENT "/* This file is generated automatically by CMake, DO NOT EDIT\n")
  set (FILE_CONTENT ${FILE_CONTENT} "   CMake: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}\n")
  set (FILE_CONTENT ${FILE_CONTENT} "   System: ${CMAKE_SYSTEM}\n")
  set (FILE_CONTENT ${FILE_CONTENT} "   Source: ${CMAKE_SOURCE_DIR} */\n\n")
  set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}ifndef _${LIBRARY}_h\n")
  set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}define _${LIBRARY}_h\n\n")
  foreach (file ${HEADER_FILES})
    set (HEADERS ${HEADERS} ${${PROJECT}_SOURCE_DIR}/${LIBRARY_DIR}/${file})
    set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}include <${LIBRARY_DIR}/${file}>\n")
  endforeach (file)
  foreach (file ${ADDITIONAL_HEADER_FILES_RELATIVE})
    set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}include <${LIBRARY_DIR}/${file}>\n")
  endforeach (file)

  set (FILE_CONTENT ${FILE_CONTENT} "\n${CISST_STRING_POUND}endif // _${LIBRARY}_h\n")
  file (WRITE ${LIBRARY_MAIN_HEADER} ${FILE_CONTENT})
  get_directory_property (existing_files_to_clean ADDITIONAL_MAKE_CLEAN_FILES)
  set_directory_properties (PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES
    "${existing_files_to_clean};${LIBRARY_MAIN_HEADER}")

  # Add the library
  list (SORT SOURCE_FILES)
  list (SORT HEADERS)
  list (SORT ADDITIONAL_SOURCE_FILES)
  list (SORT ADDITIONAL_HEADER_FILES)
  cisst_cmake_debug ("cisst_add_library: Adding library ${LIBRARY} using files ${SOURCE_FILES} ${HEADERS}")

  add_library (${LIBRARY}
               ${IS_SHARED}
               ${SOURCE_FILES}
               ${ADDITIONAL_SOURCE_FILES}
               ${LIBRARY_MAIN_HEADER}
               ${HEADERS}
               ${ADDITIONAL_HEADER_FILES}
               )

  # set version number
  set_target_properties (${LIBRARY} PROPERTIES
                         VERSION ${cisst_VERSION}
                         SOVERSION ${cisst_VERSION})

  target_include_directories (${LIBRARY} PUBLIC
    $<BUILD_INTERFACE:${cisst_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${cisst_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

  # Make sure this is defined for all compiled symbols, this allows proper association of symbols/library name
  target_compile_definitions (${LIBRARY} PRIVATE
    LIBRARY_NAME_FOR_CISST_REGISTER="${LIBRARY}")

  # Install the library
  install (TARGETS ${LIBRARY}
           EXPORT cisst-targets
           COMPONENT ${LIBRARY}
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib
           INCLUDES DESTINATION include)

  # Add dependencies for linking, also check BUILD_xxx for dependencies
  if (DEPENDENCIES)
    # Check that dependencies are built
    foreach (dependency ${DEPENDENCIES})
      list (FIND CISST_LIBRARIES ${dependency} FOUND_IT)
      if (${FOUND_IT} EQUAL -1)
        # not found
        message (SEND_ERROR "${LIBRARY} requires ${dependency} which doesn't exist or hasn't been compiled (available libraries: ${CISST_LIBRARIES}")
      endif (${FOUND_IT} EQUAL -1)
    endforeach (dependency)
    # Set the link flags
    target_link_libraries (${LIBRARY} ${DEPENDENCIES})
    cisst_cmake_debug ("cisst_add_library: Library ${LIBRARY} links against: ${DEPENDENCIES}")
  endif (DEPENDENCIES)

  # Install all header files
  install (FILES ${HEADERS}
           DESTINATION include/${LIBRARY_DIR}
           COMPONENT ${LIBRARY}-dev)
  install (FILES ${LIBRARY_MAIN_HEADER}
           DESTINATION include
           COMPONENT ${LIBRARY}-dev)

  # if a folder has been provided
  if (FOLDER)
    set_property (TARGET ${LIBRARY} PROPERTY FOLDER "${FOLDER}")
  endif (FOLDER)

endmacro (cisst_add_library)


# Macro used to compare required libraries for a given target with
# libraries actually compiled.  This macro adds the required link
# options.
macro (cisst_target_link_libraries TARGET ...)
  message (FATAL_ERROR "cisst_target_link_libraries is deprecated. Use CMake target_link_libraries with cisst imported targets or \${cisst_LIBRARIES}.")
endmacro (cisst_target_link_libraries)


# The function adds a SWIG module to a CISST-related project by processing the
# following parameters
#
# - MODULE is the prefix of the main .i file.  The module name will be <MODULE>Python
# - INTERFACE_FILENAME is the filename of the .i file (if not specified, defaults to <MODULE>.i)
# - INTERFACE_DIRECTORY is the directory containing the .i file (use relative path from current source dir)
# - FOLDER is used for IDE that support the CMake target property FOLDER
# - MODULE_LINK_LIBRARIES cisst libraries needed to link the module (can be used for other libraries as long as CMake can find them)
#
function (cisst_add_swig_module ...)
  # debug
  cisst_cmake_debug ("cisst_add_swig_module called with: ${ARGV}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       MODULE
       INTERFACE_FILENAME
       INTERFACE_DIRECTORY
       FOLDER
       HEADER_FILES
       MODULE_LINK_LIBRARIES
       INSTALL_FILES)

  # reset local variables
  foreach (keyword ${FUNCTION_KEYWORDS})
    set (${keyword} "")
  endforeach (keyword)

  # parse input
  foreach (arg ${ARGV})
    list (FIND FUNCTION_KEYWORDS ${arg} ARGUMENT_IS_A_KEYWORD)
    if (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (CURRENT_PARAMETER ${arg})
      set (${CURRENT_PARAMETER} "")
    else (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (${CURRENT_PARAMETER} ${${CURRENT_PARAMETER}} ${arg})
    endif (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
  endforeach (arg)

  # set default interface file name
  if (NOT INTERFACE_FILENAME)
    set (INTERFACE_FILENAME "${MODULE}.i")
  endif (NOT INTERFACE_FILENAME)

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_add_swig_module: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  # interface file
  set (SWIG_INTERFACE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE_DIRECTORY}/${INTERFACE_FILENAME}")
  cisst_cmake_debug ("cisst_add_swig_module: looking for interface file ${SWIG_INTERFACE_FILE}")

  if (EXISTS ${SWIG_INTERFACE_FILE})
    if (SWIG_USE_FILE)
      include (${SWIG_USE_FILE})
    endif ()
    # create a directory in build tree
    file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${INTERFACE_DIRECTORY}")
    # we are using C++ code
    set_source_files_properties (${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)
    # make sure the runtime code is not included
    set_source_files_properties (${SWIG_INTERFACE_FILE}
                                 PROPERTIES SWIG_FLAGS "-v;-fvirtual")
    # make sure source file is not used before libraries are build
    set_source_files_properties (${SWIG_INTERFACE_FILE} PROPERTIES DEPENDS "${MODULE_LINK_LIBRARIES}")
    # finally create the swig project using CMake command
    set (MODULE_NAME ${MODULE}Python)
    cisst_cmake_debug ("cisst_add_swig_module: swig_add_library (${MODULE_NAME} LANGUAGE python SOURCES ${SWIG_INTERFACE_FILE})")
    swig_add_library (${MODULE_NAME}
      LANGUAGE python
      SOURCES ${SWIG_INTERFACE_FILE})

    if (WIN32)
      set_target_properties (${MODULE_NAME} PROPERTIES SUFFIX .pyd)
      set_target_properties (${MODULE_NAME} PROPERTIES DEBUG_POSTFIX "_d")
    endif (WIN32)
    target_include_directories (${MODULE_NAME} PRIVATE ${PYTHON_ALL_INCLUDE_DIRS})
    cisst_cmake_debug ("cisst_add_swig_module: swig_link_libraries (${MODULE_NAME} ${MODULE_LINK_LIBRARIES} ${PYTHON_ALL_LIBRARIES})")
    target_link_libraries (${MODULE_NAME} ${MODULE_LINK_LIBRARIES} ${PYTHON_ALL_LIBRARIES})

    # copy the .py file generated to wherever the libraries are
    add_custom_command (TARGET ${MODULE_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy_if_different
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
                                ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${MODULE_NAME}.py)
    if (FOLDER)
      set_property (TARGET ${MODULE_NAME} PROPERTY FOLDER "${FOLDER}")
    endif (FOLDER)

    # create a cisstCommon.py as CMake assumes one should be created
    # this is a bug that should be fixed in future releases of CMake.
    add_custom_command (TARGET ${MODULE_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy_if_different
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE}.py)

    # install files if requested
    if (${INSTALL_FILES})
      # install the interface files so that one can %import them
      install (FILES ${SWIG_INTERFACE_FILE}
               DESTINATION include/${MODULE}
               COMPONENT ${MODULE})

      # install library and python file
      install (TARGETS ${MODULE_NAME}
               RUNTIME DESTINATION bin
               LIBRARY DESTINATION lib
               COMPONENT ${MODULE})
      install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
               DESTINATION lib
               COMPONENT ${MODULE})

      # install extra header files
      foreach (header ${HEADER_FILES})
        install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/${header}"
                 DESTINATION include/${MODULE}
                 COMPONENT ${MODULE})
      endforeach (header)
    endif  (${INSTALL_FILES})

  else (EXISTS ${SWIG_INTERFACE_FILE})
    message (SEND_ERROR "cisst_add_swig_module: can't file SWIG interface file for ${MODULE}: ${SWIG_INTERFACE_FILE}")
  endif (EXISTS ${SWIG_INTERFACE_FILE})

endfunction (cisst_add_swig_module)


# Function to use cisstComponentGenerator, this function assumes input
# files are provided using a relative path
function (cisst_component_generator GENERATED_FILES_VAR_PREFIX ...)
  message (WARNING "cisst_component_generator macro is now deprecated!")
  # debug
  cisst_cmake_debug ("cisst_component_generator called with: ${ARGV}")
  if (${ARGC} LESS 1)
    message (SEND_ERROR "cisst_component_generator takes at least one argument.")
  endif (${ARGC} LESS 1)
  list (REMOVE_AT ARGV 0) # first one is name of variable

  # make sure cisstComponentGenerator is being build and find it
  # try to figure out if this is build along with cisst
  if (TARGET cisstCommon)
    # make sure the target exists
    if (TARGET cisstComponentGenerator)
      # if the target exists, use its destination
      #get_target_property (CISST_CG_EXECUTABLE cisstComponentGenerator LOCATION)
      set(CISST_CG_EXECUTABLE $<TARGET_FILE:cisstComponentGenerator>)
    else (TARGET cisstComponentGenerator)
      message (SEND_ERROR "To use the cisst_component_generator function (for ${GENERATED_FILES_VAR_PREFIX}) you need to build cisstComponentGenerator")
    endif (TARGET cisstComponentGenerator)
  else (TARGET cisstCommon)
    # assumes this is an external project, find using the path provided in cisst-config.cmake
    find_program (CISST_CG_EXECUTABLE cisstComponentGenerator
                  PATHS "${CISST_BINARY_DIR}/bin")
  endif (TARGET cisstCommon)

  # loop over input files
  foreach (input ${ARGV})
    # find input name
    cisst_cmake_debug ("cisst_component_generator: found input file: ${input}")
    get_filename_component (INPUT_WE "${input}" NAME_WE)
    set (input_absolute "${CMAKE_CURRENT_SOURCE_DIR}/${input}")
    # create output name and concatenate to list available in parent scope
    set (output_absolute "${CMAKE_CURRENT_BINARY_DIR}/${INPUT_WE}_initGenerated.cpp")
    cisst_cmake_debug ("cisst_component_generator: adding output file: ${output_absolute}")
    set (GENERATED_FILES ${GENERATED_FILES} ${output_absolute} ${input})
    # tell cmake the output is generated and how to generate it
    set_source_files_properties (${output_absolute} PROPERTIES GENERATED 1)
    add_custom_command (OUTPUT ${output_absolute}
                        COMMAND "${CISST_CG_EXECUTABLE}"
                        ${input_absolute} ${output_absolute}
                        MAIN_DEPENDENCY ${input}
                        DEPENDS ${CISST_CG_EXECUTABLE}
                        COMMENT "cisstComponentGenerator for ${INPUT_WE}")
  endforeach(input)

  # create variable to store all generated files names
  set (${GENERATED_FILES_VAR_PREFIX}_CISST_CG_SRCS ${GENERATED_FILES} PARENT_SCOPE)

  # add current source dir to include directories to compile generated code
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

endfunction (cisst_component_generator)




# Function to use cisstDataGenerator, this function assumes input
# files are provided using a relative path
function (cisst_data_generator GENERATED_FILES_VAR_PREFIX GENERATED_INCLUDE_DIRECTORY GENERATED_INCLUDE_SUB_DIRECTORY ...)
  # debug
  cisst_cmake_debug ("cisst_data_generator called with: ${ARGV}")
  if (${ARGC} LESS 3)
    message (SEND_ERROR "cisst_data_generator takes at least three arguments.")
  endif (${ARGC} LESS 3)
  list (REMOVE_AT ARGV 0) # GENERATED_FILES_VAR_PREFIX
  list (REMOVE_AT ARGV 0) # GENERATED_INCLUDE_DIRECTORY
  list (REMOVE_AT ARGV 0) # GENERATED_INCLUDE_SUB_DIRECTORY

  # make sure cisstDataGenerator is being build and find it
  # try to figure out if this is build along with cisst
  if (CMAKE_CROSSCOMPILING)
    find_program (CISST_DG_EXECUTABLE cisstDataGenerator)
  else (CMAKE_CROSSCOMPILING)
    # make sure the target exists
    if (TARGET cisstDataGenerator)
      # if the target exists, use its destination
      cisst_cmake_debug ("cisst_data_generator: cisstDataGenerator has been compiled within this project")
      set (CISST_DG_EXECUTABLE "$<TARGET_FILE:cisstDataGenerator>")
    else (TARGET cisstDataGenerator)
      cisst_cmake_debug ("cisst_data_generator: looking for cisstDataGenerator in ${CISST_BINARY_DIR}/bin")
      find_program (CISST_DG_EXECUTABLE cisstDataGenerator HINTS "${CISST_BINARY_DIR}/bin")
    endif (TARGET cisstDataGenerator)
  endif (CMAKE_CROSSCOMPILING)
  cisst_cmake_debug ("cisst_data_generator: cisstDataGenerator executable found: ${CISST_DG_EXECUTABLE}")

  # loop over input files
  foreach (input ${ARGV})
    # find input name
    cisst_cmake_debug ("cisst_data_generator: found input file: ${input}")
    get_filename_component (INPUT_WE "${input}" NAME_WE)
    set (input_absolute "${CMAKE_CURRENT_SOURCE_DIR}/${input}")
    # create output name and concatenate to list available in parent scope
    set (FULL_GENERATED_INCLUDE_DIRECTORY "${GENERATED_INCLUDE_DIRECTORY}/${GENERATED_INCLUDE_SUB_DIRECTORY}")
    file (MAKE_DIRECTORY ${FULL_GENERATED_INCLUDE_DIRECTORY})
    cisst_cmake_debug ("cisst_data_generator: creating directory for generated header file ${FULL_GENERATED_INCLUDE_DIRECTORY}")
    set (header_absolute "${GENERATED_INCLUDE_DIRECTORY}/${GENERATED_INCLUDE_SUB_DIRECTORY}${INPUT_WE}.h")
    set (code_absolute "${CMAKE_CURRENT_BINARY_DIR}/${INPUT_WE}_cdg.cpp")
    cisst_cmake_debug ("cisst_data_generator: adding output files: ${header_absolute} ${code_absolute}")
    set (GENERATED_FILES ${GENERATED_FILES} ${code_absolute} ${header_absolute})
    set (GENERATED_FILES_HDRS ${GENERATED_FILES_HDRS} "${INPUT_WE}.h")
    set (GENERATED_FILES_HDRS_FULLPATH ${GENERATED_FILES_HDRS_FULLPATH} ${header_absolute})
    # tell cmake the output is generated and how to generate it
    set_source_files_properties (${header_absolute} PROPERTIES GENERATED 1)
    set_source_files_properties (${code_absolute} PROPERTIES GENERATED 1)
    set_source_files_properties ("${INPUT_WE}.h" PROPERTIES GENERATED 1)
    add_custom_command (
      OUTPUT ${header_absolute} ${code_absolute}
      COMMAND "${CISST_DG_EXECUTABLE}"
      --verbose
      --input ${input_absolute}
      --header-directory ${GENERATED_INCLUDE_DIRECTORY} --header-file ${GENERATED_INCLUDE_SUB_DIRECTORY}${INPUT_WE}.h
      --code-directory   ${CMAKE_CURRENT_BINARY_DIR} --code-file ${INPUT_WE}_cdg.cpp
      MAIN_DEPENDENCY ${input}
      DEPENDS "${CISST_DG_EXECUTABLE}"
      COMMENT "cisstDataGenerator for ${INPUT_WE}")
  endforeach(input)

  # create variables to store all generated files names
  set (${GENERATED_FILES_VAR_PREFIX}_CISST_DG_SRCS ${GENERATED_FILES} PARENT_SCOPE)
  set (${GENERATED_FILES_VAR_PREFIX}_CISST_DG_HDRS ${GENERATED_FILES_HDRS} PARENT_SCOPE)
  set (${GENERATED_FILES_VAR_PREFIX}_CISST_DG_HDRS_FULLPATH ${GENERATED_FILES_HDRS_FULLPATH} PARENT_SCOPE)

endfunction (cisst_data_generator)
# function to add all the available tests
function (cisst_add_test ...)
  # debug
  cisst_cmake_debug ("cisst_add_test called with: ${ARGV}")

  if (${ARGC} LESS 5)
    message (SEND_ERROR "cisst_add_test takes 5 arguments, test program, INSTANCES and number, ITERATIONS and number.  Got: ${ARGV}")
  endif (${ARGC} LESS 5)

  set (ALL_ARGS ${ARGV})
  list (GET ALL_ARGS 0 TEST_PROGRAM)
  list (REMOVE_AT ALL_ARGS 0) # first one is the test program
  cisst_cmake_debug ("cisst_add_test, test program ${TEST_PROGRAM} to be used with options ${ALL_ARGS}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       ITERATIONS
       INSTANCES)

  # reset local variables
  foreach(keyword ${FUNCTION_KEYWORDS})
    set (${keyword} "")
  endforeach(keyword)

  # parse input
  foreach (arg ${ALL_ARGS})
    list (FIND FUNCTION_KEYWORDS ${arg} ARGUMENT_IS_A_KEYWORD)
    if (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (CURRENT_PARAMETER ${arg})
      set (${CURRENT_PARAMETER} "")
    else (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (${CURRENT_PARAMETER} ${${CURRENT_PARAMETER}} ${arg})
    endif (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
  endforeach (arg)

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_add_test: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  # Once the test program is compiled, run it to create a list of available tests
  if (WIN32)
    # On Windows, create batch file to set path and then call test program
    set (BATCH_TEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/Run-${TEST_PROGRAM}.bat")
    file (WRITE  ${BATCH_TEST_FILE} "@ECHO OFF\n")
    file (APPEND ${BATCH_TEST_FILE} "CALL ${cisst_BINARY_DIR}/cisstvars.bat %6\n")
    file (APPEND ${BATCH_TEST_FILE} "${EXECUTABLE_OUTPUT_PATH}/%6/${TEST_PROGRAM} %1 %2 %3 %4 %5 > "
                                    "${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-${TEST_PROGRAM}.txt\n")
    add_custom_command (TARGET ${TEST_PROGRAM}
                        POST_BUILD
                        COMMAND ${BATCH_TEST_FILE}
                        ARGS -d -i ${ITERATIONS} -o ${INSTANCES} ${CMAKE_CFG_INTDIR}
                        COMMENT "Generating ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-${TEST_PROGRAM}.txt")
  else (WIN32)
    add_custom_command (TARGET ${TEST_PROGRAM}
                        POST_BUILD
                        COMMAND ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${TEST_PROGRAM}
                        ARGS -d -i ${ITERATIONS} -o ${INSTANCES} > ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-${TEST_PROGRAM}.txt
                        COMMENT "Generating ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-${TEST_PROGRAM}.txt")
  endif (WIN32)

  # Add the custom build list
  set_directory_properties (PROPERTIES TEST_INCLUDE_FILE
                            "${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-${TEST_PROGRAM}.txt")
endfunction (cisst_add_test)


# macro to generated standardized message explaining why optional code will not be compiled
macro (cisst_information_message_missing_libraries ...)
  unset (_cimml_MISSING_LIBRARIES)
  foreach (lib ${ARGV})
    list (FIND CISST_LIBRARIES ${lib} FOUND_IT)
    if (${FOUND_IT} EQUAL -1 )
      set (_cimml_MISSING_LIBRARIES ${_cimml_MISSING_LIBRARIES} ${lib})
    endif (${FOUND_IT} EQUAL -1 )
  endforeach (lib)
  # it is possible all libraries are here but cisst-config.cmake was not found
  if (_cimml_MISSING_LIBRARIES)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${_cimml_MISSING_LIBRARIES}.  You have to change your cisst configuration if you need these features.")
  else (_cimml_MISSING_LIBRARIES)
    message ("Information: all libraries have been found for ${CMAKE_CURRENT_SOURCE_DIR}, it is possible cisst-config.cmake has not been found yet,  make sure the CMake configuration is complete first.")
  endif (_cimml_MISSING_LIBRARIES)
endmacro (cisst_information_message_missing_libraries)


# Macro to find a saw component This macros uses the find_package
# CMake mechanism with a hint re. the path to search assuming saw
# packages are installed along cisst.  Once a saw component is found,
# it checks if this is an "installed" version.  In this case, both
# include and link directories are automatically modified to match the
# install root, i.e. the install directories are added at the
# beginning ot paths.  This change is performed so that
# saw<component>Config.cmake files can be generated once for the build
# tree only.
macro (cisst_find_saw_component ...)
  set (_cfc_INSTALLED_PATH "${CISST_BINARY_DIR}/${CISST_CMAKE_INSTALL_SUFFIX}/saw")

  set (_cfc_PARAMETERS ${ARGV})
  list (FIND _cfc_PARAMETERS "QUIET" _cfc_QUIET)

  if (${_cfc_QUIET} EQUAL -1)
    message ("-- Looking for saw component: ${ARGV0} in: ${_cfc_INSTALLED_PATH}")
  endif ()

   # Search using user arguments with our hints
  find_package (${ARGV}
                HINTS ${_cfc_INSTALLED_PATH}
                "${CISST_BINARY_DIR}/..")

  if (${ARGV0}_FOUND)
    if (${${ARGV0}_DIR} STREQUAL ${_cfc_INSTALLED_PATH})
      # If this is an installed version, add installed dir in front of
      # libdir and include directories
      message ("-- Found saw component \"${ARGV0}\" in cisst install path: " ${${ARGV0}_DIR})
      set (${ARGV0}_INCLUDE_DIR ${CISST_INCLUDE_DIR} ${${ARGV0}_INCLUDE_DIR})
      set (${ARGV0}_LIBRARY_DIR ${CISST_LIBRARY_DIR} ${${ARGV0}_LIBRARY_DIR})
    endif ()
    if (${_cfc_QUIET} EQUAL -1)
      message ("-- Found saw component \"${ARGV0}\" in cisst non-install path: " ${${ARGV0}_DIR})
    endif ()
  endif ()
endmacro (cisst_find_saw_component)


# Macro to add option to compile a saw component, also sets the
# ${component}_DIR variable to allow CMake to find the generated
# ${component}Config.cmake file.  Component name must match the name
# of directory to be included using the CMake function
# add_subdirectory.
macro (cisst_offer_saw_component component default)
  set (cosc_OPTION_NAME SAW_${component})
  option (${cosc_OPTION_NAME} "Build ${component}" ${default})
  if (${cosc_OPTION_NAME})
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${component}/CMakeLists.txt")
      set (${component}_DIR "${CMAKE_CURRENT_BINARY_DIR}/${component}")
      set (cosc_SOURCE_DIR "${component}")
    else ()
      if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${component}/core/CMakeLists.txt")
        set (${component}_DIR "${CMAKE_CURRENT_BINARY_DIR}/${component}/core")
        set (cosc_SOURCE_DIR "${component}/core")
      else ()
        message (SEND_ERROR "Couldn't find a CMakeLists in ${component} nor ${component}/core")
      endif ()
    endif ()
    list (APPEND CMAKE_PREFIX_PATH
                 "${${component}_DIR}"
                 "${${component}_DIR}/components")
    mark_as_advanced (${component}_DIR)
    add_subdirectory (${cosc_SOURCE_DIR})
  else (${cosc_OPTION_NAME})
    unset (${component}_DIR)
  endif (${cosc_OPTION_NAME})
endmacro (cisst_offer_saw_component)


# function to generate a config version file
function (cisst_add_config_version ...)
  # debug
  cisst_cmake_debug ("cisst_add_config_version called with: ${ARGV}")

  # get name of config file
  set (ALL_ARGS ${ARGV})
  list (GET ALL_ARGS 0 _cacv_configFile)
  list (REMOVE_AT ALL_ARGS 0) # first one is the config file
  cisst_cmake_debug ("cisst_add_config_version, config file ${_cacv_configFile} to be generated with options ${ALL_ARGS}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       VERSION
       DESTINATION
       COMPONENT)

  # reset local variables
  foreach(keyword ${FUNCTION_KEYWORDS})
    set (${keyword} "")
  endforeach(keyword)

  # parse input
  foreach (arg ${ALL_ARGS})
    list (FIND FUNCTION_KEYWORDS ${arg} ARGUMENT_IS_A_KEYWORD)
    if (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (CURRENT_PARAMETER ${arg})
      set (${CURRENT_PARAMETER} "")
    else (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (${CURRENT_PARAMETER} ${${CURRENT_PARAMETER}} ${arg})
    endif (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
  endforeach (arg)

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_add_config_version: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  write_basic_package_version_file (
    ${_cacv_configFile}
    VERSION ${VERSION}
    COMPATIBILITY SameMajorVersion)
  if (DESTINATION)
    if (COMPONENT)
      install (FILES ${_cacv_configFile}
        DESTINATION ${DESTINATION}
        COMPONENT ${COMPONENT})
    else (COMPONENT)
      install (FILES ${_cacv_configFile}
        DESTINATION ${DESTINATION})
    endif (COMPONENT)
  endif (DESTINATION)

endfunction (cisst_add_config_version)


#
# Test if this build using ROS/catkin
#
function (cisst_is_catkin_build RESULT)
  set (${RESULT} FALSE PARENT_SCOPE)
  if (DEFINED ENV{ROS_ROOT} OR DEFINED ENV{ROS_VERSION})
    message (STATUS "Assuming cisst is built using ROS/catkin since ROS_ROOT is defined in environment")
    if (DEFINED CATKIN_DEVEL_PREFIX)
      set (${RESULT} TRUE PARENT_SCOPE)
    else ()
      message (STATUS "CATKIN_DEVEL_PREFIX is not defined so assuming this is NOT a ROS/catkin build after all")
    endif ()
  endif ()
endfunction (cisst_is_catkin_build)


#
# Test if this build using ROS2/colcon
#
function (cisst_is_colcon_build RESULT)
  set (${RESULT} FALSE PARENT_SCOPE)
  if (DEFINED ENV{ROS_VERSION})
    if ($ENV{ROS_VERSION} STREQUAL "2")
      message (STATUS "Assuming cisst is built using ROS2/colcon since ROS_VERSION is 2")
      set (${RESULT} TRUE PARENT_SCOPE)
    endif ()
  endif ()
endfunction (cisst_is_colcon_build)


# macro to set default cpack settings
macro (cisst_cpack_settings ...)
  # debug
  cisst_cmake_debug ("cisst_cpack_settings called with: ${ARGV}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       VENDOR
       MAINTAINER)

  # reset local variables
  foreach (keyword ${FUNCTION_KEYWORDS})
    set (${keyword} "")
  endforeach (keyword)

  # parse input
  foreach (arg ${ARGV})
    list (FIND FUNCTION_KEYWORDS ${arg} ARGUMENT_IS_A_KEYWORD)
    if (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (CURRENT_PARAMETER ${arg})
      set (${CURRENT_PARAMETER} "")
    else (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
      set (${CURRENT_PARAMETER} ${${CURRENT_PARAMETER}} ${arg})
    endif (${ARGUMENT_IS_A_KEYWORD} GREATER -1)
  endforeach (arg)

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_cpack_settings: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  if (UNIX)
    set (CPACK_PACKAGE_VENDOR "${VENDOR}")
    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${MAINTAINER}")
    set (CPACK_GENERATOR "DEB")
    set (CPACK_DEB_PACKAGE_COMPONENT ON)
    set (CPACK_DEB_COMPONENT_INSTALL ON)
    set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set (CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  endif (UNIX)

endmacro (cisst_cpack_settings)

# Macro to set output paths based on build type, i.e. ROS 1/catkin or not
macro (cisst_set_output_path)
  cisst_is_catkin_build (_is_catkin_build)
  if (_is_catkin_build)
    set (LIBRARY_OUTPUT_PATH "${CATKIN_DEVEL_PREFIX}/lib")
    set (EXECUTABLE_OUTPUT_PATH "${CATKIN_DEVEL_PREFIX}/bin")
  endif ()
  cisst_is_colcon_build (_is_colcon_build)
  if (_is_colcon_build)
    set (LIBRARY_OUTPUT_PATH "${CISST_BINARY_DIR}/lib")
    set (EXECUTABLE_OUTPUT_PATH "${CISST_BINARY_DIR}/bin")
  endif ()
endmacro (cisst_set_output_path)

# For projects using cisst in CMake add_subdirectory
macro (cisst_use_cisst_output_directories)
  set (LIBRARY_OUTPUT_PATH "${cisst_BINARY_DIR}/${CISST_LIBRARY_INSTALL_SUFFIX}")
  set (EXECUTABLE_OUTPUT_PATH "${cisst_BINARY_DIR}/bin")
endmacro (cisst_use_cisst_output_directories)


# Add standard configuration files for SAW component, assumes all
# settings are already defined
function (cisst_add_config_files _cacf_component_name)

  cisst_is_catkin_build (_cacf_is_catkin_build)
  if (_cacf_is_catkin_build)
    set (_cacf_config_file_dir "${CATKIN_DEVEL_PREFIX}/share/${_cacf_component_name}/cmake")
  else ()
    set (_cacf_config_file_dir "${${_cacf_component_name}_BINARY_DIR}")
  endif ()

  set (_cacf_version_major ${${_cacf_component_name}_VERSION_MAJOR})
  set (_cacf_version_minor ${${_cacf_component_name}_VERSION_MINOR})
  set (_cacf_version_patch ${${_cacf_component_name}_VERSION_PATCH})
  set (_cacf_version       ${${_cacf_component_name}_VERSION})
  set (_cacf_include_dir   ${${_cacf_component_name}_INCLUDE_DIR})
  set (_cacf_library_dir   ${${_cacf_component_name}_LIBRARY_DIR})
  set (_cacf_libraries     ${${_cacf_component_name}_LIBRARIES})

  # generate componentRevision.h
  set (
    _cacf_revision_h
    "${${_cacf_component_name}_BINARY_DIR}/include/${_cacf_component_name}/${_cacf_component_name}Revision.h")
  find_file (
    _cacf_revision_h_in
    NAMES sawRevision.h.in
    PATHS ${CISST_CMAKE_DIRS}
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
  configure_file (
    ${_cacf_revision_h_in}
    ${_cacf_revision_h}
    @ONLY)

  # generate componentConfig.cmake
  set (_cacf_config_cmake
    "${_cacf_config_file_dir}/${_cacf_component_name}Config.cmake")
  find_file (
    _cacf_config_cmake_in
    NAMES sawConfig.cmake.in
    PATHS ${CISST_CMAKE_DIRS}
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
  configure_file (
    ${_cacf_config_cmake_in}
    ${_cacf_config_cmake}
    @ONLY)

  # generate componentConfigVersion.cmake
  set (_cacf_config_version_cmake
    "${_cacf_config_file_dir}/${_cacf_component_name}ConfigVersion.cmake")
  write_basic_package_version_file (
    ${_cacf_config_version_cmake}
    VERSION ${_cacf_version}
    COMPATIBILITY SameMajorVersion)

  # install cmake config files
  install (
    FILES ${_cacf_config_cmake} ${_cacf_config_version_cmake}
    DESTINATION "share/${_cacf_component_name}"
    COMPONENT ${_cacf_component_name})

endfunction (cisst_add_config_files)
