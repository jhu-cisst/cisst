#
# $Id$
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.

add_executable (cisstISIAPIExample
                main.cpp)

cisst_target_link_libraries (cisstISIAPIExample
                             cisstCommon
                             cisstVector
                             cisstOSAbstraction
                             cisstMultiTask
                             cisstRobot
                             cisstParameterTypes)

target_link_libraries (cisstISIAPIExample
                       cisstISIAPI
                       ${isi_api_LIBRARIES})
