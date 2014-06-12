#
# $Id: CMakeLists.txt 4629 2013-12-19 21:16:05Z adeguet1 $
#
# (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstStereoVision applications
project (cisstStereoVisionApplications)

# Build applications if needed
cisst_offer_application (cisstStereoVision CameraViewer ON)
cisst_offer_application (cisstStereoVision VideoPlayer ON)
cisst_offer_application (cisstStereoVision VideoConverter ON)
