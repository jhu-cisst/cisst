#
# $Id$
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

ADD_SUBDIRECTORY(example1)
ADD_SUBDIRECTORY(example2)
ADD_SUBDIRECTORY(example3)
ADD_SUBDIRECTORY(example6)

IF(CISST_HAS_SWIG_PYTHON)
  ADD_SUBDIRECTORY(example4)
  ADD_SUBDIRECTORY(example5)
ENDIF(CISST_HAS_SWIG_PYTHON)
