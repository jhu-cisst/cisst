#
# (C) Copyright 2003-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# set dependencies
set (DEPENDENCIES cisstCommon)

set (SOURCE_FILES
      kdl_utilities.cpp

      robJacobian.cpp
      robLinkDynamics.cpp
      robLinkKinematics.cpp
      robModifiedDH.cpp
      robStandardDH.cpp
    )

# all header files
set (HEADER_FILES
      robExport.h

      kdl_utilities.h

      robJacobian.h
      robLinkDynamics.h
      robLinkKinematics.h
      robModifiedDH.h
      robStandardDH.h
    )

option (CISST_ROB_HAS_REFLEXXES_TYPEII "Compile cisstRobot with TypeIIRML wrappers" ON)

if (CISST_ROB_HAS_REFLEXXES_TYPEII)
  include (ExternalProject)

  # download and build options
  set (cisstReflexxesTypeII_PREFIX Reflexxes)
  set (cisstReflexxesTypeII_GIT_REPOSITORY https://github.com/jhu-cisst-external/ReflexxesTypeII.git)
  set (cisstReflexxesTypeII_GIT_TAG 1.2.6.p2)

  ExternalProject_Add (cisstReflexxesTypeIIExternal
                        PREFIX ${cisstReflexxesTypeII_PREFIX}
                        GIT_REPOSITORY    ${cisstReflexxesTypeII_GIT_REPOSITORY}
                        GIT_TAG           ${cisstReflexxesTypeII_GIT_TAG}
                        CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                        -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                                        -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                                        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                        -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
                                        -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstReflexxesTypeII
                                        -DLIBRARY_OUTPUT_PATH:FILEPATH=${LIBRARY_OUTPUT_PATH}
                        INSTALL_DIR ${cisst_BINARY_DIR}/cisstReflexxesTypeII
                        UPDATE_COMMAND ""
                        )
  # set all cisstReflexxesTypeII variables based on install directory
  set (cisstReflexxesTypeII_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstReflexxesTypeII/include/ReflexxesTypeII")
  set (cisstReflexxesTypeII_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstReflexxesTypeII/lib")
  file (MAKE_DIRECTORY "${cisstReflexxesTypeII_INCLUDE_DIR}")
  set (cisstReflexxesTypeII_LIBRARIES ReflexxesTypeII)
  # add wrappers to cisstRobot
  set (SOURCE_FILES ${SOURCE_FILES} robReflexxes.cpp)
  set (HEADER_FILES ${HEADER_FILES} robReflexxes.h)
  # install for CPack
  if (WIN32)
    install (FILES ${cisst_BINARY_DIR}/cisstReflexxesTypeII/lib/$<CONFIG>/${CMAKE_SHARED_LIBRARY_PREFIX}ReflexxesTypeII${CMAKE_SHARED_LIBRARY_SUFFIX}
              COMPONENT cisstRobot
              DESTINATION lib
              OPTIONAL)
  else ()
    install (FILES ${cisst_BINARY_DIR}/cisstReflexxesTypeII/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ReflexxesTypeII${CMAKE_SHARED_LIBRARY_SUFFIX}
              COMPONENT cisstRobot
              DESTINATION lib
              OPTIONAL)
  endif ()
endif ()

cisst_add_library (LIBRARY cisstRobot
                    FOLDER cisstRobot
                    DEPENDENCIES ${DEPENDENCIES}
                    SOURCE_FILES ${SOURCE_FILES}
                    HEADER_FILES ${HEADER_FILES})

if (CISST_ROB_HAS_REFLEXXES_TYPEII)
  add_dependencies (cisstRobot cisstReflexxesTypeIIExternal)
  target_include_directories (cisstRobot PRIVATE ${cisstReflexxesTypeII_INCLUDE_DIR})
  target_link_directories (cisstRobot PRIVATE ${cisstReflexxesTypeII_LIBRARY_DIRS})
  target_link_libraries (cisstRobot ${cisstReflexxesTypeII_LIBRARIES})
endif ()
