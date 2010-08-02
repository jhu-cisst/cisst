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
SET( LIBRARY cisstRobot)

# set dependencies
SET( DEPENDENCIES cisstCommon cisstVector )

# all source files
SET(
  
  SOURCE_FILES

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
  
  robJoint.cpp
  robMass.cpp
  
  robDH.cpp
  robLink.cpp
  robManipulator.cpp

  robCollaborativeControlForce.cpp

  BH/robBH.cpp
  BH/robBHF1.cpp
  BH/robBHF2.cpp
  BH/robBHF3.cpp

  SAH/robSAH.cpp
  SAH/robSAHFinger.cpp
  SAH/robSAHThumb.cpp

  robComputedTorque.cpp
  robPD.cpp

  )

# all header files
SET(HEADER_FILES

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
  
  robMass.h
  robJoint.h
  
  robDH.h
  robLink.h
  robManipulator.h

  robCollaborativeControlForce.h

  BH/robBH.h
  BH/robBHF1.h
  BH/robBHF2.h
  BH/robBHF3.h

  SAH/robSAH.h
  SAH/robSAHFinger.h
  SAH/robSAHThumb.h

  robControllerJoints.h
  robComputedTorque.h
  robPD.h

  )

# use for findblas and findlapack
#enable_language( Fortran )

#include( FindBLAS )
#if( BLAS_FOUND )
#    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES BLAS_LIBRARIES )
#endif( BLAS_FOUND )
  
#include( FindLAPACK )
#if( LAPACK_FOUND )
#    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES LAPACK_LIBRARIES )
#endif( LAPACK_FOUND )
  

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


