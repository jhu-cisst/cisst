/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Wen P. Liu, Anton Deguet
Created on: 2012-01-27

(C) Copyright 2009 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstNumerical/nmrRegistrationRigid.h>
#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3VTKStippleActor.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkVolumeReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkVolume16Reader.h>
#include <vtkOutlineFilter.h>
#include <vtkImageActor.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkPolyDataReader.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPointPicker.h>
#include <vtkPropPicker.h>
#include <vtkTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkVectorText.h>
#include <vtkLinearExtrusionFilter.h>

// Always include last!
#include <ui3BehaviorsExport.h>

class ManualRegistrationSurfaceVisibleStippleObject;

class CISST_EXPORT ManualRegistration: public ui3BehaviorBase
{
public:
    enum VisibleObjectType {MODEL = 0};
    enum BooleanFlagTypes {DEBUG = 0, VISIBLE, PREVIOUS_MAM, LEFT_BUTTON, RIGHT_BUTTON,
                           CAMERA_PRESSED, BOTH_BUTTON_PRESSED, UPDATE_FIDUCIALS, LEFT_BUTTON_RELEASED, RIGHT_BUTTON_RELEASED};

    ManualRegistration(const std::string & name);
    ~ManualRegistration();

    void Startup(void);
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
        return this->VisibleList;
    }

protected:
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void SecondaryMasterButtonCallback(const prmEventButton & event);
    void UpdateButtonEvents(void);
    void ResetButtonEvents(void)
    {
        this->BooleanFlags[RIGHT_BUTTON] = false;
        this->BooleanFlags[LEFT_BUTTON] = false;
        this->BooleanFlags[BOTH_BUTTON_PRESSED] = false;
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
    }
    void FollowMaster(void);
    void ComputeTransform(double pointa[3], double pointb[3],
                          double point1[3], double point2[3],
                          double object_displacement[3],
                          double object_rotation[4]);
    void CameraControlPedalCallback(const prmEventButton & payload);
    void PositionDepth(void);
    void PositionBack(void);
    void PositionHome(void);
    void ToggleUpdateFiducials(void);
    void UpdateFiducials(void);
    void ToggleVisibility(void);
    void UpdateCameraPressed(void);
    vctFrm3 GetCurrentECMtoECMRCM(void);
    void UpdatePreviousPosition();
    bool ImportFiducialFile(const std::string & inputFile);
    void Tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters);
    void AddFiducial(vctFrm3 positionUI3, bool virtualFlag);
    ManualRegistrationSurfaceVisibleStippleObject* FindClosestFiducial(vctFrm3 positionUI3, bool virtualFlag);
    void Register(void);
    void UpdateVisibleList(void);

    StateType PreviousState;
    mtsFunctionRead GetCartesianPositionSlave;
    mtsFunctionRead GetJointPositionECM;
    typedef std::map<int, bool> FlagType;
    FlagType BooleanFlags;

    vctDouble3 InitialMasterLeft, InitialMasterRight;

    ui3VisibleList * VisibleList, * VisibleListECM, * VisibleListECMRCM, * VisibleListVirtual, * VisibleListReal;
    typedef std::map<int, ManualRegistrationSurfaceVisibleStippleObject *> ManualRegistrationType;
    ManualRegistrationType VisibleObjects, VisibleObjectsVirtualFiducials, VisibleObjectsRealFiducials;
};
