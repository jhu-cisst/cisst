/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009-03-12 

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlRenderTargets_h
#define _svlRenderTargets_h

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstVector/vctDynamicVector.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class svlRenderTargets;

class CISST_EXPORT svlRenderTargetBase
{
friend class svlRenderTargets;

protected:
    svlRenderTargetBase() {}
    virtual ~svlRenderTargetBase() {}

public:
    virtual bool SetImage(unsigned char* buffer, int offsetx, int offsety, bool vflip) = 0;
    virtual unsigned int GetWidth() = 0;
    virtual unsigned int GetHeight() = 0;
};


class CISST_EXPORT svlRenderTargets
{
private:
    svlRenderTargets();
    ~svlRenderTargets();

public:
    static svlRenderTargets* Instance();
    static svlRenderTargetBase* Get(unsigned int deviceID);
    static void Release(unsigned int deviceID);
    static void ReleaseAll();

private:
    vctDynamicVector<int> TargetDeviceID;
    vctDynamicVector<svlRenderTargetBase*> Targets;
};


#endif // _svlRenderTargets_h

