/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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


#ifndef _svlStreamProc_h
#define _svlStreamProc_h

#include <cisstOSAbstraction/osaForwardDeclarations.h>
#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlTypes.h>


class svlStreamProc
{
public:
    svlStreamProc(unsigned int threadcount, unsigned int threadid);

    void* Proc(svlStreamManager* baseref);

private:
    svlStreamProc();

    double GetAbsoluteTime(osaTimeServer* timeserver);

    unsigned int ThreadID;
    unsigned int ThreadCount;
};

#endif // _svlStreamProc_h

