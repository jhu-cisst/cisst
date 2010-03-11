PROJECT(daVinciViz)

# -----------------------------------------------------------------------------
# cisst
# -----------------------------------------------------------------------------
find_file(CISST_CMAKE
          NAMES cisstBuild.cmake
          DOC "Path of your cisstBuild.cmake"
          )

if(EXISTS ${CISST_CMAKE})
  include("${CISST_CMAKE}")

  # extend CMake Module Path to find cisst defined macros
  set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      ${CISST_CMAKE_DIR}
      )

  include(cisstMacros)  # includes CISST_REQUIRES

  include_directories(${CISST_INCLUDE_DIR})
  link_directories(${CISST_LIBRARY_DIR})

  # Allow generated c++ code to find header files
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  # ---------------------------------------------------------------------------
  # Qt
  # ---------------------------------------------------------------------------
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})

  # FLTK package 
  FIND_PACKAGE(FLTK REQUIRED) 

  # add include paths	
  INCLUDE_DIRECTORIES(${PyQtEmbedded_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})
  # Build executable for sineTask
  ADD_EXECUTABLE(SineTaskMain
	         sineTask.cpp 
                 sineMain.cpp 
                 sineTask.h 
                 )
 
  TARGET_LINK_LIBRARIES(SineTaskMain
                        debug ${PYTHON_DEBUG_LIBRARIES}
                        optimized ${PYTHON_LIBRARIES})

  # link with the cisst libraries
  CISST_REQUIRES("SineTaskMain" "cisstCommon;cisstOSAbstraction;cisstMultiTask;cisstInteractive")

  FLTK_WRAP_UI(DisplayTaskMain displayUI.fl)
  # Build separate executable for displayTask 
  ADD_EXECUTABLE(DisplayTaskMain 
                 displayTask.cpp 
                 displayMain.cpp 

                 displayTask.h 
	         ${DisplayTaskMain_FLTK_UI_SRCS}
                 ) 
 
  TARGET_LINK_LIBRARIES(DisplayTaskMain ${FLTK_LIBRARIES})

  # link with the cisst libraries
  CISST_REQUIRES("DisplayTaskMain" "cisstCommon;cisstOSAbstraction;cisstVector;cisstMultiTask")

  IF(UNIX)
     IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        TARGET_LINK_LIBRARIES(SineTaskMain util)
        TARGET_LINK_LIBRARIES(DisplayTaskMain util)
     ENDIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  ENDIF(UNIX)

# Copy the Python example to the binary tree
#ADD_CUSTOM_COMMAND(TARGET pyqtEmbedded
#                   POST_BUILD
#                   COMMAND ${CMAKE_COMMAND}
#                   ARGS -E copy
#                        ${CMAKE_CURRENT_SOURCE_DIR}/pyQtEmbedded.py
#                        ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/pyQtEmbedded.py
#                   COMMENT "Copy of pyQTEmbedded.py to the executable directory")

else(EXISTS ${CISST_CMAKE})
  message("Cannot configure unless CISST_CMAKE is provided")

endif(EXISTS ${CISST_CMAKE})
