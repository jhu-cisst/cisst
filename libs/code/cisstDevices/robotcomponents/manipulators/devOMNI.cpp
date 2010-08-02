/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstDevices/robotcomponents/manipulators/devOMNI.h>
#include <HDU/hduError.h>

const std::string devOMNI::ButtonsInterface  = "ButtonsInterface";
const std::string devOMNI::Buttons           = "Buttons";

static HDCallbackCode HDCALLBACK __OMNICallback(void *argv){

  devOMNI* omni = (devOMNI*)argv;

  hdBeginFrame( hdGetCurrentDevice() );
  
  hdGetDoublev( HD_CURRENT_JOINT_ANGLES,  omni->asyncpositions );
  hdGetDoublev( HD_CURRENT_GIMBAL_ANGLES, omni->asyncgimbals );
  hdGetIntegerv( HD_CURRENT_BUTTONS,      &omni->asyncbuttons ) ;
  hdSetDoublev( HD_CURRENT_JOINT_TORQUE,  omni->asynctorques ) ;

  hdEndFrame( hdGetCurrentDevice() );
  
  return HD_CALLBACK_CONTINUE;
  
}

static HDCallbackCode HDCALLBACK __OMNIGetJointPosition( void *argv ){

  devOMNI* omni = (devOMNI*)argv;

  for( int i=0; i<3; i++ ){
    omni->syncpositions[i] = omni->asyncpositions[i];
    omni->syncgimbals[i] = omni->asyncgimbals[i];
  }  

  return HD_CALLBACK_DONE;

}

static HDCallbackCode HDCALLBACK __OMNIGetButtons( void *argv ){

  devOMNI* omni = (devOMNI*)argv;

  omni->syncbuttons = omni->asyncbuttons;

  return HD_CALLBACK_DONE;

}


static HDCallbackCode HDCALLBACK __OMNISetJointTorque( void *argv ){

  devOMNI* omni = (devOMNI*)argv;

  for( int i=0; i<3; i++ )
    { omni->asynctorques[i] = omni->synctorques[i]; }  

  return HD_CALLBACK_DONE;

}


devOMNI::devOMNI( const std::string& devname, double period ) : 
  devManipulator( devname, period, 6 ){

  {
    // The manipulator provides an input interface
    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( devOMNI::ButtonsInterface );
    if( interface ){
      buttons.SetSize( 1 );
      buttons.SetAll( 0.0 );
      StateTable.AddData( buttons, "Buttons" );
      interface->AddCommandReadState( StateTable, buttons, devOMNI::Buttons );
    }
  }
  
}

devOMNI::~devOMNI(){
  hdStopScheduler();
  hdUnschedule(hUpdateHandle);
  hdDisableDevice(hHD);
}

void devOMNI::Configure(){
  this->hHD = hdInitDevice(HD_DEFAULT_DEVICE);
  hUpdateHandle = hdScheduleAsynchronous( __OMNICallback,
					  this, 
					  HD_MAX_SCHEDULER_PRIORITY );
  hdEnable(HD_FORCE_OUTPUT);
  hdStartScheduler();
}

vctDynamicVector<double> devOMNI::Read(){ 

  vctDynamicVector<double> qoffset( 6, 0.0 );
  qoffset[0] =  0.000;
  qoffset[1] = -0.268;
  qoffset[2] =  2.213;
  qoffset[3] =  M_PI;
  qoffset[4] = -3.916;
  qoffset[5] = -M_PI;

  hdScheduleSynchronous( __OMNIGetJointPosition,this,HD_MAX_SCHEDULER_PRIORITY);
  vctDynamicVector<double> q( 6, 
			      -syncpositions[0],
			      -syncpositions[1],
			      -(syncpositions[2] - syncpositions[1] - M_PI_2),
			      -syncgimbals[0],
			      syncgimbals[1],
			      syncgimbals[2] );
  /*
			      syncpositions[0],
			      syncpositions[1],
			      syncpositions[2] - syncpositions[1] - M_PI_2,
			      syncgimbals[0],
			      syncgimbals[1],
			      syncgimbals[2] );*/
  hdScheduleSynchronous( __OMNIGetButtons, this, HD_MAX_SCHEDULER_PRIORITY );

  switch( syncbuttons ){
  case 0:
    buttons[0] = 0.0;
    break;
  case 1:
    buttons[0] = 1.0;
    break;
  case 2:
    buttons[0] = -1.0;
    break;
  }

  return q - qoffset;
}

void devOMNI::Write( const vctDynamicVector<double>& ft ){
  synctorques[0] = -ft[0];  synctorques[1] = ft[1];  synctorques[2] = ft[2];
  hdScheduleSynchronous(__OMNISetJointTorque, this, HD_MAX_SCHEDULER_PRIORITY );
}

