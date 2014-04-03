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

#ifndef _svlFilterImageWindowTargetSelect_h
#define _svlFilterImageWindowTargetSelect_h

#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlWindowManagerBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageWindowTargetSelect : public svlWindowEventHandlerBase, public svlFilterImageWindow
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageWindowTargetSelect();

    virtual int SetMaxTargets(unsigned int maxtargets);
    virtual void SetEnableAdd(bool enable);
    virtual void SetEnableModify(bool enable);
    virtual void SetAlwaysSendTargets(bool enable);
    virtual void SetCrosshairStyle(svlRGB color, unsigned int radius, unsigned int thickness);

    virtual void SetTargets(const svlSampleTargets& targets);
    virtual void GetTargets(svlSampleTargets& targets) const;

protected:
    virtual void SetFullScreen(const bool & fullscreen);
    virtual void SetEventHandler(svlWindowEventHandlerBase* handler);
    virtual void GetFullScreen(bool & fullscreen) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

    virtual void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);

private:
    svlFilterImageWindow Window;
    svlSample* DisplayImage;

    svlRGB CrosshairColor;
    unsigned int CrosshairRadius;
    unsigned int CrosshairThickness;
    bool SendTargets;
    bool AlwaysSend;
    bool EnableAdd;
    bool EnableModify;
    bool ButtonDown;
    int SelectedTarget;
    int SelectedTargetWindow;
    vctFixedSizeVector<int, 2> SelectionOffset;
    svlSampleTargets Targets;
};


CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageWindowTargetSelect)

#endif // _svlFilterImageWindowTargetSelect_h

