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
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Widget3D.h>

// forward declaration for our visible objects
class ImageViewerSkinVisibleObject;
class ImageViewerBoneVisibleObject;
class ImageViewerOutlineVisibleObject;
class ImageViewerSlicesVisibleObject;

// Always include last!
#include <ui3BehaviorsExport.h>

class CISST_EXPORT ImageViewer: public ui3BehaviorBase
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
    void ToggleHandles(void);
    void ToggleSkin(void);
    void ToggleBone(void);
    void ToggleSlicesAxial(void);
    void ToggleSlicesCoronal(void);
    void ToggleSlicesSagittal(void);

    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;

    vtkVolume16Reader * VolumeReader;

    ui3Widget3D * Widget3D;
    ImageViewerSkinVisibleObject * Skin;
    bool SkinShow;
    ImageViewerBoneVisibleObject * Bone;
    bool BoneShow;
    ImageViewerOutlineVisibleObject * Outline;
    ImageViewerSlicesVisibleObject * Slices;
    bool SlicesAxialShow;
    bool SlicesCoronalShow;
    bool SlicesSagittalShow;

    bool Widget3DHandlesActive;
};
