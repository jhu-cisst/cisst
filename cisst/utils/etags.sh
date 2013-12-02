#!/bin/sh

# $Id$

# Author(s): Anton Deguet
# Created on: 2010-11-29

# (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

find . -name "*.h" -o -name "*.cpp" -o -name "*.py" -o -name "*.i" -o -name "*.txt" -o -name "*.in" -o -name "*.sh" -o -name "*.tex" -o -name "*.cmake" -o -name "*.ice" -o -name "*.ui" -o -name "*.java" -o -name "*.dox" -o -name "*.cdg" | xargs etags
