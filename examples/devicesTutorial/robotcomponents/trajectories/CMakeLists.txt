#
# $Id: CMakeLists.txt 75 2009-02-24 16:47:20Z adeguet1 $
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# name of project and executable
project(trajectories)

# name the main executable and specifies with source files to use
add_executable( queuelinearrn queuelinearrn.cpp )
add_executable( tracklinearrn tracklinearrn.cpp )

add_executable( queuelinearr3 queuelinearr3.cpp )
add_executable( tracklinearr3 tracklinearr3.cpp )

add_executable( queueqlqrn    queueqlqrn.cpp )

add_executable( queueslerp queueslerp.cpp )

add_executable( queuelinearse3 queuelinearse3.cpp )

add_executable( queuelinearik queuelinearik.cpp )

# link with the cisst libraries
cisst_requires( queuelinearrn "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( tracklinearrn "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( queuelinearr3 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( tracklinearr3 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( queueqlqrn "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( queueslerp "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( queuelinearse3 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")

cisst_requires( queuelinearik "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices;cisstRobot")


