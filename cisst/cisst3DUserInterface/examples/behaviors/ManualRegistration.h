/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Anton Deguet, Simon DiMaio
Created on: 2009-09-13

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

// Always include last!
#include <ui3BehaviorsExport.h>

class ManualRegistrationSurfaceVisibleStippleObject;

class ManualRegistrationVTKCallback: public vtkCommand
{
public:
    ManualRegistrationVTKCallback() { m_pvtkActorSelection = 0; }
    static ManualRegistrationVTKCallback *New(void) { return new ManualRegistrationVTKCallback; }
    void PrintSelf(ostream&, vtkIndent) { }
    void PrintTrailer(ostream&, vtkIndent) { }
    void PrintHeader(ostream&, vtkIndent) { }
    void CollectRevisions(ostream&) {}
    void SetSelectionActor(vtkActor* pvtkActorSelection) {
        if (pvtkActorSelection) {
            m_pvtkActorSelection = pvtkActorSelection;
            std::cerr << "ManualRegistrationVTKCallback::SetSelectionActor good" << std::endl;
        }
        else {
            std::cerr << "ManualRegistrationVTKCallback::SetSelectionActor bad" << std::endl;
        }
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*) {
        std::cerr << "ManualRegistrationVTKCallback::Execute" << std::endl;
        vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(caller);
        vtkPointPicker *picker = (vtkPointPicker *)iren->GetPicker();
        std::cerr << "PointId: " << picker->GetPointId() << std::endl;
        if (picker->GetPointId() != -1) {
            if (m_pvtkActorSelection) {
                m_pvtkActorSelection->SetPosition(picker->GetPickPosition());
            }
            iren->Render();
        }
    }
private:
    vtkActor * m_pvtkActorSelection;
};


class CISST_EXPORT ManualRegistration: public ui3BehaviorBase
{
public:
    enum VisibleObjectType {MODEL = 0};
    enum BooleanFlagTypes {DEBUG = 0, VISIBLE, PREVIOUS_MAM, LEFT_BUTTON, RIGHT_BUTTON,
                           CAMERA_PRESSED, BOTH_BUTTON_PRESSED, ADD_FIDUCIALS, LEFT_BUTTON_RELEASED, RIGHT_BUTTON_RELEASED};

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

    ui3VisibleObject* GetVisibleObjectAtIndex(int index);
    ManualRegistrationVTKCallback * PickerCallback;

protected:
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void SecondaryMasterButtonCallback(const prmEventButton & event);
    void UpdateFollowing(void);
    void FollowMaster(void);
    void ComputeTransform(double pointa[3], double pointb[3],
                          double point1[3], double point2[3],
                          double object_displacement[3],
                          double object_rotation[4]);
    void CameraControlPedalCallback(const prmEventButton & payload);
    void PositionDepth(void);
    void PositionBack(void);
    void PositionHome(void);
    void ToggleAddFiducials(void);
    void ToggleVisibility(void);
    void UpdateVisibleList(bool updateAll = false);
    void UpdateECMtoECMRCM(void);
    void UpdatePreviousPosition();
    bool ImportFiducialFile(const std::string & inputFile);
    void Tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters);
    void AddFiducial(vctFrm3 positionUI3, bool virtualFlag);
    void Register(void);

    StateType PreviousState;
    vctFrm3 PositionECMRCM;        //absolute position of model
    vctFrm3 ECMtoECMRCM;        //cache and updated by UpdateECMtoECMRCM()
    vctFrm3 ECMtoUI3, UI3toECM; //constant frames

    vctDouble3 InitialMasterLeft, InitialMasterRight;

    ui3VisibleList * VisibleList, * VisibleListVirtual, * VisibleListReal;
    typedef std::map<int, ManualRegistrationSurfaceVisibleStippleObject *> ManualRegistrationType;
    ManualRegistrationType VisibleObjects, VisibleObjectsVirtualFiducials, VisibleObjectsRealFiducials;

    mtsFunctionRead GetCartesianPositionSlave;
    mtsFunctionRead GetJointPositionECM;
    prmPositionJointGet JointsECM;

    typedef std::map<int, bool> FlagType;
    FlagType BooleanFlags;

    osaThreadSignal PickSignal;
};
