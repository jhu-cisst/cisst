/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlRGBSwapper.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterRGBSwapper_h
#define _svlFilterRGBSwapper_h

#include <cisstStereoVision/svlStreamManager.h>

class svlFilterRGBSwapper : public svlFilterBase
{
public:
    svlFilterRGBSwapper();
    virtual ~svlFilterRGBSwapper();

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    void SwapChannels(unsigned char* buffer, unsigned int pixelcount);
};

#endif // _svlFilterRGBSwapper_h

