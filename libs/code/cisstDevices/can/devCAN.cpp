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
  // Clear up everything
  this->id = 0;                              // default ID 
  this->nbytes=0;                            // no data
  for(devCANFrame::DataLength i=0; i<8; i++) // clear the data
    { this->data[i] = 0x00; }
}

devCANFrame::devCANFrame( devCANFrame::ID id, 
			  devCANFrame::DataField data,
			  devCANFrame::DataLength nbytes ){

  // Clear up everything before starting
  this->id = 0;                              // default ID 
  this->nbytes = 0;                          // no data
  for(devCANFrame::DataLength i=0; i<8; i++) // clear the data
    { this->data[i] = 0x00; }

  // A can ID has 11 bits. Ensure that only 11 bits are used
  if( (~0x07FF) & id ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal CAN id: " << id 
			<< std::endl;
  }

  else{
    // Now check that no more than 8 bytes are given
    if( 8 < nbytes ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Illegal message length: " << nbytes
			  << std::endl;
    }

    else{
      this->id = (0x07FF & id);                       // Copy the CAN ID
      this->nbytes = nbytes;                          // Copy the data length
      for(devCANFrame::DataLength i=0; i<nbytes; i++) // Copy the nbytes of data
	{ this->data[i] = data[i]; }
    }

  }
}

// default constructor of a can device
devCAN::devCAN( devCAN::Rate rate ){ 

  // Ensure that the rate is ok
  if( (rate == devCAN::RATE_150) ||
      (rate == devCAN::RATE_300) ||
      (rate == devCAN::RATE_1000) ){
    this->rate = rate;
  }
  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal CAN rate " << rate
			<< std::endl;
  }
}

devCAN::~devCAN(){}
