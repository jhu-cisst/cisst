/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ImageViewer.h 848 2009-09-13 17:59:39Z adeguet1 $

  Author(s):	Anton Deguet, Simon DiMaio
  Created on:	2009-09-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisst3DUserInterface.h>

// forward declaration for our visible objects
class ImageViewerVisibleObject;

class ImageViewer: public ui3BehaviorBase
{
public:
    ImageViewer(const std::string & name);
    ~ImageViewer();

    void Startup(void) {}
    void Cleanup(void) {}
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void OnQuit(void);
    void OnStart(void);
    void Configure(const std::string & CMN_UNUSED(configFile)) {}
    bool SaveConfiguration(const std::string & CMN_UNUSED(configFile)) { return true; }
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->Widget3D;
    }

protected:
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void ToggleColor(void);
    void ToggleHandles(void);
    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;

    ui3Widget3D * Widget3D;
    ImageViewerVisibleObject * VisibleObject1;
    ImageViewerVisibleObject * VisibleObject2;
};
