#
# $Id$
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


PROJECT(cisstLibs)


# Set the ouptut path for the libraries and executables
SET(LIBRARY_OUTPUT_PATH ${cisstLibs_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${cisstLibs_BINARY_DIR}/bin)


# Interactive Research Environment meaningless without Python wrapping
VARIABLE_REQUIRES(CISST_BUILD_LIBS_cisstInteractive REQUIREMENT_MET CISST_HAS_SWIG_PYTHON)


# Build everything
FOREACH(lib ${CISST_LIBS})
  IF(CISST_BUILD_LIBS_${lib})
    ADD_SUBDIRECTORY(code/${lib})
  ENDIF(CISST_BUILD_LIBS_${lib})
ENDFOREACH(lib)
