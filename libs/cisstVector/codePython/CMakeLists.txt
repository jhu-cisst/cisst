#
# $Id$
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

project (cisstVectorPython)

cisst_add_swig_module(MODULE cisstVector
                      INTERFACE_DIRECTORY ..
                      CISST_LIBRARIES cisstCommon cisstVector)

