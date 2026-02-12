#
# Author(s):  Peter Kazanzides, Anton Deguet
# Created on: 2019
#
# (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

function (check_if_different_types VARIABLE type1 type2)
  # make sure we don't test over and over
  if (${VARIABLE} MATCHES "^${VARIABLE}$")
    message (STATUS "Checking to see if ${type1} and ${type2} are different types")
    set (SOURCE "
          // First, test overloaded functions
          char method(${type1} p) {
             return 't1';
          }
          char method(${type2} p) {
            return 't2';
          }
          // Next, test template specialization
          template <class _elementType>
          char template_method(void) {
            return '?';
          }
          template <>
          char template_method<${type1}>(void) {
            return 't1';
          }
          template <>
          char template_method<${type2}>(void) {
            return 't2';
          }
          int main(void) {}")

    # Convert spaces (e.g., in "long long") to underscores for file names
    string (REPLACE " " "_" TESTFILE "test_${type1}_and_${type2}.cpp")
    file (WRITE
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/${TESTFILE}"
          "${SOURCE}\n")

    try_compile (${VARIABLE}
                 ${CMAKE_BINARY_DIR}
                 "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/${TESTFILE}"
                 OUTPUT_VARIABLE OUTPUT)

    # report using message and log files
    if (${VARIABLE})
      message (STATUS "Checking to see if ${type1} and ${type2} are different types - yes")
      file (APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
            "Determining if ${type1} and ${type2} are different types passed with "
            "the following output:\n${OUTPUT}\n\n")
    else (${VARIABLE})
      message (STATUS "Checking to see if ${type1} and ${type2} are different types - no")
      file (APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
            "Determining if ${type1} and ${type2} are different types passed with "
            "the following output:\n${OUTPUT}\n\n")
    endif (${VARIABLE})

  endif (${VARIABLE} MATCHES "^${VARIABLE}$")

endfunction (check_if_different_types)
