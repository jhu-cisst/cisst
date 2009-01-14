#
# $Id: CMakeLists.txt,v 1.41 2008/10/21 19:34:51 tian Exp $
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# Backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisstExamples)


# Set the ouptut path for the libraries and executables
SET(LIBRARY_OUTPUT_PATH ${cisstExamples_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${cisstExamples_BINARY_DIR}/bin)


# Lists all examples (one per subdirectory)
SET(CISST_EXAMPLES
    commonTutorial
    multiTaskTutorial
    nArrayBenchmark
    numericalTutorial
    vectorTutorial
    )    


# Which examples to build
FOREACH(example ${CISST_EXAMPLES})
  # Build by default
  OPTION(BUILD_EXAMPLES_${example} "Build ${example}." OFF)
ENDFOREACH(example)


# Don't offer packages based on dependencies
IF(NOT CISST_HAS_SWIG_PYTHON)
  SET(BUILD_EXAMPLES_pythonEmbedded OFF)
  SET(BUILD_EXAMPLES_pythonEmbeddedIRE OFF)
ENDIF(NOT CISST_HAS_SWIG_PYTHON)


# Create targets for each example
FOREACH(example ${CISST_EXAMPLES})
  IF(BUILD_EXAMPLES_${example})
    SUBDIRS(${example})
  ENDIF(BUILD_EXAMPLES_${example})
ENDFOREACH(example)
