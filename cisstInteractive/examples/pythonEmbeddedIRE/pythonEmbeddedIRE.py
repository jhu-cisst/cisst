#!/usr/bin/python
# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#

#  Author(s): Peter Kazanzides
#  Created on: 2005-12-21

# (C) Copyright 2005-2025 Johns Hopkins University (JHU), All Rights Reserved.
 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

import cisstCommonPython as cisstCommon
import SineGeneratorPython as SineGenerator

wave = cisstCommon.cmnObjectRegister.FindObject("SineGenerator")

print("Sine Wave: amplitude = %f, frequency = %f" % \
      (wave.GetAmplitude(), wave.GetFrequency()))

print("Use wave.SetAmplitude(X) to set amplitude to X")
print("Use wave.SetFrequency(Y) to set frequency to Y")
print("Use wave.SetFrequency(0) to pause output display")
