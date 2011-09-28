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

# All cisstStereoVision libraries
project (cisstStereoVisionLibs)

add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_HAS_QT)
  add_subdirectory (codeQt)
endif (CISST_HAS_QT)


# # Use XML or not, using libxml2 over
# find_package (LibXml2)
# if (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
#   option (CISST_HAS_LIBXML2_XML "Use libxml2 for cmnXMLPath" ON)
# else (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
#   option (CISST_HAS_LIBXML2_XML "Use libxml2 for cmnXMLPath" OFF)
# endif (LIBXML2_FOUND AND NOT CISST_HAS_QT_XML)
# mark_as_advanced (CISST_HAS_LIBXML2_XML)

# if (CISST_HAS_LIBXML2_XML)
#   find_package (LibXml2 REQUIRED)
#   cisst_set_package_settings (cisstCommonXML Libxml2 LIBRARIES LIBXML2_LIBRARIES)
# else (CISST_HAS_LIBXML2_XML)
#   cisst_unset_all_package_settings (cisstCommonXML libxml2)
# endif (CISST_HAS_LIBXML2_XML)

# # Safety check
# if (CISST_HAS_LIBXML2_XML AND CISST_HAS_QT_XML)
#   message (SEND_ERROR "You must choose either CISST_HAS_LIBXML2_XML or CISST_HAS_QT_XML")
# endif (CISST_HAS_LIBXML2_XML AND CISST_HAS_QT_XML)

# # Do we have some kind of XML?
# if (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
#   set (CISST_HAS_XML ON)
#   add_subdirectory (codeXML)
# else (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
#   set (CISST_HAS_XML OFF)
# endif (CISST_HAS_LIBXML2_XML OR CISST_HAS_QT_XML)
# mark_as_advanced (CISST_HAS_XML)
