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


#include <cisstOSG/mtsOSGCameraTask.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstCommon/cmnLogger.h>

// This operator is called during update traversal
void mtsOSGCameraTask::UpdateCallback::operator()( osg::Node* node, 
						   osg::NodeVisitor* nv ){

  mtsOSGCameraTask::Data* data = NULL;
  data = dynamic_cast<mtsOSGCameraTask::Data*>( node->getUserData() );

  if( data != NULL )
    { data->GetCameraTask()->UpdateTransform(); }

  traverse( node, nv );

}


mtsOSGCameraTask::mtsOSGCameraTask( const std::string& name, cisstOSGCamera* camera ) : 
  mtsTaskContinuous( name ),
  camera( camera ),
  input( NULL ){

  // Create the IO interface and add read/write commands
  input = AddInterfaceRequired( "Input", MTS_OPTIONAL );
  if( input )
    { input->AddFunction( "GetPositionOrientation", GetPosition ); }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Input" 
			    << std::endl;
  }

}

void mtsOSGCameraTask::Startup(){ 

  // This must be done in the thread context
  camera->Initialize();

  // Here we remove the non-MTS update callback of the camera
  osg::ref_ptr<osg::NodeCallback> osgcb;
  osgcb = camera->getCamera()->getUpdateCallback();
  camera->getCamera()->removeUpdateCallback( osgcb );

  // Set the user data of the camera to point to an mtsOSGCameraTask instead of
  // a cisstOSGCamera
  camera->getCamera()->setUserData( new mtsOSGCameraTask::Data( this ) );
    
  // Install a mtsOSGCamera update callback. This callback will fetch the 
  // data from MTS component
  osg::ref_ptr<osg::NodeCallback> mtscb;
  mtscb = new mtsOSGCameraTask::UpdateCallback();
  camera->getCamera()->setUpdateCallback( mtscb );

}
#include <cisstOSAbstraction/osaSleep.h>

void mtsOSGCameraTask::Run(){
  ProcessQueuedCommands();

  if( !camera->done() )
    { camera->frame(); }
}

void mtsOSGCameraTask::Cleanup(){}

// This is called from the mtsOSGCameraTask::UpdateCallback
void mtsOSGCameraTask::UpdateTransform(){

  if( GetPosition.IsValid() ){

    // Get the position of the camera
    prmPositionCartesianGet Rt;
    GetPosition( Rt );
    
    // Set the transformation
    camera->SetTransform( Rt.Position() );

  }

  // Update the transformation
  camera->UpdateTransform();

}
