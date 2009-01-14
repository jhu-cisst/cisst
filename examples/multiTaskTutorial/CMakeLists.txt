#
# $Id: CMakeLists.txt,v 1.7 2008/09/04 05:19:09 anton Exp $
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

PROJECT(multiTaskTutorial)

# Threads for cisstOSAbstraction
FIND_PACKAGE(Threads REQUIRED)

SUBDIRS(example1 example2 example3 example6)
IF(CISST_HAS_SWIG_PYTHON)
  SUBDIRS(example4 example5)
ENDIF(CISST_HAS_SWIG_PYTHON)
