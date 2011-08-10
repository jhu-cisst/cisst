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

#include <cisstCAN/cisstCANFrame.h>
#include <cisstCommon/cmnLogger.h>

// Default initialization of a CAN frame
cisstCANFrame::cisstCANFrame(){ 
  // Clear up everything
  this->id = 0;                                  // default ID 
  this->nbytes=0;                                // no data
  for(cisstCANFrame::DataLength i=0; i<8; i++) // clear the data
    { this->data[i] = 0x00; }
}

cisstCANFrame::cisstCANFrame( cisstCANFrame::ID id, 
			      cisstCANFrame::DataField data,
			      cisstCANFrame::DataLength nbytes ){

  // Clear up everything before starting
  this->id = 0;                                  // default ID 
  this->nbytes = 0;                              // no data
  for(cisstCANFrame::DataLength i=0; i<8; i++) // clear the data
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
      this->id = (0x07FF & id);                          // Copy the CAN ID
      this->nbytes = nbytes;                             // Copy the data length
      for(cisstCANFrame::DataLength i=0; i<nbytes; i++)// Copy the data
	{ this->data[i] = data[i]; }
    }

  }

}

cisstCANFrame::cisstCANFrame( cisstCANFrame::ID id, 
			      const vctDynamicVector<cisstCANFrame::Data>& data){

  // Clear up everything before starting
  this->id = 0;                              // default ID 
  this->nbytes = 0;                          // no data
  for(cisstCANFrame::DataLength i=0; i<8; i++) // clear the data
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
      for(cisstCANFrame::DataLength i=0; i<nbytes; i++) // Copy the data
	{ this->data[i] = data[i]; }
    }
  }

}

