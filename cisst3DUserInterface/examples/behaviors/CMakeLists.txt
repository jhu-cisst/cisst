#
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


add_library (ui3ExBehaviors
             ${IS_SHARED}
             ui3BehaviorsExport.h
             SimpleBehavior.h
             SimpleBehavior.cpp
             MeasurementBehavior.h
             MeasurementBehavior.cpp
             MapBehavior.h
             MapBehavior.cpp
             ImageViewer.h
             ImageViewer.cpp
             ImageViewerKidney.h
             ImageViewerKidney.cpp
             RegistrationBehavior.h
             RegistrationBehavior.cpp
             ManualRegistration.h
             ManualRegistration.cpp
             PNGViewer3D.h
             PNGViewer3D.cpp
             VideoViewer3D.h
             VideoViewer3D.cpp
             )
set_property (TARGET ui3ExBehaviors PROPERTY FOLDER "cisst3DUserInterface/examples")

cisst_target_link_libraries (ui3ExBehaviors
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                             cisstStereoVision cisstRobot cisstParameterTypes cisst3DUserInterface)
