#
#
# (C) Copyright 2005-2013 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_subdirectory (tutorial)
add_subdirectory (PeriodicTask)
add_subdirectory (CommandsAndEvents)
add_subdirectory (ConnectionTest)
if (${CISST_cisstParameterTypes} STREQUAL ON)
  add_subdirectory (Benchmarks)
  add_subdirectory (2DPlotVisualizer)
endif()
