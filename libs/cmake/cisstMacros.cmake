#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2004-01-22
#
# (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# function used to determine if some extra configuration messages
# should be displayed
function (cisst_cmake_debug ...)
  if (CISST_HAS_CMAKE_DEBUG)
    message ("cisst CMake debug: ${ARGV}")
  endif (CISST_HAS_CMAKE_DEBUG)
endfunction (cisst_cmake_debug)


# macro to load settings set for external packages
# usage: cisst_load_package_setting (cisstCommon cisstVector) or cisst_load_package_setting (${CISST_LIBRARIES})
macro (cisst_load_package_setting ...)
  # Set all variables based on dependencies
  foreach (lib ${ARGV})
    # External dependency file
    set (SETTINGS_FILE "${cisst_BINARY_DIR}/cisst-dependencies/${lib}External.cmake")
    if (EXISTS ${SETTINGS_FILE})
      include (${SETTINGS_FILE})
      set (EXTERNAL_PACKAGES ${CISST_EXTERNAL_PACKAGES_FOR_${lib}})
      foreach (package ${EXTERNAL_PACKAGES})
        set (PACKAGE_FILE "${cisst_BINARY_DIR}/cisst-dependencies/${lib}${package}.cmake")
        if (EXISTS ${PACKAGE_FILE})
          include (${PACKAGE_FILE})
        else (EXISTS ${PACKAGE_FILE})
          message (SEND_ERROR "Based on ${EXTERNAL_PACKAGES}, there should be a file named ${PACKAGE_FILE}, you might need to start from an empty build tree")
        endif (EXISTS ${PACKAGE_FILE})
      endforeach (package)
    endif (EXISTS ${SETTINGS_FILE})
     # find and load general settings
    set (SETTINGS CISST_GENERAL_SETTINGS_FOR_${lib})
    if (${SETTINGS})
      cisst_load_package_setting (${${SETTINGS}})
    endif (${SETTINGS})
  endforeach (lib)
endmacro (cisst_load_package_setting)


# macro to set the include directory based on external settings
macro (cisst_include_directories ...)
  foreach (lib ${ARGV})
    # find and load setting for external packages
    set (PACKAGES CISST_EXTERNAL_PACKAGES_FOR_${lib})
    if (${PACKAGES})
      foreach (package ${${PACKAGES}})
        set (VARIABLE_NAME CISST_INCLUDE_DIRECTORIES_FOR_${lib}_USING_${package})
        if (${VARIABLE_NAME})
          include_directories (${${VARIABLE_NAME}})
        endif (${VARIABLE_NAME})
      endforeach (package)
    endif (${PACKAGES})
     # find and load general settings
    set (SETTINGS CISST_GENERAL_SETTINGS_FOR_${lib})
    if (${SETTINGS})
      cisst_include_directories (${${SETTINGS}})
    endif (${SETTINGS})
  endforeach (lib)
endmacro (cisst_include_directories)


# macro to set the link directories based on external settings
macro (cisst_link_directories ...)
  foreach (lib ${ARGV})
    # find and load setting for external packages
    set (PACKAGES CISST_EXTERNAL_PACKAGES_FOR_${lib})
    if (${PACKAGES})
      foreach (package ${${PACKAGES}})
        set (VARIABLE_NAME CISST_LINK_DIRECTORIES_FOR_${lib}_USING_${package})
        if (${VARIABLE_NAME})
          link_directories (${${VARIABLE_NAME}})
        endif (${VARIABLE_NAME})
      endforeach (package)
    endif (${PACKAGES})
     # find and load general settings
    set (SETTINGS CISST_GENERAL_SETTINGS_FOR_${lib})
    if (${SETTINGS})
      cisst_link_directories (${${SETTINGS}})
    endif (${SETTINGS})
  endforeach (lib)
endmacro (cisst_link_directories)


# macro to find packages based on external settings
macro (cisst_find_and_use_packages ...)
  foreach (lib ${ARGV})
    # find and load setting for external packages
    set (PACKAGES CISST_EXTERNAL_PACKAGES_FOR_${lib})
    if (${PACKAGES})
      foreach (package ${${PACKAGES}})
        # find package
        set (VARIABLE_NAME CISST_PACKAGES_FOR_${lib}_USING_${package})
        if (${VARIABLE_NAME})
          # check for components if needed
          set (COMPONENT_VARIABLE_NAME CISST_PACKAGE_COMPONENTS_FOR_${lib}_USING_${package})
          if (${COMPONENT_VARIABLE_NAME})
            find_package (${${VARIABLE_NAME}} REQUIRED ${${COMPONENT_VARIABLE_NAME}})
          else (${COMPONENT_VARIABLE_NAME})
            find_package (${${VARIABLE_NAME}} REQUIRED)
          endif (${COMPONENT_VARIABLE_NAME})
        endif (${VARIABLE_NAME})
        # use package
        set (VARIABLE_NAME CISST_CMAKE_FILES_FOR_${lib}_USING_${package})
        if (${VARIABLE_NAME})
          include (${${VARIABLE_NAME}})
        endif (${VARIABLE_NAME})
      endforeach (package)
    endif (${PACKAGES})
    # find and load general settings
    set (SETTINGS CISST_GENERAL_SETTINGS_FOR_${lib})
    if (${SETTINGS})
      cisst_find_and_use_packages (${${SETTINGS}})
    endif (${SETTINGS})
  endforeach (lib)
endmacro (cisst_find_and_use_packages)


# helper macro to set all directories
# Use macro instead of function due to scope issues
macro (cisst_set_directories ...)
  cisst_load_package_setting (${ARGV})
  cisst_include_directories (${ARGV})
  cisst_link_directories (${ARGV})
  cisst_find_and_use_packages (${ARGV})
endmacro (cisst_set_directories)


# The macro adds a library to a CISST-related project by processing the
# following parameters
#
# - PROJECT (cisstLibs by default)
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
       DEPENDENCIES
       SOURCE_FILES HEADER_FILES
       ADDITIONAL_SOURCE_FILES ADDITIONAL_HEADER_FILES)

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
    set (PROJECT "cisstLibs")
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
  set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}pragma once\n")
  set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}ifndef _${LIBRARY}_h\n")
  set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}define _${LIBRARY}_h\n\n")
  foreach (file ${HEADER_FILES})
    set (HEADERS ${HEADERS} ${${PROJECT}_SOURCE_DIR}/${LIBRARY_DIR}/${file})
    set (FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}include <${LIBRARY_DIR}/${file}>\n")
  endforeach (file)
  set (FILE_CONTENT ${FILE_CONTENT} "\n${CISST_STRING_POUND}endif // _${LIBRARY}_h\n")
  file (WRITE ${LIBRARY_MAIN_HEADER} ${FILE_CONTENT})

  # Add the main header to the library, for IDEs
  set (HEADERS ${HEADERS} ${LIBRARY_MAIN_HEADER})

  # Set paths
  cisst_set_directories (${LIBRARY} ${DEPENDENCIES})

  # Add the library
  cisst_cmake_debug ("cisst_add_library: Adding library ${LIBRARY} using files ${SOURCE_FILES} ${HEADERS}")
  add_library (${LIBRARY}
               ${IS_SHARED}
               ${SOURCE_FILES}
               ${ADDITIONAL_SOURCE_FILES}
               ${HEADERS}
               ${ADDITIONAL_HEADER_FILES}
               )

  # Install the library
  install_targets (/lib ${LIBRARY})

  # Add dependencies for linking, also check BUILD_xxx for dependencies
  if (DEPENDENCIES)
    # Check that dependencies are build
    set (BUILD_DEPENDENCIES "")
    foreach (dependency ${DEPENDENCIES})
      set (BUILD_DEPENDENCIES ${BUILD_DEPENDENCIES} CISST_BUILD_LIBS_${dependency})
    endforeach (dependency)
    variable_requires (CISST_BUILD_LIBS_${LIBRARY} CISST_BUILD_LIBS_${LIBRARY} ${BUILD_DEPENDENCIES})
    # Set the link flags
    target_link_libraries (${LIBRARY} ${DEPENDENCIES})
    cisst_cmake_debug ("cisst_add_library: Library ${LIBRARY} links against: ${DEPENDENCIES}")
  endif (DEPENDENCIES)

  # Link to cisst additional libraries
  cisst_target_link_package_libraries (${LIBRARY} ${LIBRARY} ${DEPENDENCIES})

  # Install all header files
  install_files (/include/${LIBRARY_DIR}
                 ".h"
                 ${HEADERS})
  install_files (/include/
                 ".h"
                 ${LIBRARY_MAIN_HEADER})

endmacro (cisst_add_library)


macro (cisst_target_link_package_libraries target ...)
  # create list of all but target
  set (DEPENDENCIES ${ARGV})
  list (REMOVE_AT DEPENDENCIES 0) # remove first argument, i.e. target
  cisst_load_package_setting(${DEPENDENCIES})
  foreach (lib ${DEPENDENCIES})
    # find and load setting for external packages
    set (PACKAGES CISST_EXTERNAL_PACKAGES_FOR_${lib})
    if (${PACKAGES})
      foreach (package ${${PACKAGES}})
        set (VARIABLE_NAME CISST_LIBRARIES_FOR_${lib}_USING_${package})
        if (${VARIABLE_NAME})
          target_link_libraries (${target} ${${VARIABLE_NAME}})
        endif (${VARIABLE_NAME})
      endforeach (package)
    endif (${PACKAGES})
     # find and load general settings
    set (SETTINGS CISST_GENERAL_SETTINGS_FOR_${lib})
    if (${SETTINGS})
      cisst_target_link_package_libraries (${target} ${${SETTINGS}})
    endif (${SETTINGS})
  endforeach (lib)
endmacro (cisst_target_link_package_libraries)


# Macro used to compare required libraries for a given target with
# libraries actually compiled.  This macro adds the required link
# options.
macro (cisst_target_link_libraries TARGET ...)
  # debug
  cisst_cmake_debug ("cisst_target_link_libraries called with: ${ARGV}")
  if (${ARGC} LESS 2)
    message (SEND_ERROR "cisst_target_link_libraries takes at least two arguments, target and one or more libraries.  Got: ${ARGV}")
  endif (${ARGC} LESS 2)

  set (REQUIRED_CISST_LIBRARIES ${ARGV})
  list (GET REQUIRED_CISST_LIBRARIES 0 WHO_REQUIRES)
  list (REMOVE_AT REQUIRED_CISST_LIBRARIES 0) # first one is the library name
  cisst_cmake_debug ("cisst_target_link_libraries, library ${WHO_REQUIRES} to link against ${REQUIRED_CISST_LIBRARIES}")

  if (CISST_BUILD_SHARED_LIBS)
    add_definitions(-DCISST_DLL)
  endif (CISST_BUILD_SHARED_LIBS)

  # First test that all libraries should have been compiled
  foreach (required ${REQUIRED_CISST_LIBRARIES})
    if ("${CISST_LIBRARIES}"  MATCHES ${required})
    else ("${CISST_LIBRARIES}"  MATCHES ${required})
      message (SEND_ERROR "${WHO_REQUIRES} requires ${required} which doesn't exist or hasn't been compiled")
    endif ("${CISST_LIBRARIES}"  MATCHES ${required})
  endforeach (required)

  # Second, create a list of libraries in the right order
  foreach (existing ${CISST_LIBRARIES})
    if ("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
      set (CISST_LIBRARIES_TO_USE ${CISST_LIBRARIES_TO_USE} ${existing})
    endif ("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
  endforeach (existing)

  # Finally, link with the required libraries
  target_link_libraries (${WHO_REQUIRES} ${CISST_LIBRARIES_TO_USE})
  cisst_target_link_package_libraries (${WHO_REQUIRES} ${CISST_LIBRARIES_TO_USE})

endmacro (cisst_target_link_libraries)


# The function adds a SWIG module to a CISST-related project by processing the
# following parameters
#
# - MODULE is the prefix of the main .i file.  The module name will be <MODULE>Python
# - INTERFACE_FILENAME is the filename of the .i file (if not specified, defaults to <MODULE>.i)
# - INTERFACE_DIRECTORY is the directory containing the .i file (use relative path from current source dir)
# - CISST_LIBRARIES cisst libraries needed to link the module (can be used for other libraries as long as CMake can find them)
#
# NOTE: This must be a function, not a macro, because if it is a macro the keyword CISST_LIBRARIES
#       interferes with the CISST_LIBRARIES variable defined in the main CMakeLists.txt.
#       It would be better to rename one of these.
function (cisst_add_swig_module ...)
  # debug
  cisst_cmake_debug ("cisst_add_swig_module called with: ${ARGV}")

  # set all keywords and their values to ""
  set (FUNCTION_KEYWORDS
       MODULE
       INTERFACE_FILENAME
       INTERFACE_DIRECTORY
       CISST_LIBRARIES)

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

  # set default interface file name
  if (NOT INTERFACE_FILENAME)
    set (INTERFACE_FILENAME "${MODULE}.i")
  endif (NOT INTERFACE_FILENAME)

  # debug
  foreach (keyword ${FUNCTION_KEYWORDS})
    cisst_cmake_debug ("cisst_add_swig_module: ${keyword}: ${${keyword}}")
  endforeach (keyword)

  # interface file
  set (SWIG_INTERFACE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE_DIRECTORY}/${INTERFACE_FILENAME})
  cisst_cmake_debug ("cisst_add_swig_module: looking for interface file ${SWIG_INTERFACE_FILE}")

  if (EXISTS ${SWIG_INTERFACE_FILE})
    # create a directory in build tree
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${INTERFACE_DIRECTORY})
    # we are using C++ code
    set_source_files_properties (${SWIG_INTERFACE_FILE} PROPERTIES CPLUSPLUS ON)
    # make sure the runtime code is not included
    set_source_files_properties (${SWIG_INTERFACE_FILE}
                                 PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
    set (MODULE_NAME ${MODULE}Python)
    swig_add_module (${MODULE_NAME} python ${SWIG_INTERFACE_FILE})
    if (WIN32)
      set_target_properties (_${MODULE_NAME} PROPERTIES SUFFIX .pyd)
      set_target_properties (_${MODULE_NAME} PROPERTIES DEBUG_POSTFIX "_d")
    endif (WIN32)
    cisst_load_package_setting ("cisstPython")
    cisst_include_directories ("cisstPython")
    swig_link_libraries (${MODULE_NAME} ${CISST_LIBRARIES}
                         ${CISST_LIBRARIES_FOR_cisstPython_USING_Python})

    # copy the .py file generated to wherever the libraries are
    add_custom_command (TARGET _${MODULE_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy_if_different
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
                                ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${MODULE_NAME}.py)
    # create a cisstCommon.py as CMake assumes one should be created
    # this is a bug that should be fixed in future releases of CMake.
    add_custom_command (TARGET _${MODULE_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy_if_different
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
                                ${CMAKE_CURRENT_BINARY_DIR}/${MODULE}.py)
    # install the interface files so that one can %import them
    install_files (/include
                   ".i"
                   ${INTERFACE})

  else (EXISTS ${SWIG_INTERFACE_FILE})
    message (SEND_ERROR "Can't file SWIG interface file for ${MODULE}: ${SWIG_INTERFACE_FILE}")
  endif (EXISTS ${SWIG_INTERFACE_FILE})

endfunction (cisst_add_swig_module)


# Function to use cisstComponentGenerator, this function assumes input
# files are provided using a relative path
function (cisst_component_generator GENERATED_FILES_VAR_PREFIX ...)
  # debug
  cisst_cmake_debug ("cisst_component_generator called with: ${ARGV}")
  if (${ARGC} LESS 1)
    message (SEND_ERROR "cisst_component_generator takes at least one argument.")
  endif (${ARGC} LESS 1)
  list (REMOVE_AT ARGV 0) # first one is name of variable

  # make sure cisstComponentGenerator is being build and find it
  # try to figure out if this is build along with cisst
  if (TARGET cisstCommon)
    # make sure the target existsOUTPUT_NAME
    if (TARGET cisstComponentGenerator)
      # if the target exists, use its destination
      get_target_property (CISST_CG_EXECUTABLE cisstComponentGenerator LOCATION)
    else (TARGET cisstComponentGenerator)
      message (SEND_ERROR "To use the cisst_component_generator function (for ${GENERATED_FILES_VAR_PREFIX}) you need to build cisstComponentGenerator, turn CISST_BUILD_UTILITIES ON first and then CISST_BUILD_UTILITIES_cisstComponentGenerator")
    endif (TARGET cisstComponentGenerator)
  else (TARGET cisstCommon)
    # assumes this is an external project, find using the path provided in cisst-config.cmake
    find_program (CISST_CG_EXECUTABLE cisstComponentGenerator
                  PATHS "${CISST_BINARY_DIR}/utilities/bin")
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
function (cisst_data_generator GENERATED_FILES_VAR_PREFIX ...)
  # debug
  cisst_cmake_debug ("cisst_data_generator called with: ${ARGV}")
  if (${ARGC} LESS 1)
    message (SEND_ERROR "cisst_data_generator takes at least one argument.")
  endif (${ARGC} LESS 1)
  list (REMOVE_AT ARGV 0) # first one is name of variable

  # make sure cisstDataGenerator is being build and find it
  # try to figure out if this is build along with cisst
  if (TARGET cisstCommon)
    # make sure the target existsOUTPUT_NAME
    if (TARGET cisstDataGenerator)
      # if the target exists, use its destination
      get_target_property (CISST_DG_EXECUTABLE cisstDataGenerator LOCATION)
    else (TARGET cisstDataGenerator)
      message (SEND_ERROR "To use the cisst_data_generator function (for ${GENERATED_FILES_VAR_PREFIX}) you need to build cisstDataGenerator, turn CISST_BUILD_UTILITIES ON first and then CISST_BUILD_UTILITIES_cisstDataGenerator")
    endif (TARGET cisstDataGenerator)
  else (TARGET cisstCommon)
    # assumes this is an external project, find using the path provided in cisst-config.cmake
    find_program (CISST_DG_EXECUTABLE cisstDataGenerator
                  PATHS "${CISST_BINARY_DIR}/utilities/bin")
  endif (TARGET cisstCommon)

  # loop over input files
  foreach (input ${ARGV})
    # find input name
    cisst_cmake_debug ("cisst_data_generator: found input file: ${input}")
    get_filename_component (INPUT_WE "${input}" NAME_WE)
    set (input_absolute "${CMAKE_CURRENT_SOURCE_DIR}/${input}")
    # create output name and concatenate to list available in parent scope
    set (header_absolute "${CMAKE_CURRENT_BINARY_DIR}/${INPUT_WE}.h")
    set (code_absolute "${CMAKE_CURRENT_BINARY_DIR}/${INPUT_WE}.cpp")
    cisst_cmake_debug ("cisst_data_generator: adding output files: ${header_absolute} ${code_absolute}")
    set (GENERATED_FILES ${GENERATED_FILES} ${header_absolute} ${code_absolute})
    # tell cmake the output is generated and how to generate it
    set_source_files_properties (${header_absolute} PROPERTIES GENERATED 1)
    set_source_files_properties (${code_absolute} PROPERTIES GENERATED 1)
    add_custom_command (OUTPUT ${header_absolute} ${code_absolute}
                        COMMAND "${CISST_DG_EXECUTABLE}"
			${input_absolute} ${CMAKE_CURRENT_BINARY_DIR} ${INPUT_WE}.h ${CMAKE_CURRENT_BINARY_DIR} ${INPUT_WE}.cpp
			MAIN_DEPENDENCY ${input}
			DEPENDS ${CISST_DG_EXECUTABLE}
                        COMMENT "cisstDataGenerator for ${INPUT_WE}")
  endforeach(input)

  # create variable to store all generated files names
  set (${GENERATED_FILES_VAR_PREFIX}_CISST_DG_SRCS ${GENERATED_FILES} PARENT_SCOPE)

endfunction (cisst_data_generator)




# DEPRECATED, USE cisst_add_library INSTEAD
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
IF(CISST_BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

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
  SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}pragma once\n")
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
      SET(BUILD_DEPENDENCIES ${BUILD_DEPENDENCIES} CISST_BUILD_LIBS_${dependency})
    ENDFOREACH(dependency)
    VARIABLE_REQUIRES(CISST_BUILD_LIBS_${LIBRARY} CISST_BUILD_LIBS_${LIBRARY} ${BUILD_DEPENDENCIES})
    # Set the link flags
    TARGET_LINK_LIBRARIES(${LIBRARY} ${DEPENDENCIES})
  ENDIF(DEPENDENCIES)

  # Install all header files
  INSTALL_FILES(/include/${LIBRARY}
                ".h"
                ${HEADERS})
  INSTALL_FILES(/include/
                ".h"
                ${LIBRARY_MAIN_HEADER})

ENDIF(CISST_BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

ENDMACRO(CISST_ADD_LIBRARY_TO_PROJECT)



# DEPRECATED, USE cisst_target_link_libraries INSTEAD
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
       MESSAGE(SEND_ERROR "${WHO_REQUIRES} requires ${required} which doesn't exist or hasn't been compiled")
     ENDIF("${CISST_LIBRARIES}"  MATCHES ${required})
   ENDFOREACH(required)

   # Second, create a list of libraries in the right order
   FOREACH(existing ${CISST_LIBRARIES})
     IF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
       SET(CISST_LIBRARIES_TO_USE ${CISST_LIBRARIES_TO_USE} ${existing})
     ENDIF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
   ENDFOREACH(existing)

   # Finally, link with the required libraries
   TARGET_LINK_LIBRARIES(${WHO_REQUIRES} ${CISST_LIBRARIES_TO_USE})

ENDMACRO(CISST_REQUIRES)



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
  cisst_cmake_debug ("cisst_add_test, test program ${TEST_PROGRAM} to ne used with options ${ALL_ARGS}")

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
  add_custom_command (TARGET ${TEST_PROGRAM}
                      POST_BUILD
                      COMMAND ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${TEST_PROGRAM}
                      ARGS -d -i ${ITERATIONS} -o ${INSTANCES} > ${CMAKE_CURRENT_BINARY_DIR}/DartTestfile-${TEST_PROGRAM}.txt
                      COMMENT "Generating DartTestfile-${TEST_PROGRAM}.txt")

  # Add the custom build list
  set_directory_properties (PROPERTIES TEST_INCLUDE_FILE
                            "${CMAKE_CURRENT_BINARY_DIR}/DartTestfile-${TEST_PROGRAM}.txt")
endfunction (cisst_add_test)

