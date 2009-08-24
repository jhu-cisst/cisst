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
    )

# all source files
SET(SOURCE_FILES
	robRnBlender.cpp
	robSO3Blender.cpp
	robRnConstant.cpp
	robSE3Constant.cpp
	robDOF.cpp
	robFunctionPiecewise.cpp
	robHermite.cpp
	robLinear.cpp
	robSigmoid.cpp
	robSLERP.cpp
	robTrajectory.cpp
	robCollaborativeControlForce.cpp
    )

# all header files
SET(HEADER_FILES
	robRnBlender.h
	robSO3Blender.h
	robRnConstant.h
	robSE3Constant.h
	robDOF.h
	robFunction.h	
	robFunctionPiecewise.h
	robHermite.h
	robLinear.h
	robSigmoid.h
	robSLERP.h
	robTrajectory.h
	robCollaborativeControlForce.h
	robExport.h
    )

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)
