/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-09-09

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _clientTask_h
#define _clientTask_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

class clientTask: public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    mtsFunctionQualifiedRead QualifiedReadFunction;

    const osaTimeServer * TimeServer;
    
    unsigned int NumberOfSamplesSkipped;
	unsigned int NumberOfSamplesCollected;
    bool BenchmarkCompleted;

    vctDynamicVector<double> Results;

public:
    // see sineTask.h documentation
    clientTask(const std::string & taskName, double period);
    ~clientTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

    bool IsBenchmarkCompleted(void) const;
    void ShowResults();
};

CMN_DECLARE_SERVICES_INSTANTIATION(clientTask);

#endif // _clientTask_h
