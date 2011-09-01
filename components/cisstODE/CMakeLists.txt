
# Need to add the module path to search for FindRTSocket.cmake
set( 
  CMAKE_MODULE_PATH 
  ${CMAKE_MODULE_PATH} 
  ${CMAKE_CURRENT_SOURCE_DIR}/code/cmake/Modules
  )

add_subdirectory( code )
add_subdirectory( examples )

