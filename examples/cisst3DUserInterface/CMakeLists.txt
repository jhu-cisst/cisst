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

if (BUILD_LIBS_cisst3DUserInterface)

  find_package (VTK REQUIRED)
  include (${VTK_USE_FILE})

  # library of sample behavior
  include_directories(behaviors)
  add_subdirectory(behaviors)

  # programs, depends on hardware availability

  # Sensable specific examples, requires cisstDevices
  if (CISST_DEV_HAS_SENSABLEHD)
    add_subdirectory(SensableNoVideo)
    add_subdirectory(SensableStereoVideo)
    add_subdirectory(SensableStereoVideoNetwork)
  else (CISST_DEV_HAS_SENSABLEHD)
    message (WARNING " cisst3DUserInterface examples SensableNoVideo, SensableStereoVideo, and SensableStereoVideoNetwork will not be compiled since they require CISST_DEV_HAS_SENSABLEHD")
  endif (CISST_DEV_HAS_SENSABLEHD)

  # Intuitive Surgical Inc daVinci - requires Read Only API.  Contact cisst devel group.
  option (CISST_UI3_HAS_DAVINCI "Build 3DUserInterface example using the daVinci API" OFF)
  if (CISST_UI3_HAS_DAVINCI)
    option (CISST_UI3_HAS_DAVINCI_BBAPI "Use the daVinci BBAPI" OFF)
    if (CISST_UI3_HAS_DAVINCI_BBAPI)
      find_package (cisstDaVinci-isi-bbapi)
      include_directories (${cisstDaVinci-isi-bbapi_INCLUDE_DIR})
      link_directories (${cisstDaVinci-isi-bbapi_LIBRARY_DIR})
    endif (CISST_UI3_HAS_DAVINCI_BBAPI)
    set(daVinci_ALL_LIBRARIES ${daVinci_LIBRARIES} ${BBAPI_LIBRARIES} ${cisstDaVinci-isi-bbapi_LIBRARIES})
    add_subdirectory(daVinciStereoVideo)
  else (CISST_UI3_HAS_DAVINCI)
    message (WARNING " cisst3DUserInterface example daVinciStereoVideo will not be compiled since it requires CISST_UI3_HAS_DAVINCI")
  endif (CISST_UI3_HAS_DAVINCI)

else (BUILD_LIBS_cisst3DUserInterface)
  message (WARNING " cisst3DUserInterface examples will not be compiled since they require BUILD_LIBS_3DUserInterface")
endif (BUILD_LIBS_cisst3DUserInterface)
