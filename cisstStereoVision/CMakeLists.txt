#
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstStereoVision libraries
project (cisstStereoVisionLibs)

add_subdirectory (code)

# Applications
add_subdirectory (applications)

cisst_offer_python (cisstStereoVision)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

cisst_offer_examples (cisstStereoVision)
