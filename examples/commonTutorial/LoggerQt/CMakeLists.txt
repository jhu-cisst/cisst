#
#   $Id$
#
#   Author(s):  Anton Deguet
#   Created on: 2009-11-16
#
#   (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_executable(commonTutorialLoggerQt
               main.cpp
               )

target_link_libraries(commonTutorialLoggerQt
                      ${QT_LIBRARIES}
                      )

cisst_requires(commonTutorialLoggerQt
               "cisstCommon;cisstCommonQt"
               )
