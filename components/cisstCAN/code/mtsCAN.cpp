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


#include <cisstCAN/mtsCAN.h>
#include <cisstCommon/cmnLogger.h>

CMN_IMPLEMENT_SERVICES( mtsCAN );

mtsCAN::mtsCAN( const std::string& componentname, cisstCAN* can ) :
  mtsComponent( componentname ),
  can( can ),
  io( NULL ),
  ctl( NULL ){

  // Ensure the CAN is valid
  if( can == NULL )
    { CMN_LOG_CLASS_RUN_ERROR << "No CAN device!" << std::endl; }

  // Create the IO interface and add read/write commands
  io = AddInterfaceProvided( "IO" );
  if( io ){
    io->AddCommandRead ( &mtsCAN::mtsRead,  this, "Read" );
    io->AddCommandWrite( &mtsCAN::mtsWrite, this, "Write" );
  }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface IO" << std::endl;
  }

  // Create the CTL itnerface
  ctl = AddInterfaceProvided( "CTL" );
  if( ctl ){
    ctl->AddCommandVoid( &mtsCAN::mtsOpen,  this, "Open" );
    ctl->AddCommandVoid( &mtsCAN::mtsClose, this, "Close" );
  }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface CTL"<< std::endl;
  }

}

// Write a CAN frame to the device
void mtsCAN::mtsOpen()
{ if( can != NULL ) { can->Open(); } }

// Write a CAN frame to the device
void mtsCAN::mtsClose()
{ if( can != NULL ) { can->Close(); } }

// Read a CAN frame from the device
void mtsCAN::mtsRead( mtsCANFrame &frame ) const {
  if( can != NULL ){
    cisstCANFrame f;
    can->Recv( f );
    frame = mtsCANFrame( f );
  }
}

// Write a CAN frame to the device
void mtsCAN::mtsWrite( const mtsCANFrame &frame ) {
  if( can != NULL )
    { can->Send( (cisstCANFrame)frame ); }
}

