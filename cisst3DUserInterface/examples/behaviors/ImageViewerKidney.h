/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#include <cisst3DUserInterface/ui3BehaviorBase.h>

// forward declaration for our visible objects
class ImageViewerKidneySurfaceVisibleObject;

// Always include last!
#include <ui3BehaviorsExport.h>

class CISST_EXPORT ImageViewerKidney: public ui3BehaviorBase
{
public:
    ImageViewerKidney(const std::string & name);
    ~ImageViewerKidney();

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
    void ToggleHandles(void);
    void ToggleOuter(void);
    void ToggleTree(void);
    void ToggleTumor(void);

    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;

    ui3Widget3D * Widget3D;
    ImageViewerKidneySurfaceVisibleObject * Outer;
    bool OuterShow;
    ImageViewerKidneySurfaceVisibleObject * Tree;
    bool TreeShow;
    ImageViewerKidneySurfaceVisibleObject * Tumor;
    bool TumorShow;

    bool Widget3DHandlesActive;
};
