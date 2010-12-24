#
# $Id$
#
# (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (BUILD_LIBS_cisstCommon)
  add_subdirectory (getChar)
  add_subdirectory (portability)
  add_subdirectory (serialization)
  add_subdirectory (units)

  if (CISST_HAS_QT)
    add_subdirectory (LoggerQt)
  else (CISST_HAS_QT)
    message (WARNING " commonTutorial example LoggerQt will not be compiled since it requires CISST_HAS_QT")
  endif (CISST_HAS_QT)

else (BUILD_LIBS_cisstCommon)
  message (WARNING " commonTutorial examples will not be compiled since they require BUILD_LIBS_cisstCommon")
endif (BUILD_LIBS_cisstCommon)