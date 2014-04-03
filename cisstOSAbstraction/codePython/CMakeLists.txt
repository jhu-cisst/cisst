#
#
# CMakeLists for cisstOSAbstraction
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

project (cisstOSAbstractionPython)

cisst_add_swig_module (MODULE cisstOSAbstraction
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstOSAbstraction
                       HEADER_FILES ../osaPython.h
                       MODULE_LINK_LIBRARIES cisstCommon cisstOSAbstraction
                       INSTALL_FILES 1)
