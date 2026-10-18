cmake_minimum_required (VERSION 3.16)
project (cisstCppUnitDiscovery LANGUAGES NONE)

# Each scenario gets a fresh process so imported targets and find caches cannot
# hide differences between package configs, the manual finder and missing files.
set (_generator -G "${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR_PLATFORM)
  list (APPEND _generator -A "${CMAKE_GENERATOR_PLATFORM}")
endif ()
foreach (_case manual release-only config namespaced-config existing missing required)
  execute_process (
    COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_CURRENT_LIST_DIR}/consumer"
            -B "${CMAKE_CURRENT_BINARY_DIR}/${_case}" ${_generator} "-DCASE=${_case}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
  file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/${_case}.log" "${_output}${_error}")
  if (_case STREQUAL "required")
    if (_result EQUAL 0 OR NOT "${_error}" MATCHES "Could NOT find CppUnit")
      message (FATAL_ERROR "Missing REQUIRED CppUnit did not fail correctly:\n${_output}${_error}")
    endif ()
  elseif (NOT _result EQUAL 0)
    message (FATAL_ERROR "CppUnit ${_case} failed:\n${_output}${_error}")
  elseif (_case STREQUAL "missing" AND "${_output}${_error}" MATCHES "Could NOT find CppUnit")
    message (FATAL_ERROR "QUIET CppUnit discovery printed a failure message")
  endif ()
  message (STATUS "CppUnit ${_case}: passed")
endforeach ()
