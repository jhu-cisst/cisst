#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# Support for OpenGL
find_package (OpenGL)
if (OPENGL_FOUND)
  option (CISST_HAS_OPENGL "Is OpenGL available." ON)
  mark_as_advanced (CISST_HAS_OPENGL)
else (OPENGL_FOUND)
  option (CISST_HAS_OPENGL "Is OpenGL available." OFF)
endif (OPENGL_FOUND)

if (CISST_HAS_OPENGL)
  find_package (OpenGL REQUIRED)
  if (OPENGL_INCLUDE_DIR)
    cisst_set_package_settings (cisstOpenGL OpenGL INCLUDE_DIRECTORIES OPENGL_INCLUDE_DIR)
  endif (OPENGL_INCLUDE_DIR)
  cisst_set_package_settings (cisstOpenGL OpenGL LIBRARIES OPENGL_LIBRARIES)
else (CISST_HAS_OPENGL)
  cisst_unset_all_package_settings (cisstOpenGL OpenGL)
endif (CISST_HAS_OPENGL)


# Support for QT
set (CISST_QT_COMPONENTS QtCore QtGui QtOpenGL QtXML QtXMLPatterns)
find_package (Qt4 REQUIRED ${CISST_QT_COMPONENTS})
if (QT_FOUND)
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" ON)
  mark_as_advanced (CISST_HAS_QT)
else (QT_FOUND)
  option (CISST_HAS_QT "Is Qt available.  If not, Qt widgets and some examples won't be compiled" OFF)
endif (QT_FOUND)

# if detected or if user requests it
if (CISST_HAS_QT)
  # Qt is now required so we can include the QT_USE_FILE
  find_package (Qt4 REQUIRED ${CISST_QT_COMPONENTS})
  include (${QT_USE_FILE})
  set (CISST_QT_PACKAGE "Qt4")
  cisst_set_package_settings (cisstQt Qt PACKAGES           CISST_QT_PACKAGE)
  cisst_set_package_settings (cisstQt Qt PACKAGE_COMPONENTS CISST_QT_COMPONENTS)
  cisst_set_package_settings (cisstQt Qt CMAKE_FILES        QT_USE_FILE)
  cisst_set_package_settings (cisstQt Qt LIBRARIES          QT_LIBRARIES)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # XML Qt based
  option (CISST_HAS_QT_XML "Use Qt XML and Patterns for cmnXMLPath" ON)
  mark_as_advanced (CISST_HAS_QT_XML)
else (CISST_HAS_QT)
  cisst_unset_all_package_settings (cisstQt Qt)
endif (CISST_HAS_QT)


# Support for FLTK
find_package (FLTK)
if (FLTK_FOUND)
  option (CISST_HAS_FLTK "Is FLTK available.  If not, some examples and utilities won't be compiled" ON)
  mark_as_advanced (CISST_HAS_FLTK)
else (FLTK_FOUND)
  option (CISST_HAS_FLTK "Is FLTK available.  If not, some examples and utilities won't be compiled" OFF)
endif (FLTK_FOUND)

if (CISST_HAS_FLTK)
  # OpenGL
  set (CISST_HAS_OPENGL ON CACHE BOOL "Is OpenGL available." FORCE)
  mark_as_advanced (CISST_HAS_OPENGL)
  # FLTK is now required
  find_package (FLTK REQUIRED)
  cisst_set_package_settings (cisstFLTK FLTK INCLUDE_DIRECTORIES FLTK_INCLUDE_DIR)
  cisst_set_package_settings (cisstFLTK FLTK LIBRARIES           FLTK_LIBRARIES)
  if (FLTK_LIBRARY_DIRS)
    # If FLTK was built using CMake (i.e., there is an FLTKConfig.cmake), the FLTK_LIBRARIES (above) does not include the path,
    # so we need to add FLTK_LIBRARY_DIRS (which is only defined if FLTK was built using CMake) to the link directories.
    cisst_set_package_settings (cisstCommon FLTK LINK_DIRECTORIES  FLTK_LIBRARY_DIRS)
  endif (FLTK_LIBRARY_DIRS)
else (CISST_HAS_FLTK)
  cisst_unset_all_package_settings (cisstCommon FLTK)
endif (CISST_HAS_FLTK)
