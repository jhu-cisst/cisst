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

# All cisstMatlab libraries
project (cisstMatlabLibs)

add_subdirectory (code)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstMatlab)
