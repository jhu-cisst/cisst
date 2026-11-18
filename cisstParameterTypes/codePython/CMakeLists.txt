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

cisst_add_pybind11_module (MODULE cisstParameterTypesPython
                           FOLDER cisstParameterTypes
                           SOURCES cisstParameterTypesPybind.cpp ${cisstParameterTypes_CISST_DG_PYTHON_SRCS}
                           MODULE_LINK_LIBRARIES cisstParameterTypes
                                                 cisstMultiTask
                                                 cisstOSAbstraction
                                                 cisstCommon
                           INSTALL_FILES)
