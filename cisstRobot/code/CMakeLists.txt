#
# (C) Copyright 2003-2021 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# set dependencies
set (DEPENDENCIES cisstCommon cisstVector cisstNumerical)

if (CISST_HAS_CISSTNETLIB)

  set (SOURCE_FILES
       robFunction.cpp
       robFunctionSO3.cpp
       robFunctionSE3.cpp
       robFunctionRn.cpp

       robLinearRn.cpp
       robQuintic.cpp
       robQLQRn.cpp

       robSLERP.cpp
       robCubicSO3.cpp

       robLinearSE3.cpp
       #  robCSCSO3.cpp

       robLSPB.cpp

       robJoint.cpp
       robMass.cpp

       robKinematics.cpp
       robDH.cpp
       robHayati.cpp
       robModifiedDH.cpp
       robModifiedHayati.cpp
       robLink.cpp
       robManipulator.cpp

#    robComputedTorque.cpp
#    robPD.cpp

      )

  # all header files
  set (HEADER_FILES
       robExport.h

       robFunction.h
       robFunctionRn.h
       robFunctionSE3.h
       robFunctionSO3.h

       robLinearRn.h
       robLinearSE3.h
       robQuintic.h
       robQLQRn.h
       robSLERP.h
       robCubicSO3.h
       #  robCSCSO3.h

       robLSPB.h

       robMass.h
       robJoint.h

       robKinematics.h
       robDH.h
       robHayati.h
       robModifiedDH.h
       robModifiedHayati.h
       robLink.h
       robManipulator.h

#    robControllerJoints.h
#    robComputedTorque.h
#    robPD.h

      )

  option (CISST_ROB_HAS_REFLEXXES_TYPEII "Compile cisstRobot with TypeIIRML wrappers" ON)

  if (CISST_ROB_HAS_REFLEXXES_TYPEII)
    include (ExternalProject)

    # download and build options
    set (cisstReflexxesTypeII_PREFIX Reflexxes)
    set (cisstReflexxesTypeII_GIT_REPOSITORY https://github.com/jhu-cisst-external/ReflexxesTypeII.git)
    set (cisstReflexxesTypeII_GIT_TAG 1.2.6.p1)

    ExternalProject_Add (cisstReflexxesTypeIIExternal
                         PREFIX ${cisstReflexxesTypeII_PREFIX}
                         GIT_REPOSITORY    ${cisstReflexxesTypeII_GIT_REPOSITORY}
                         GIT_TAG           ${cisstReflexxesTypeII_GIT_TAG}
                         CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                          -DCMAKE_INSTALL_PREFIX:FILEPATH=${cisst_BINARY_DIR}/cisstReflexxesTypeII
                                          -DLIBRARY_OUTPUT_PATH:FILEPATH=${LIBRARY_OUTPUT_PATH}
                         INSTALL_DIR ${cisst_BINARY_DIR}/cisstReflexxesTypeII
                         UPDATE_COMMAND ""
                         )
    # set all cisstReflexxesTypeII variables based on install directory
    set (cisstReflexxesTypeII_INCLUDE_DIR "${cisst_BINARY_DIR}/cisstReflexxesTypeII/include/ReflexxesTypeII")
    set (cisstReflexxesTypeII_LIBRARY_DIRS "${cisst_BINARY_DIR}/cisstReflexxesTypeII/lib")
    set (cisstReflexxesTypeII_LIBRARIES ReflexxesTypeII)
    # save for external packages
    cisst_set_package_settings (cisstRobot cisstReflexxesTypeII INCLUDE_DIRECTORIES cisstReflexxesTypeII_INCLUDE_DIR)
    cisst_set_package_settings (cisstRobot cisstReflexxesTypeII LIBRARIES           cisstReflexxesTypeII_LIBRARIES)
    cisst_set_package_settings (cisstRobot cisstReflexxesTypeII LINK_DIRECTORIES    cisstReflexxesTypeII_LIBRARY_DIRS)
    # add wrappers to cisstRobot
    set (SOURCE_FILES ${SOURCE_FILES} robReflexxes.cpp)
    set (HEADER_FILES ${HEADER_FILES} robReflexxes.h)
    # install for CPack
    install (FILES ${cisst_BINARY_DIR}/cisstReflexxesTypeII/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ReflexxesTypeII${CMAKE_SHARED_LIBRARY_SUFFIX}
             COMPONENT cisstRobot
             DESTINATION lib)
  else ()
    cisst_unset_all_package_settings (cisstRobot cisstReflexxesTypeII)
  endif ()

  cisst_add_library (LIBRARY cisstRobot
                     FOLDER cisstRobot
                     DEPENDENCIES ${DEPENDENCIES}
                     SOURCE_FILES ${SOURCE_FILES}
                     HEADER_FILES ${HEADER_FILES})

  if (CISST_ROB_HAS_REFLEXXES_TYPEII)
    add_dependencies (cisstRobot cisstReflexxesTypeIIExternal)
  endif ()

else (CISST_HAS_CISSTNETLIB)
  message ("cisstRobot requires cisstNumerical to be compiled with the option CISST_HAS_CISSTNETLIB")
  set (CISST_cisstRobot OFF CACHE BOOL "Build cisstRobot" FORCE)
endif (CISST_HAS_CISSTNETLIB)
