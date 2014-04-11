#
#
# CMakeLists for cisstStereoVision
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

project (cisstStereoVisionPython)

# create wrappers using swig
cisst_add_swig_module (MODULE cisstStereoVision
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstStereoVision
                       HEADER_FILES ../svlPython.h
                       MODULE_LINK_LIBRARIES cisstStereoVision
                                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                       INSTALL_FILES 1)
