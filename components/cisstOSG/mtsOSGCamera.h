/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsOSGCamera_h
#define _mtsOSGCamera_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstOSG/cisstOSGCamera.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT mtsOSGCamera : public mtsTaskContinuous{
  
 private:

  cisstOSGCamera* cameraptr;
  osg::ref_ptr<cisstOSGCamera> camera;

  //! The input interface
  mtsInterfaceRequired* input;
  mtsFunctionRead GetPosition;


  //! User data for the MTS camera
  /** 
      This class stores a pointer to a MTS camera object. This pointer is is
      used during traversals to capture/process images and update the
      orientation/position of the camera.
  */
  class Data : public osg::Referenced {
  private:
    //! Pointer to a camera object
    mtsOSGCamera* mtsCamera;
  public:
    //! Default constructor.
    Data( mtsOSGCamera* camera ) : mtsCamera( camera ){}
    //! Get the pointer to the camera
    mtsOSGCamera* GetCamera() { return mtsCamera; }
  };


  //! Update Callback
  /**
     This callback is used to update the position/orientation of the camera
     during the update traversal.
  */
  class UpdateCallback : public osg::NodeCallback {
    //! Callback operator
    /**
       This operator is called during the update traversal. It's purpose is to
       update the position/orientation of the camera by calling the Update
       method.
    */
    void operator()( osg::Node* node, osg::NodeVisitor* );
  };

  //! This update method is called from the mtsOSGCamera::UpdateCallback
  virtual void UpdateTransform();

 public:
  
  // Main constructor
  mtsOSGCamera( const std::string& name, cisstOSGCamera* camera );

  ~mtsOSGCamera(){}

  void Startup();
  void Run();
  void Cleanup();

};

#endif
