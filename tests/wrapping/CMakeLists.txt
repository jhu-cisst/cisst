#
# $Id$
#
# CMakeLists for wrapping tests
#
# (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# save parent directory to allow sub directories to access cisstTestMain.py.in
set (cisstWrappingTests_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Check what seems available
foreach (lib ${CISST_LIBRARIES})

  # Make sure there is actually a directory of test case
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib})
    add_subdirectory (${lib})
  endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib})

endforeach (lib)

