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

#include <cisstDevices/wam/devPuck.hpp>
#include <cisstDevices/wam/devGroup.hpp>
#include <cisstCommon/cmnLogger.h>

// Initialize the puck to its ID and the CAN bus
devPuck::devPuck( devPuckID puckid, devCAN* canbus ){
  this->puckid = puckid;
  this->canbus = canbus;
}

devPropertyValue devPuck::IpNm() const {return ipnm;}
devPropertyValue devPuck::CountsPerRevolution() const {return cntprev;}
devPropertyValue devPuck::Index() const {return grpidx;}

// STATIC return the CAN of a message from the host (00000) to a puck ID
devCANID devPuck::CANId( devPuckID puckid )
{ return (devCANID)(0x1F & puckid); }

// STATIC return true if the CAN frame is a command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
bool devPuck::IsSetCommand( const devCANFrame& canframe ){
  const unsigned char* data = canframe.Data();
  return ( data[0] & 0x80 ) == 0x80;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a puck (as opposed to a group)
devPuckID devPuck::Destination( devCANID cid )
{ return (devPuckID)( cid & 0x001F); }

// STATIC returns the destination of a CAN frame
devPuckID devPuck::Destination( const devCANFrame& canframe )
{ return devPuck::Destination( canframe.Id() ); }

// STATIC returns the origin of a CAN id.
devPuckID devPuck::Origin( devCANID cid ) 
{ return (devPuckID)((cid>>5) & 0x001F); }

// STATIC returns the origin of a CAN frame.
devPuckID devPuck::Origin( const devCANFrame& canframe ) 
{ return devPuck::Origin( canframe.Id() ); }

// return true if the property id is valid. This needs a lot of work
bool devPuck::IsValid( devPropertyID propid ) const {
  if( 0 < propid && propid < NUM_PROPERTIES )    return true;
  else    return false;
}

// return true if the property id is valid with the value
bool devPuck::IsValid( devPropertyID, devPropertyValue ) const {return true;}

// Get a property from the puck. this sends a query to the puck and wait for
// its reply
devPropertyValue devPuck::GetProperty( devPropertyID propid ){ 

  if( IsValid( propid ) ){
    
    devCANFrame canframe;
    
    // pack the property in a can frame
    if( PackProperty( canframe, propid, 0, false) ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Failed to pack property " << propid
			  << std::endl;
      return 0;
    }

    // send the can frame and block until completed
    if( canbus->Send( canframe, true) ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Failed to querry puck." 
			  << std::endl;
      return 0;
    }
    
    // receive a can frame and block until completed
    if( canbus->Recv( canframe, true) ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Failed to receive property." 
			  << std::endl;
      return 0;
    }

    devPropertyID recvpropid;
    devPropertyValue recvpropval;
    if( UnpackCANFrame( canframe, recvpropid, recvpropval ) ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Failed to unpack CAN frame."
			  << std::endl;
      return 0;
    }

    // make sure that the received property is the same we ask for
    if( propid != recvpropid ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Oop! Unexpected property ID." 
			  << std::endl;
      return 0;
    }
    return recvpropval;
  }

  else{
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Invalid property."
			<< std::endl;
    return 0;
  }

}

// this sets the property of a puck to a value
bool devPuck::SetProperty( devPropertyID propid, 
			   devPropertyValue propval,
			   bool verify){

  if( IsValid( propid, propval ) ){

    // pack the property pack( property, value, this is a set )
    devCANFrame canframe;
    if( PackProperty( canframe, propid, propval, true ) ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			  << ": Failed to pack property " << propid
			  << std::endl;
    }

    if( canbus->Send( canframe, true) ){
      CMN_LOG_RUN_ERROR<< __PRETTY_FUNCTION__
		       << ": Failed to set the puck." 
		       << std::endl;
      return true;
    }

    if( verify ){

      // pause to let the puck initialzise
      if( propid  == devPropertyCommon::STATUS && 
	  propval == devPuckStatus::READY )
	usleep(300000);

      // query the puck to make sure that the property is set
      if( propval != GetProperty( propid ) ){
	CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
			    << ": Oop! Unexpected property value." 
			    << std::endl;
	return true;
      }
    }

    return false;
  }
  else{
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
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
  
  if( IsValid( propid, propval ) ){
    // Can message is 8 bytes long
    unsigned char data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char length = 1;  // default messagne length (this is for a "get")
    
    data[0] = propid & 0x7F;    // data[0] = APPPPPP
                                // data[1] = 0000000
    if(set){                    // this is a 'SET' command
      data[0] |= 0x80;          // data[1] = 1PPPPPP

      for(size_t i=2; i<6; i++){// data[i] 
	data[i] = (unsigned char)( propval & 0x000000FF);
 	propval >>= 8;
      }
      length = 6;
    }
    canframe = devCANFrame( devPuck::CANId( puckid ), data, length );
    return false;
  }
  else{
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
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

  const unsigned char* data = canframe.Data();
  size_t length = canframe.Length();

  // Make sure that this frame came from the proper ID...just in case
  if( Origin(canframe) == puckid ){

    // Replies to position queries are ALWAYS set command addressed to group 3
    if( IsSetCommand(canframe)                              &&  // is this a SET?
	devGroup::IsForGroup(canframe)                      &&  // to a group?...
	devGroup::Destination(canframe) == devGroup::POSITION){ // group 3
      
      propid = devPropertyMotor::POS; // set the property ID to position

      propval = 0;    // decode the payload
      propval |= ( (devPropertyValue)data[0] << 16) & 0x003F0000;
      propval |= ( (devPropertyValue)data[1] << 8 ) & 0x0000FF00;
      propval |= ( (devPropertyValue)data[2] )      & 0x000000FF;
      
      if(propval & 0x00200000) // If negative 
	propval |= 0xFFC00000; // Sign-extend 
      
      return false;
    }
    
    if( IsSetCommand(canframe) ){    // is a "normal" SET?

      propid = data[0] & 0x7F;   // extract the property ID
      
      propval = 0;               // decode the payload
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

  CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__
		      << ": the CAN frame did not originated from this puck"
		      << std::endl
		      << "devPuck ID: " << (int)puckid 
		      << std::endl 
		      << canframe
		      << std::endl;
  return true;
}

bool devPuck::Configure(){

  devPropertyValue status = GetProperty( devPropertyCommon::STATUS );
  
  if( status == devPuckStatus::READY ){
   
    // set puck to idle
    if( SetProperty( devPropertyCommon::MODE, devPuckMode::IDLE) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Faile to idle the puck #" << puckid 
			<< std::endl;
      return true;
    }

    cntprev = GetProperty( devPropertyMotor::COUNTSPERREV );
    ipnm = GetProperty( devPropertyMotor::IPNM );
    grpidx = GetProperty( devPropertyMotor::PUCKINDEX );

    return false;
  }

  if( status == devPuckStatus::RESET ){
    if( SetProperty( devPropertyCommon::STATUS, devPuckStatus::READY ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to wake up puck #" << puckid 
			<< std::endl;
      return true;
    }
  }

  return false;
}

