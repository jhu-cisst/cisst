#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2011-06-02
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
#


# make sure DeckLink was found
if (DeckLink_FOUND)

  # add the include and lib paths
  include_directories (${DeckLink_INCLUDE_DIR})

  # on Windows, create a library from idl file
  if (WIN32)
    cmake_minimum_required (VERSION 2.8.4) # for proper handling of MIDL
    set (DeckLink_IDL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/BlackMagicDeckLink.dir/${CMAKE_CFG_INTDIR}")
    include_directories (${DeckLink_IDL_BINARY_DIR})
    set (DeckLink_INCLUDE_DIR ${DeckLink_INCLUDE_DIR} ${DeckLink_IDL_BINARY_DIR})
    # create library using idl and c file generated from idl
    add_library (BlackMagicDeckLink STATIC
                 ${DeckLink_EXTRA_CODE_IDL}
		 "${DeckLink_IDL_BINARY_DIR}/DeckLinkAPI_i.c")
    set_source_files_properties ("${DeckLink_IDL_BINARY_DIR}/DeckLinkAPI_i.c" PROPERTIES GENERATED TRUE)
    # add extra libray for MS COM
    set (DeckLink_LIBRARIES ${DeckLink_LIBRARIES} comsuppw)

  else (WIN32)
    # on Mac and Linux, create a static library from cpp file 
    add_library (BlackMagicDeckLink STATIC
                 ${DeckLink_EXTRA_CODE_CPP})
  endif (WIN32)

else (DeckLink_FOUND)
  message (ERROR "Can't use DeckLink as it has not been found yet")
endif (DeckLink_FOUND)