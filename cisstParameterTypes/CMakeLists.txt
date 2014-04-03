#
#
# (C) Copyright 2005-2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstParameterTypes libraries
project (cisstParameterTypesLibs)

add_subdirectory (code)

# SWIG Python wrappers
cisst_offer_python (cisstParameterTypes)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstParameterTypes)
cisst_offer_tests (cisstParameterTypes)
