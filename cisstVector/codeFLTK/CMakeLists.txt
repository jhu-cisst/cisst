#
#
# (C) Copyright 2003-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstVectorFLTK)

# set directories for this project
cisst_set_directories (cisstFLTK)

# add the FLTK library
cisst_add_library (
  LIBRARY cisstVectorFLTK
  LIBRARY_DIR cisstVector
  FOLDER cisstVector
  DEPENDENCIES cisstCommon
               cisstVector
  SETTINGS     cisstFLTK
  SOURCE_FILES vctPlot2DOpenGLFLTK.cpp
  HEADER_FILES vctPlot2DOpenGLFLTK.h
               vctExportFLTK.h)

# link against FLTK libraries
target_link_libraries (cisstVectorFLTK ${FLTK_LIBRARIES})
