/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisst3DUserInterface/ui3VisibleAxes.h>
#include <cisst3DUserInterface/ui3VTKStippleActor.h>
#include <cisst3DUserInterface/ui3VTKRenderer.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAxesActor.h>
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
#include <vtkCylinderSource.h>
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
    enum VisibleObjectType {ALL = 0, NO_FIDUCIALS, MODEL, TUMOR, CURSOR, TARGETS_REAL, TARGETS_VIRTUAL, FIDUCIALS_REAL, FIDUCIALS_VIRTUAL, CALIBRATION_REAL, CALIBRATION_VIRTUAL};
    enum BooleanFlagTypes {DEBUG = 0, VISIBLE, PREVIOUS_MAM, LEFT_BUTTON, RIGHT_BUTTON,
                           CAMERA_PRESSED, CLUTCH_PRESSED, BOTH_BUTTON_PRESSED, UPDATE_FIDUCIALS, LEFT_BUTTON_RELEASED, RIGHT_BUTTON_RELEASED};
    enum Frame {ECM=0, UI3, ECMRCM};

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
    void MasterClutchPedalCallback(const prmEventButton & payload);
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
    void UpdateCameraPressed(void);

private:
    void PositionDepth(void);
    void PositionBack(void);
    void PositionHome(void);
    void ToggleFiducials(void);
    void UpdateFiducials(void);
    void ToggleVisibility(void);

    vctFrm3 GetCurrentECMtoECMRCM(void);
    vctFrm3 GetCurrentCartesianPositionSlave(void);
    void UpdatePreviousPosition();
    bool ImportFiducialFile(const std::string & inputFile, VisibleObjectType type);
    void Tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters);
    void AddFiducial(vctFrm3 positionUI3, VisibleObjectType type);
    ManualRegistrationSurfaceVisibleStippleObject* FindClosestFiducial(vctFrm3 positionUI3, VisibleObjectType type, int& index);
    void Register(void);
    void ComputeTRE();
    bool RayRayIntersect(vctDouble3 p1,vctDouble3 p2,vctDouble3 p3,vctDouble3 p4,vctDouble3 &pa,vctDouble3 &pb);
    void GetFiducials(vctDynamicVector<vct3>& fiducialsVirtualECMRCM, vctDynamicVector<vct3>& fiducialsRealECMRCM,VisibleObjectType type, Frame frame);
    void UpdateVisibleList(void);

    StateType PreviousState;
    mtsFunctionRead GetCartesianPositionSlave;
    mtsFunctionRead GetJointPositionECM;
    typedef std::map<int, bool> FlagType;
    FlagType BooleanFlags;

    VisibleObjectType VisibleToggle;
    VisibleObjectType FiducialToggle;
    double MeanTRE, MaxTRE, MeanTREProjection, MaxTREProjection, MeanTRETriangulation, MaxTRETriangulation;
    size_t TREFiducialCount;
    FILE *TRE, * TREProjection, *TRETriangulation;

    vctDouble3 InitialMasterLeft, InitialMasterRight;
    vctFrm3 WristCalibration, WristToTip;
    ui3VisibleObject * Cursor;
    ui3VisibleList * VisibleList, * VisibleListECM, * VisibleListECMRCM, * VisibleListVirtual, * VisibleListReal;
    typedef std::map<size_t, ManualRegistrationSurfaceVisibleStippleObject *> ManualRegistrationType;
    ManualRegistrationType VisibleObjects, VisibleObjectsVirtualFiducials, VisibleObjectsRealFiducials, VisibleObjectsVirtualTargets, VisibleObjectsRealTargets,
        VisibleObjectsVirtualCalibration,VisibleObjectsRealCalibration;
};
