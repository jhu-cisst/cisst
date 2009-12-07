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

#ifdef CISST_HAS_PEAKCAN

#include <cisstDevices/can/devPEAKCAN.hpp>
#include <cisstCommon/cmnLogger.h>

devPEAKCAN::devPEAKCAN( devCANRate rate ) : devCAN( rate ){}

bool devPEAKCAN::Open(){

  // Open the device (ok, this is not very device independent since it's for
  // PCI hardware --> must put a parameter in the constructor
  handle = CAN_Open(HW_PCI, 0);

  if(handle == NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": CAN_Open failed." 
		      << std::endl;
    return true;
  }

  // Check the status (kind of pointless here)
  if( CAN_Status(handle) != CAN_ERR_OK ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to reset the status." 
		      << std::endl;
    return true;
  }

  // Initialize the can device must switch between the baud rates
  if( CAN_Init(handle, CAN_BAUD_1M, CAN_INIT_TYPE_ST) != CAN_ERR_OK ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to initialize the device." 
		      << std::endl;
    return true;
  }

  // flush the filters
  if( CAN_ResetFilter(handle) != CAN_ERR_OK ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to reset the filters." 
		      << std::endl;
    return true;
  }

  return false;
}

bool devPEAKCAN::Close(){
  if( CAN_Close(handle) != CAN_ERR_OK ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to close the device." 
		      << std::endl;
    return true;
  }
  return false;
}

bool devPEAKCAN::Recv( devCANFrame& canframe, bool block ){

  TPCANRdMsg msg;
  
  if( LINUX_CAN_Read(handle, &msg) != CAN_ERR_OK ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to read a message." 
		      << std::endl;
    return true;
  }
  
  canframe = devCANFrame( (devCANID)msg.Msg.ID, 
			  (unsigned char*)msg.Msg.DATA,
			  (unsigned char) msg.Msg.LEN );
  
  return false;
}

bool devPEAKCAN::Send( const devCANFrame& canframe, bool block ){
  
  const unsigned char* framedata = canframe.Data();
  
  TPCANMsg msg;
  
  msg.ID = (DWORD)canframe.Id();
  msg.LEN = (BYTE)(canframe.Length() & 0x0F);
  for( size_t i=0; i<canframe.Length(); i++)
    msg.DATA[i] = (BYTE)framedata[i];
  msg.MSGTYPE = MSGTYPE_STANDARD;
  
  if( CAN_Write(handle, &msg) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to write the message." 
		      << std::endl;
    return true;
  }

  return false;
}

#endif
