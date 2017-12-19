#
# (C) Copyright 2003-2017 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstCommonQt)

# set directories for this project
cisst_set_directories (cisstQt)

# add the Qt library
cisst_add_library (
  LIBRARY cisstCommonQt
  LIBRARY_DIR cisstCommon
  FOLDER cisstCommon
  DEPENDENCIES cisstCommon
  SETTINGS     cisstQt
  SOURCE_FILES cmnClassServicesQt.cpp
               cmnLoggerQtWidget.cpp
               cmnQt.cpp
  HEADER_FILES cmnExportQt.h
               cmnLoggerQtWidget.h
               cmnQt.h)
