/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3VisibleObject_h
#define _ui3VisibleObject_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnAccessorMacros.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Provides a base class for all visible objects.
*/
class CISST_EXPORT ui3VisibleObject: public cmnGenericObject
{
    friend class ui3SceneManager;
    friend class ui3VisibleList;
    friend class ui3Manager;

public:
    typedef ui3SceneManager::VTKHandleType VTKHandleType;

    ui3VisibleObject(const std::string & name = "Unnamed");

    /*!
     Destructor
    */
    virtual ~ui3VisibleObject(void) {};

    /*! This method needs to be overload by the user to create the
      actual VTK objects */
    virtual bool CreateVTKObjects(void) = 0;

    virtual bool UpdateVTKObjects(void) = 0;

    virtual bool Update(ui3SceneManager * sceneManager);

    virtual vtkProp3D * GetVTKProp(void);

    void Show(void);

    void Hide(void);

    void SetMatrixElement(unsigned int i, unsigned int j, double value);

    void SetPosition(const vctDouble3 & position, bool useLock = true);

    template <class _containerType>
    void SetOrientation(const vctMatrixRotation3ConstBase<_containerType> & rotationMatrix, bool useLock = true) {
        CMN_LOG_CLASS_RUN_DEBUG << "SetOrientation: called for object \"" << this->Name() << "\"" << std::endl; 
        if (this->Created()) {
            unsigned int i, j;
            if (useLock) {
                this->Lock();
            }
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    this->SetMatrixElement(i, j, rotationMatrix.Element(i, j));
                }
            }
            if (useLock) {
                this->Unlock();
            }
        } else {
            CMN_LOG_CLASS_RUN_VERBOSE << "SetOrientation: called on object \"" << this->Name() << "\" not yet created" << std::endl;
        }
    }

    void SetScale(const double & scale, bool useLock = true);

    template <bool _storageOrder>
    void SetTransformation(const vctFrameBase<vctMatrixRotation3<double, _storageOrder> > & frame,
                           bool useLock = true) {
        if (useLock) {
            this->Lock();
        }
        this->SetPosition(frame.Translation(), false);
        this->SetOrientation(frame.Rotation(), false);
        if (useLock) {
            this->Unlock();
        }
    }

    vctDoubleFrm3 GetTransformation(void) const;

    vctDoubleFrm3 GetAbsoluteTransformation(void) const;

    void SetVTKMatrix(vtkMatrix4x4 * matrix);

    void Lock(void);

    void Unlock(void);

    void AddPart(vtkProp3D * part);

    bool IsAddedToScene(void) const;

    void WaitForCreation(void) const;


 protected:
    void SetVTKHandle(VTKHandleType handle) {
        this->VTKHandle = handle;
    }

    
 private:
    // make assembly private to control access
    vtkAssembly * Assembly;
    std::vector<vtkProp3D *> Parts;

 protected:
    vtkMatrix4x4 * Matrix;
    ui3SceneManager * SceneManager;
    VTKHandleType VTKHandle;
    ui3VisibleList * ParentList;

 private:
	bool IsSceneList;

 protected:

    virtual void PropagateVisibility(bool visible);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Created);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Visible);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, Name);
    
};


#endif // _ui3VisibleObject_h
