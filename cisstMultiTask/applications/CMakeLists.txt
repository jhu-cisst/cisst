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

# All cisstMultiTask applications
project (cisstMultiTaskApplications)

# Build applications if needed
cisst_offer_application (cisstMultiTask ComponentGenerator OFF)
cisst_offer_application (cisstMultiTask ComponentManager ON)
cisst_offer_application (cisstMultiTask Logger ON)

# ICE dependent applications
if (CISST_MTS_HAS_ICE)
  cisst_offer_application (cisstMultiTask GlobalComponentManager ON)
  cisst_offer_application (cisstMultiTask ComponentViewer ON)
  # FLTK dependent applications
  if (CISST_HAS_FLTK)
    cisst_offer_application (cisstMultiTask GlobalComponentManagerFLTK ON)
  else (CISST_HAS_FLTK)
    cisst_do_not_offer_application (cisstMultiTask GlobalComponentManagerFLTK)
  endif (CISST_HAS_FLTK)
else (CISST_MTS_HAS_ICE)
  cisst_do_not_offer_application (cisstMultiTask GlobalComponentManager)
  cisst_do_not_offer_application (cisstMultiTask GlobalComponentManagerFLTK)
endif (CISST_MTS_HAS_ICE)

# Python dependent applications
if (CISST_HAS_SWIG_PYTHON)
  cisst_offer_application (cisstMultiTask IREComponent ON)
else (CISST_HAS_SWIG_PYTHON)
  cisst_do_not_offer_application (cisstMultiTask IREComponent)
endif (CISST_HAS_SWIG_PYTHON)
