#
# $Id$
#
# (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_executable (cisstNDISerialQtExample
                mainQt.cpp
                )

target_link_libraries (cisstNDISerialQtExample
                       cisstNDISerial
                       cisstNDISerialQt
                       ${QT_LIBRARIES}
                       )

cisst_target_link_libraries (cisstNDISerialQtExample
                             cisstCommon cisstCommonXML cisstOSAbstraction cisstMultiTask cisstParameterTypes cisstRobot
                             )
