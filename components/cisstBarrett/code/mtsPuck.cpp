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


#include <cisstBarrett/mtsPuck.h>
#include <cisstCommon/cmnLogger.h>

CMN_IMPLEMENT_SERVICES( mtsPuck );

mtsPuck::  mtsPuck( const std::string& componentname, 
		    osaPuck::ID id,
		    cisstCAN* can ) :
  mtsComponent( componentname ),
  osaPuck( id, can ),
  io( NULL ),
  ctl( NULL ){

  // Create the IO interface and add read/write commands
  io = AddInterfaceProvided( "IO" );
  if( io ){
    io->AddCommandRead ( &mtsPuck::mtsReadPosition,  this, "ReadPosition" );
    io->AddCommandWrite( &mtsPuck::mtsWriteTorque, this, "WriteTorque" );
  }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface IO" << std::endl;
  }

  // Create the CTL itnerface
  ctl = AddInterfaceProvided( "CTL" );
  if( ctl ){
    ctl->AddCommandVoid( &mtsPuck::mtsReset,      this, "Reset" );
    ctl->AddCommandVoid( &mtsPuck::mtsReady,      this, "Ready" );
    ctl->AddCommandVoid( &mtsPuck::mtsInitialize, this, "Initialize" );
  }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface CTL"<< std::endl;
  }

}

// Write a CAN frame to the device
void mtsPuck::mtsReset(){ 
  if( osaPuck::Reset() != osaPuck::ESUCCESS ){
    CMN_LOG_CLASS_RUN_ERROR << "Failed to reset puck " << GetID() << std::endl;
  }
}

// Write a CAN frame to the device
void mtsPuck::mtsReady(){ 
  if( osaPuck::Ready() != osaPuck::ESUCCESS ){
    CMN_LOG_CLASS_RUN_ERROR << "Failed to ready puck " << GetID() << std::endl;
  }
}


// Write a CAN frame to the device
void mtsPuck::mtsInitialize(){ 
  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if( osaPuck::InitializeSM() != osaPuck::ESUCCESS ){
      CMN_LOG_CLASS_RUN_ERROR<<"Failed to initialize puck "<<GetID()<<std::endl;
    }
  }
  else{
    if( osaPuck::InitializeMotor() != osaPuck::ESUCCESS ){
      CMN_LOG_CLASS_RUN_ERROR<<"Failed to initialize puck "<<GetID()<<std::endl;
    }
  }
}


// Read a CAN frame from the device
void mtsPuck::mtsReadPosition( mtsLongLong& position ) const {
}

// Write a CAN frame to the device
void mtsPuck::mtsWriteTorque( const mtsLongLong& torque ) {
}

