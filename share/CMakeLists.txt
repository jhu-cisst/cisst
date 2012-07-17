#
# $Id$
#
# (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (data)

# to determine the shared directory path to build
find_package (cisst REQUIRED)

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # directory to copy files
  set (_directoryToCopyTo "${CISST_BINARY_DIR}/${CISST_SHARE_INSTALL_SUFFIX}")

  set (_directorieToBeCopied "models" "images" "movies")
  foreach (_directory ${_directorieToBeCopied})
    file (MAKE_DIRECTORY "${_directoryToCopyTo}/${_directory}")
    add_custom_target (copy_data_${_directory} ALL
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                               "${CMAKE_CURRENT_SOURCE_DIR}/${_directory}"
                               "${_directoryToCopyTo}/${_directory}"
                       COMMENT "Copying data files from ${_directory} to ${_directoryToCopyTo}/${_directory}")
  endforeach (_directory)

endif (cisst_FOUND)