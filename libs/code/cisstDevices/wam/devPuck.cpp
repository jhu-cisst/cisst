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

#include <cisstDevices/wam/devPuck.h>
#include <cisstDevices/wam/devGroup.h>
#include <cisstCommon/cmnLogger.h>

// Initialize the puck to its ID and the CAN bus
devPuck::devPuck( devPuckID puckid, devCAN* canbus ){
  this->puckid = puckid;
  this->canbus = canbus;
}

// return the motor constant
devPropertyValue devPuck::IpNm() const {return ipnm;}
// return the encoder constant
devPropertyValue devPuck::CountsPerRevolution() const {return cntprev;}
// return pucks index within its group
devPropertyValue devPuck::Index() const {return grpidx;}

// STATIC return the CAN of a message from the host (00000) to a puck ID
// A puck ID is represented by 5 bits whereas a CAN ID has 11
devCANID devPuck::CANId( devPuckID puckid )
{ return (devCANID)(0x1F & puckid); }

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool devPuck::IsSetCommand( const devCANFrame& canframe ){
  const unsigned char* data = canframe.Data();
  return ( data[0] & 0x80 ) == 0x80;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a puck (as opposed to a group)
// The destination puck ID compose the 5 LSB of a CAN ID
devPuckID devPuck::Destination( devCANID cid )
{ return (devPuckID)( cid & 0x001F); }

// STATIC returns the destination of a CAN frame
// just call the above method
devPuckID devPuck::Destination( const devCANFrame& canframe )
{ return devPuck::Destination( canframe.Id() ); }

// STATIC returns the origin of a CAN id.
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
devPuckID devPuck::Origin( devCANID cid ) 
{ return (devPuckID)((cid>>5) & 0x001F); }

// STATIC returns the origin of a CAN frame.
// just call the above method
devPuckID devPuck::Origin( const devCANFrame& canframe ) 
{ return devPuck::Origin( canframe.Id() ); }

// return true if the property id is valid. This needs a lot of work
// check if the property ID is within bounds
bool devPuck::IsValid( devPropertyID propid ) const {
  if( 0 < propid && propid < NUM_PROPERTIES )    return true;
  else    return false;
}

// return true if the property id is valid with the value
// not implemented
bool devPuck::IsValid( devPropertyID, devPropertyValue ) const {return true;}

// Get a property from the puck. this sends a query to the puck and wait for
// its reply
devPropertyValue devPuck::GetProperty( devPropertyID propid ){ 

  // Ensure that the property is valid
  if( IsValid( propid ) ){
    
    devCANFrame canframe;
    
    // pack the property query in a can frame
    if( PackProperty( canframe, propid, 0, false) ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to pack property " << propid
			  << std::endl;
      return 0;
    }

    // send the CAN frame
    if( canbus->Send( canframe, true) ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to querry puck." 
			  << std::endl;
      return 0;
    }
    
    // receive the puck's response in a CAN frame
    if( canbus->Recv( canframe, true) ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to receive property." 
			  << std::endl;
      return 0;
    }

    devPropertyID recvpropid;
    devPropertyValue recvpropval;
    // unpack the can frame
    if( UnpackCANFrame( canframe, recvpropid, recvpropval ) ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to unpack CAN frame."
			  << std::endl;
      return 0;
    }

    // make sure that the property received is the one we asked for
    if( propid != recvpropid ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Oop! Unexpected property ID." 
			  << std::endl;
      return 0;
    }
    return recvpropval;
  }

  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			<< ": Invalid property."
			<< std::endl;
    return 0;
  }

}

// this sets the property of a puck to a value
bool devPuck::SetProperty( devPropertyID propid, 
			   devPropertyValue propval,
			   bool verify){

  // check that the property ID and property value combinaison is valid
  if( IsValid( propid, propval ) ){

    // pack the property ID and value in a CAN frame command (set)
    devCANFrame canframe;
    if( PackProperty( canframe, propid, propval, true ) ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to pack property " << propid
			  << std::endl;
    }

    // send the CAN frame
    if( canbus->Send( canframe, true) ){
      CMN_LOG_RUN_ERROR<< CMN_LOG_DETAILS
		       << ": Failed to set the puck." 
		       << std::endl;
      return true;
    }

    // do we double check that the value was set?
    if( verify ){

      // If we just changed the status of the puck, give it a bit of time to
      // initialize itself
      if( propid  == devPropertyCommon::STATUS && 
	  propval == devPuckStatus::READY )
	usleep(300000);

      // query the puck to make sure that the property is set
      if( propval != GetProperty( propid ) ){
	CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			    << ": Oop! Unexpected property value." 
			    << std::endl;
	return true;
      }
    }

    return false;
  }
  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Illegal property."
			<< std::endl;
    return true;
  }
}

// This packs a frame originating from the host and destined to the puck
bool devPuck::PackProperty( devCANFrame& canframe, 
			    devPropertyID propid,
			    devPropertyValue propval,
			    bool set ){

  // ensure that the property ID and property value combinaison is valid
  if( IsValid( propid, propval ) ){

    // Can message is 8 bytes long
    unsigned char data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char length = 1;  // default messagne length (this is for a query)
    
    // See Barrett's documentation to understand the format

    data[0] = propid & 0x7F;                              // data[0] = APPPPPP
                                                          // data[1] = 0000000
    if(set){                                              // this is a 'SET'
      data[0] |= 0x80;                                    // data[0] = 1PPPPPP

      // fill the rest of the bytes with the property value
      for(size_t i=2; i<6; i++){
	data[i] = (unsigned char)( propval & 0x000000FF); // data[i] = values
 	propval >>= 8;
      }
      length = 6; // values are always 6 bytes 
    }
    // create a new CAN frame
    canframe = devCANFrame( devPuck::CANId( puckid ), data, length );
    return false;
  }
  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Invalud property ID/value" 
			<< std::endl;
    return true;
  }
}

// this unpacks a frame originating from the puck and destined to the host
// this is a bit backwards, because this methods is usually called from the 
// perspective of the host. Therefore, this is akin to asking a puck to unpack
// a message that it sent...whatever
bool devPuck::UnpackCANFrame(const devCANFrame& canframe,
			     devPropertyID& propid,
			     devPropertyValue& propval ){

  // get the data and the data length
  const unsigned char* data = canframe.Data();
  size_t length = canframe.Length();

  // Ensure that the CAN frame originated from this puck!
  if( Origin(canframe) == puckid ){

    // Replies to position queries are ALWAYS "set" frames addressed to group 3
    if( IsSetCommand(canframe)                              && // is this a SET?
	devGroup::IsForGroup(canframe)                      && // to a group?...
	devGroup::Destination(canframe) == devGroup::POSITION){// ...group 3?

      // at this point we know that the CAN frame contain a motor position

      propid = devPropertyMotor::POS; // set the property ID to position

      // decode the position payload
      propval = 0;
      propval |= ( (devPropertyValue)data[0] << 16) & 0x003F0000;
      propval |= ( (devPropertyValue)data[1] << 8 ) & 0x0000FF00;
      propval |= ( (devPropertyValue)data[2] )      & 0x000000FF;
      
      if(propval & 0x00200000) // If negative 
	propval |= 0xFFC00000; // Sign-extend 
      return false;
    }

    // does the CAN frame contain a "set" command for the puck?
    if( IsSetCommand( canframe ) ){     // is a "normal" SET?

      propid = data[0] & 0x7F;          // extract the property ID
      
      propval = 0;                      // decode the payload
      unsigned char i;
      for(i=0; i<length-2; i++)
	propval |=((devPropertyValue)data[i+2]<<(i*8))&(0x000000FF<<(i*8));
      
      if(propval & (1 << ((i*8) - 1)))  // Sign extend the value 
	propval |= 0xFFFFFFFF << (i*8);
      
      return false;
    }

    // Assume firmware request (GET). Is this ever used?
    propid = -(data[0] & 0x7F);  // extract the property ID
    propval = 0;                 // set the propval
  
    return false;
  }

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << ": the CAN frame did not originated from this puck"
		      << std::endl
		      << "devPuck ID: " << (int)ID()
		      << std::endl 
		      << canframe
		      << std::endl;
  return true;
}

// configure the status of the puck and the motor/encoder constants
bool devPuck::Configure(){

  // query the status of the puck
  CMN_LOG_RUN_VERBOSE << "Querying the status of puck " << (int)ID()<<std::endl;

  devPropertyValue status = GetProperty( devPropertyCommon::STATUS );
  
  // if the puck is "ready"
  if( status == devPuckStatus::READY ){
   
    // set puck mode to idle
    CMN_LOG_RUN_VERBOSE << "Idling the puck " << (int)ID() << std::endl;
    if( SetProperty( devPropertyCommon::MODE, devPuckMode::IDLE) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Faile to idle the puck #" << (int)ID()
			<< std::endl;
      return true;
    }

    // get the encoder constant
    CMN_LOG_RUN_VERBOSE << "Querying the encoder resolution." << std::endl;
    cntprev = GetProperty( devPropertyMotor::COUNTSPERREV );
    CMN_LOG_RUN_VERBOSE << "Encoder ticks: " << cntprev << std::endl;

    // get the motor torque constant
    CMN_LOG_RUN_VERBOSE << "Querying the motor constant." << std::endl;
    ipnm = GetProperty( devPropertyMotor::IPNM );
    CMN_LOG_RUN_VERBOSE << "Motor constant: " << ipnm << "I/Nm." << std::endl;

    // get the puck's index within its group
    CMN_LOG_RUN_VERBOSE << "Querying the group index." << std::endl;
    grpidx = GetProperty( devPropertyMotor::PUCKINDEX );
    CMN_LOG_RUN_VERBOSE << "Group index: " << grpidx << std::endl;

    return false;
  }

  // if the puck is not ready
  if( status == devPuckStatus::RESET ){
    // change its status to ready
    if( SetProperty( devPropertyCommon::STATUS, devPuckStatus::READY ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to wake up puck #" << (int)ID()
			<< std::endl;
      return true;
    }
  }

  return false;
}

