#
# $Id$
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

# All cisstStereoVision libraries
project (cisstStereoVisionLibs)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)

cisst_offer_examples (cisstStereoVision)
