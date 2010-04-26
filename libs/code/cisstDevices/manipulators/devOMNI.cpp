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


#include <cisstDevices/manipulators/devOMNI.h>
#include <HDU/hduError.h>

static HDCallbackCode HDCALLBACK __OMNICallback(void *argv){

  devOMNI* omni = (devOMNI*)argv;

  hdBeginFrame( hdGetCurrentDevice() );
  
  hdGetDoublev( HD_CURRENT_JOINT_ANGLES, omni->asyncpositions );
  hdGetDoublev( HD_CURRENT_GIMBAL_ANGLES, omni->asyncgimbals );
  hdSetDoublev( HD_CURRENT_JOINT_TORQUE, omni->asynctorques ) ;

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


static HDCallbackCode HDCALLBACK __OMNISetJointTorque( void *argv ){

  devOMNI* omni = (devOMNI*)argv;

  for( int i=0; i<3; i++ )
    { omni->asynctorques[i] = omni->synctorques[i]; }  

  return HD_CALLBACK_DONE;

}


devOMNI::devOMNI( const std::string& devname, double period ) : 
  devManipulator( devname, period, 6 ){}

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
  hdScheduleSynchronous( __OMNIGetJointPosition,this,HD_MAX_SCHEDULER_PRIORITY);
  vctDynamicVector<double> q( 6, 
			      syncpositions[0],
			      syncpositions[1],
			      syncpositions[2] - syncpositions[1] - M_PI_2,
			      syncgimbals[0],
			      syncgimbals[1],
			      syncgimbals[2] );
  return q;
}

void devOMNI::Write( const vctDynamicVector<double>& ft ){
  synctorques[0] = -ft[0];  synctorques[1] = ft[1];  synctorques[2] = ft[2];
  hdScheduleSynchronous(__OMNISetJointTorque, this, HD_MAX_SCHEDULER_PRIORITY );
}

