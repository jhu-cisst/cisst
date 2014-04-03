/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ali Uneri
  Created on: 2009-08-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstNumerical/nmrRegistrationRigid.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3Manager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3Widget3D.h>

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkAssembly.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVRMLImporter.h>

class RegistrationModelFiducials;
class RegistrationModel;


class RegistrationBehavior: public ui3BehaviorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    RegistrationBehavior(const std::string & name);
    ~RegistrationBehavior(void);

    void Configure(const std::string & CMN_UNUSED(configFile)) {}
    bool SaveConfiguration(const std::string & CMN_UNUSED(configFile)) {
        return true;
    }
    void Startup(void) {}
    void Cleanup(void) {}
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void OnStart(void);
    void OnQuit(void);
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->Widget3D;
    }

protected:
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void ToggleHandles(void);
    void Register(void);
    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;

    ui3Widget3D * Widget3D;
    RegistrationModel * VisibleObject1;
    ui3VisibleList * ModelFiducials;
    vctDynamicVector<vct3> VirtualFiducials;
    vctDynamicVector<vct3> RealFiducials;
    vctFrm3 Registration;
};

CMN_DECLARE_SERVICES_INSTANTIATION(RegistrationBehavior);
