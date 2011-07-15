#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

find_path (Stealthlink_INCLUDE_DIRS NAMES "AsCL/AsCL_Client.h")
find_library (Stealthlink_GTK_LIBRARY AsCL_GTK PATHS "${Stealthlink_INCLUDE_DIRS}/i686-linux-gnu3")
find_library (Stealthlink_XT_LIBRARY AsCL_Xt PATHS "${Stealthlink_INCLUDE_DIRS}/i686-linux-gnu3")

set (Stealthlink_FOUND FALSE)
if (Stealthlink_INCLUDE_DIRS AND Stealthlink_GTK_LIBRARY AND Stealthlink_XT_LIBRARY)
  set (Stealthlink_FOUND TRUE)
  set (Stealthlink_LIBRARIES ${Stealthlink_GTK_LIBRARY} ${Stealthlink_XT_LIBRARY})
endif (Stealthlink_INCLUDE_DIRS AND Stealthlink_GTK_LIBRARY AND Stealthlink_XT_LIBRARY)

mark_as_advanced (Stealthlink_INCLUDE_DIRS
                  Stealthlink_GTK_LIBRARY
                  Stealthlink_XT_LIBRARY
                  Stealthlink_LIBRARIES)
