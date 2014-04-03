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

project (cisstCommonPython)

# create wrappers using swig
cisst_add_swig_module (MODULE cisstCommon
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstCommon
                       HEADER_FILES ../cmnPython.h
                       MODULE_LINK_LIBRARIES cisstCommon
                       INSTALL_FILES 1)
