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

project (cisstParameterTypesPython)

cisst_add_swig_module (MODULE cisstParameterTypes
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstParameterTypes
                       HEADER_FILES ../prmPython.h
                       MODULE_LINK_LIBRARIES cisstParameterTypes
                                             cisstMultiTask
                                             cisstOSAbstraction
                                             cisstVector
                                             cisstCommon
                       INSTALL_FILES 1)
