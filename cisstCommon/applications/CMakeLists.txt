#
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstCommon applications
project (cisstCommonApplications)

# data generator should always be compiled so we don't use cisst_offer_application
if (NOT CMAKE_CROSSCOMPILING)
  add_subdirectory (cisstDataGenerator)
endif (NOT CMAKE_CROSSCOMPILING)
