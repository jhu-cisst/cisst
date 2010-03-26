# CMakeLists for prmParameterTypes
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstRobot)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector
#    cisstNumerical
    )

# all source files
SET(SOURCE_FILES
  robConstantR1.cpp
  robConstantSO3.cpp
  robLinear.cpp
  robMapping.cpp
  robQuintic.cpp
  robSigmoid.cpp
  robSLERP.cpp
  robCasteljauSO3.cpp
  robSpace.cpp
#  robTrackSE3.cpp
  robTrajectory.cpp
  robTrajectoryR1.cpp
  robTrajectorySO3.cpp
  robTrajectoryManager.cpp
  robVariable.cpp
  
  robJoint.cpp
  robMass.cpp
  
  robDH.cpp
  robLink.cpp
  robManipulator.cpp
  robControlLoop.cpp  
  )

# all header files
SET(HEADER_FILES
  robConstantR1.h
  robConstantSO3.h
  robFunction.h
  robLinear.h
  robMapping.h
  robQuintic.h
  robSigmoid.h
  robSLERP.h
  robCasteljauSO3.h
  robSpace.h
  robTrackSE3.h
  robTrajectory.h
  robTrajectoryR1.h
  robTrajectorySO3.h
  robTrajectoryManager.h
  robVariable.h
  
  robMass.h
  robJoint.h
  
  robDH.h
  robLink.h
  robManipulator.h
  robControlLoop.h  
  )

# use for findblas and findlapack
enable_language( Fortran )

include( FindBLAS )
if( BLAS_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES BLAS_LIBRARIES )
endif( BLAS_FOUND )
  
include( FindLAPACK )
if( LAPACK_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES LAPACK_LIBRARIES )
endif( LAPACK_FOUND )
  

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


