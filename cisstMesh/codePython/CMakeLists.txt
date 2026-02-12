#
# CMakeLists for cisstMesh
#
# (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstMeshPython)

cisst_add_swig_module (MODULE cisstMesh
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstMesh
                       HEADER_FILES ../mshPython.h
                       MODULE_LINK_LIBRARIES cisstCommon cisstVector cisstNumerical cisstMesh
                       INSTALL_FILES 1)
