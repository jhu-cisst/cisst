#
# $Id$
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

project(multiTaskTutorial)

add_subdirectory(example1)
add_subdirectory(example2)
add_subdirectory(example3)
add_subdirectory(example6)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory(example4)
  add_subdirectory(example5)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_MTS_HAS_ICE)
  add_subdirectory(example7)
  add_subdirectory(example8)
  add_subdirectory(example9)
endif (CISST_MTS_HAS_ICE)

