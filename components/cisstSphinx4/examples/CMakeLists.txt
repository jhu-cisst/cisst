#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# example with Qt
get_filename_component (JAVA_LIBRARY_DIR ${JAVA_AWT_LIBRARY} PATH)
link_directories (${JAVA_LIBRARY_DIR}/xawt)

add_executable (cisstSphinx4ExampleQt cisstSphinx4ExampleQt.cpp)
target_link_libraries (cisstSphinx4ExampleQt
                       cisstSphinx4 cisstSphinx4Qt ${QT_LIBRARIES})
cisst_target_link_libraries (cisstSphinx4ExampleQt
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
