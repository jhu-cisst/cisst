#
# (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights
# Reserved.
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

#    BH/robBH.cpp
#    BH/robBHF1.cpp
#    BH/robBHF2.cpp
#    BH/robBHF3.cpp

#    SAH/robSAH.cpp
#    SAH/robSAHFinger.cpp
#    SAH/robSAHThumb.cpp

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

#    BH/robBH.h
#    BH/robBHF1.h
#    BH/robBHF2.h
#    BH/robBHF3.h

#    SAH/robSAH.h
#    SAH/robSAHFinger.h
#    SAH/robSAHThumb.h

#    robControllerJoints.h
#    robComputedTorque.h
#    robPD.h

      )

  cisst_add_library (LIBRARY cisstRobot
                     FOLDER cisstRobot
                     DEPENDENCIES ${DEPENDENCIES}
                     SOURCE_FILES ${SOURCE_FILES}
                     HEADER_FILES ${HEADER_FILES})

else (CISST_HAS_CISSTNETLIB)
  message ("cisstRobot requires cisstNumerical to be compiled with the option CISST_HAS_CISSTNETLIB")
  set (CISST_cisstRobot OFF CACHE BOOL "Build cisstRobot" FORCE)
endif (CISST_HAS_CISSTNETLIB)
