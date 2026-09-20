#
# Author(s):  Anton Deguet
# Created on: 2010-08-11
#
# (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#

message (DEPRECATION
  "Usecisst.cmake is deprecated. Use find_package(cisst REQUIRED COMPONENTS ...) "
  "and link targets with target_link_libraries(... ${cisst_LIBRARIES}) or cisst:: targets.")
