#
# $Id$
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

# name of project
PROJECT(multiTaskTutorialBenchmark1)

# Set include/link directories
cisst_set_directories (cisstCommon cisstOSAbstraction cisstMultiTask cisstParameterTypes)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskTutorialBenchmark1Local
               clientTask.cpp
               serverTask.cpp
               localMain.cpp
               clientTask.h
               serverTask.h
               configuration.h
               )

# link with the cisst libraries
cisst_target_link_libraries(multiTaskTutorialBenchmark1Local cisstCommon cisstOSAbstraction cisstMultiTask cisstParameterTypes)


# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  # name the main executable and specifies with source files to use
  ADD_EXECUTABLE(multiTaskTutorialBenchmark1Server
                 serverTask.cpp
                 serverMain.cpp
                 serverTask.h
                 configuration.h
                 )          

  # link with the cisst libraries
  cisst_target_link_libraries(multiTaskTutorialBenchmark1Server cisstCommon cisstOSAbstraction cisstMultiTask cisstParameterTypes)

  # name the main executable and specifies with source files to use
  ADD_EXECUTABLE(multiTaskTutorialBenchmark1Client
                 clientTask.cpp
                 clientMain.cpp
                 clientTask.h
                 configuration.h
                 )

  # link with the cisst libraries
  cisst_target_link_libraries(multiTaskTutorialBenchmark1Client cisstCommon cisstOSAbstraction cisstMultiTask cisstParameterTypes)

endif (CISST_MTS_HAS_ICE)
