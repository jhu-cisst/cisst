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


if(CISST_DEV_HAS_ODE )

  project( ode )

  add_executable( ODEtest ODEtest.cpp )
  add_executable( ODEstereo ODEstereo.cpp )

  cisst_target_link_libraries( ODEtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

  cisst_target_link_libraries( ODEstereo cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

  if( BUILD_LIBS_cisstStereoVision )

    add_executable( ODEsvl ODEsvl.cpp )
    cisst_target_link_libraries( ODEsvl cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask cisstStereoVision)

  endif( BUILD_LIBS_cisstStereoVision )

endif( CISST_DEV_HAS_ODE )





