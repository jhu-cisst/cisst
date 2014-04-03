/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2005-12-21

  (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _SineGenerator_h
#define _SineGenerator_h

#include <cmath>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnConstants.h>

// code required for the DLL generation
#ifdef SineGenerator_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL
// end of code for the DLL generation

class CISST_EXPORT SineGenerator: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
protected:
    double Amplitude;
    double Frequency;
    double Output;
    
public:
    SineGenerator(double amp, double freq) : Amplitude(amp), Frequency(freq), Output(0.0) { }
    ~SineGenerator() {}

    double GetAmplitude() const { return Amplitude; }
    void SetAmplitude(double amp) {
		CMN_LOG_CLASS_RUN_ERROR << "Setting amplitude to " << amp << std::endl;
		Amplitude = amp;
	}

    double GetFrequency() const { return Frequency; }
    void SetFrequency(double freq) {
		CMN_LOG_CLASS_RUN_ERROR << "Setting frequency to " << freq << std::endl;
		Frequency = freq; }

    double ComputeOutput(double time)
    { Output = Amplitude*sin(2.0*cmnPI*Frequency*time); return Output; }

    double GetOutput() const { return Output; }
};

CMN_DECLARE_SERVICES_INSTANTIATION(SineGenerator)

#endif // _SineGenerator_h

