/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$
 
 Author(s):  Balazs Vagvolgyi
 Created on: 2010
 
 (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
 Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */

#ifndef _exampleComponent_h
#define _exampleComponent_h

#define CAMERA_SOURCE

#include <cisstMultiTask.h>
#include <cisstStereoVision/svlRequiredInterfaces.h>
#include "exampleFilter.h"


class exampleComponent: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    // see sineTask.h documentation
    exampleComponent(const std::string & taskName, double period);
    ~exampleComponent() {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

protected:

    // Test filter commands & state
    struct {
        mtsFunctionRead  Get;
        mtsFunctionWrite Set;
    } FilterParams;
    exampleFilter::Parameters FilterState;


    IReqStreamManager StreamControl;
#ifdef CAMERA_SOURCE
    IReqFilterSourceVideoCapture SourceConfig;
#else
    IReqFilterSourceVideoFile SourceConfig;
#endif
    IReqFilterImageWindow WindowConfig;
};

CMN_DECLARE_SERVICES_INSTANTIATION(exampleComponent);

#endif // _exampleComponent_h

