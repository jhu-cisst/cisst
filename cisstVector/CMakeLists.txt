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

# All cisstVector libraries
project (cisstVectorLibs)

add_subdirectory (code)

if (CISST_HAS_FLTK AND CISST_HAS_OPENGL)
  add_subdirectory (codeFLTK)
endif (CISST_HAS_FLTK AND CISST_HAS_OPENGL)

cisst_offer_python (cisstVector)
cisst_offer_tests_python (cisstVector)

if (CISST_HAS_QT AND CISST_HAS_OPENGL)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT AND CISST_HAS_OPENGL)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstVector)
cisst_offer_tests (cisstVector)
