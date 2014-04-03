/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2011-04-25

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>

#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Widget3D.h>

// Always include last!
#include <ui3BehaviorsExport.h>

class CISST_EXPORT VideoViewer3D: public ui3BehaviorBase
{
 public:
    VideoViewer3D(const std::string & name);
    ~VideoViewer3D();

	int img1W, img1H;

    void Startup(void);
    void Cleanup(void);
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void OnQuit(void);
    void OnStart(void);
    void Configure(const std::string & videoFile = "");
    bool SaveConfiguration(const std::string & CMN_UNUSED(configFile)) { return true; }
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->Widget3D;
	}

 protected:
	void ToggleHandles(void);
    void StartStop(void);
	void OnStreamSample(svlSample * sample, int streamindex);

	svlStreamManager Stream;
	svlFilterSourceVideoFile Source;

    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;

    ui3Widget3D * Widget3D;
    bool Widget3DHandlesActive;
	ui3ImagePlane * ImagePlane;
	bool Playing;
};
