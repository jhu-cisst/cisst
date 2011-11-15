#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (CISST_cisstDevices)

  if (CISST_DEV_HAS_ROBOTCOMPONENTS)
    #add_subdirectory (robotcomponents)
  else (CISST_DEV_HAS_ROBOTCOMPONENTS)
      message ("Information: cisstDevices examples robotcomponents will not be compiled since they require CISST_DEV_HAS_ROBOTCOMPONENTS")
  endif (CISST_DEV_HAS_ROBOTCOMPONENTS)

else (CISST_cisstDevices)
  message ("Information: devicesTutorial examples will not be compiled since they require CISST_Devices")
endif (CISST_cisstDevices)