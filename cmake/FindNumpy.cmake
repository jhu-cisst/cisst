#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2010-01-20
#
# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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
# File based on FindNUMARRAY distributed with ITK 3.4 (see itk.org)
#
# Main modifications:
# - use Numpy instead of Numarray for all naming
# - added path for Python 2.5 and 2.6
# - renamed python script generated (det_npp became determineNumpyPath)
# - use lower case for CMake commands and keywords
# - updated python script to use get_include, not get_numpy_include which is now deprecated
#
# ---
#
# Try to find numpy python package
# Once done this will define
#
# PYTHON_NUMPY_FOUND        - system has numpy development package and it should be used
# PYTHON_NUMPY_INCLUDE_DIR  - directory where the arrayobject.h header file can be found
#
#
if(PYTHON_EXECUTABLE)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
                    ERROR_QUIET
                    OUTPUT_VARIABLE NUMPY_PATH
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    )
endif()

find_path(PYTHON_NUMPY_INCLUDE_DIR numpy/arrayobject.h
          HINTS "${NUMPY_PATH}"
                "${PYTHON_INCLUDE_PATH}/numpy/"
          PATHS "/usr/include/python2.7/numpy/"
                "/usr/include/python2.6/numpy/"
                "/usr/include/python2.5/numpy/"
                "/usr/include/python2.4/numpy/"
                "/usr/include/python2.3/numpy/"
          DOC "Directory where the arrayobject.h header file can be found. This file is part of the numpy package"
          )

# handle the REQUIRED and QUIET arguments to find_package()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Numpy DEFAULT_MSG PYTHON_NUMPY_INCLUDE_DIR)
