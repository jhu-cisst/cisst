#
# $Id: CMakeLists.txt 495 2009-06-23 15:37:24Z adeguet1 $
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

add_library (3DUserInterfaceTutorialBehaviors
	     SimpleBehavior.h
	     SimpleBehavior.cpp
	     MeasurementBehavior.h
	     MeasurementBehavior.cpp
	     )

CISST_REQUIRES(3DUserInterfaceTutorialBehaviors "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstRobot;cisstParameterTypes;cisst3DUserInterface")
