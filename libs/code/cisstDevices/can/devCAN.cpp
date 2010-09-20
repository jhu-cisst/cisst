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
devCAN::Frame::Frame(){ 
  // Clear up everything
  this->id = 0;                              // default ID 
  this->nbytes=0;                            // no data
  for(devCAN::Frame::DataLength i=0; i<8; i++) // clear the data
    { this->data[i] = 0x00; }
}

devCAN::Frame::Frame( devCAN::Frame::ID id, 
		      devCAN::Frame::DataField data,
		      devCAN::Frame::DataLength nbytes ){

  // Clear up everything before starting
  this->id = 0;                              // default ID 
  this->nbytes = 0;                          // no data
  for(devCAN::Frame::DataLength i=0; i<8; i++) // clear the data
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
      this->id = (0x07FF & id);                         // Copy the CAN ID
      this->nbytes = nbytes;                            // Copy the data length
      for(devCAN::Frame::DataLength i=0; i<nbytes; i++) // Copy the data
	{ this->data[i] = data[i]; }
    }

  }
}

devCAN::Frame::Frame( devCAN::Frame::ID id, 
		      const vctDynamicVector<devCAN::Frame::Data>& data ){

  // Clear up everything before starting
  this->id = 0;                              // default ID 
  this->nbytes = 0;                          // no data
  for(devCAN::Frame::DataLength i=0; i<8; i++) // clear the data
    { this->data[i] = 0x00; }

  // A can ID has 11 bits. Ensure that only 11 bits are used
  if( (~0x07FF) & id ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal CAN id: " << id 
			<< std::endl;
  }

  else{
    // Now check that no more than 8 bytes are given
    if( 8 < data.size() ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Illegal message length: " << data.size()
			  << std::endl;
    }

    else{
      this->id = (0x07FF & id);                         // Copy the CAN ID
      this->nbytes = data.size();                       // Copy the data length
      for(devCAN::Frame::DataLength i=0; i<nbytes; i++) // Copy the data
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
