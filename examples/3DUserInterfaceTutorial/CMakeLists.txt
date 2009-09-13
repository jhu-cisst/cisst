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

project(3DUserInterfaceTutorial)

# library of sample behavior
add_subdirectory(behaviors)

include_directories(behaviors)

# programs, depends on hardware availability

# Sensable specific examples, requires cisstDevices
if (CISST_DEV_HAS_SENSABLEHD)
    add_subdirectory(example1)
    add_subdirectory(example2)
endif (CISST_DEV_HAS_SENSABLEHD)

# Intuitive Surgical Inc daVinci - requires Read Only API.  Contact cisst devel group.
option (CISST_UI3_HAS_DAVINCI "Build 3DUserInterface example using the daVinci API" OFF)
if (CISST_UI3_HAS_DAVINCI)
    find_path (daVinci_INCLUDE_DIRS cisstDaVinciAPI.h)
    if (daVinci_INCLUDE_DIRS)
        include_directories(${daVinci_INCLUDE_DIRS})
    endif (daVinci_INCLUDE_DIRS)
    find_library (daVinci_LIBRARIES cisstDaVinciAPI)
    add_subdirectory(example3)
    add_subdirectory(example4)
endif (CISST_UI3_HAS_DAVINCI)
