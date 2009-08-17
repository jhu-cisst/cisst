#
# $Id$
#
# (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

project(devicesTutorial)

add_subdirectory(example2)

if(CISST_DEV_HAS_SENSABLEHD OR CISST_MTS_HAS_ICE)
  add_subdirectory(example1)
endif(CISST_DEV_HAS_SENSABLEHD OR CISST_MTS_HAS_ICE)

if(CISST_DEV_HAS_SENSABLEHD)
  add_subdirectory(example3)
endif(CISST_DEV_HAS_SENSABLEHD)

if(CISST_DEV_HAS_OPENIGTLINK)
  add_subdirectory(example4)
endif(CISST_DEV_HAS_OPENIGTLINK)
