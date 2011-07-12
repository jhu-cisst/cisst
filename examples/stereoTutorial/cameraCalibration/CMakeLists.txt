#
# $Id: CMakeLists.txt 1660 2010-07-22 20:22:54Z adeguet1 $
#
# (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

IF(CISST_SVL_HAS_X11 OR WIN32)

	IF(CISST_SVL_HAS_OPENCV OR CISST_SVL_HAS_OPENCV2)
		ADD_EXECUTABLE(svlExCameraCalibration cameraCalibration.cpp)

		CISST_REQUIRES("svlExCameraCalibration" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision")
	ENDIF(CISST_SVL_HAS_OPENCV OR CISST_SVL_HAS_OPENCV2)
	
ENDIF(CISST_SVL_HAS_X11 OR WIN32)

