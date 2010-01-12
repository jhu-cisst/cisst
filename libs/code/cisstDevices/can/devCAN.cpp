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

#include <cisstDevices/can/devCAN.h>
#include <cisstCommon/cmnLogger.h>

// Default initialization of a CAN frame
devCANFrame::devCANFrame(){ 
  canid = 0;                  // default ID 
  nbytes=0;                   // no data
  for(size_t i=0; i<8; i++)   // clear the data
    bytes[i] = 0x00;
}

devCANFrame::devCANFrame( devCANID canid, 
			  unsigned char bytes[8], 
			  size_t nbytes ){

  // A can ID has 11 bits. Ensure that only 11 bits are used
  if( ~0x07FF & ~canid )
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal CAN id: " << canid 
			<< ". Truncating the ID to: " << (0x07FF & canid)
			<< std::endl;
  this->canid = 0x07FF & canid;

  if( 8 < nbytes )
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal message length: " << nbytes
			<< ". Truncating to 8 bytes"
			<< std::endl;
  this->nbytes = ( nbytes < 8 ) ? nbytes : 8;

  for(size_t i=0; i<8; i++)
    this->bytes[i] = bytes[i];
}

// default constructor of a can device
devCAN::devCAN( devCANRate rate ){ this->rate = rate; }
devCAN::~devCAN(){}
