/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageWindowQt_h
#define _svlFilterImageWindowQt_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>


// Forward declarations
class svlWindowManagerQt4OpenGL;
class svlWindowEventHandlerBase;
class svlWindowManagerThreadProc;
class osaThread;


class CISST_EXPORT svlFilterImageWindowQt : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class svlWindowManagerThreadProc;

public:
    svlFilterImageWindowQt();
    virtual ~svlFilterImageWindowQt();

    virtual int SetPosition(const int x, const int y, const unsigned int videoch = SVL_LEFT);
    virtual int GetPosition(int & x, int & y, unsigned int videoch = SVL_LEFT) const;
    virtual void SetEventHandler(svlWindowEventHandlerBase* handler);

    virtual void SetFullScreen(const bool & fullscreen);
    virtual void SetTitle(const std::string & title);
    virtual void GetFullScreen(bool & fullscreen) const;
    virtual void GetTitle(std::string & title) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    bool FullScreenFlag;
    bool PositionSetFlag;
    int PosX[2], PosY[2];
    std::string Title;
    osaThread* Thread;
    svlWindowManagerThreadProc* ThreadProc;
    bool StopThread;

    svlWindowManagerQt4OpenGL* WindowManager;
    svlWindowEventHandlerBase* EventHandler;

protected:
    virtual void CreateInterfaces();
    virtual void SetPositionLCommand(const vctInt2 & position);
    virtual void SetPositionRCommand(const vctInt2 & position);
    virtual void GetPositionLCommand(vctInt2 & position) const;
    virtual void GetPositionRCommand(vctInt2 & position) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageWindowQt)

#endif // _svlFilterImageWindowQt_h

