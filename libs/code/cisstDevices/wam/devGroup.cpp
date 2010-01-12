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

#include <cisstDevices/wam/devGroup.h>
#include <cisstCommon/cmnLogger.h>

devGroup::devGroup(devGroupID groupid, devCAN* canbus){
  this->groupid=groupid;
  this->canbus=canbus;
}

// STATIC return the CAN of a message from the host (00000) to a group ID
// A group ID is represented by 5 bits whereas a CAN ID has 11
devCANID devGroup::CANId( devGroupID groupid )
{ return (devCANID)( devGroup::GROUPTAG | (0x1F & groupid) ); }

// STATIC return true of the CAN frame is destined to a group
// For this we test the CAN ID for the GROUPTAG bit
bool devGroup::IsForGroup( const devCANFrame canframe )
{return (canframe.Id() & devGroup::GROUPTAG) == devGroup::GROUPTAG;}

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool devGroup::IsSetCommand( const devCANFrame& canframe ){
  const unsigned char* data = canframe.Data();
  return ( data[0] & 0x80 ) == 0x80;
}

// return true if the property id is valid. This needs a lot of work
// check if the property ID is within bounds
bool devGroup::IsValid( devPropertyID propid ) const {
  if( 0 < propid && propid < NUM_PROPERTIES )    return true;
  else    return false;
}

// return true if the property id is valid with the value
// not implemented
bool devGroup::IsValid( devPropertyID, devPropertyValue )const{return true;}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a group (as opposed to a puck)
// The destination group ID compose the 5 LSB of a CAN ID
devGroupID devGroup::Destination( devCANID cid )
{ return (devGroupID) ( cid & 0x001F); }

// STATIC extract the destination group ID
// just call the above method
devGroupID devGroup::Destination( const devCANFrame& canframe )
{ return devGroup::Destination( canframe.Id() ); }

// STATIC extract the origin group ID
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
devGroupID devGroup::Origin( devCANID cid )
{ return (devGroupID) ((cid>>5) & 0x001F); }

// STATIC extract the origin group ID
devGroupID devGroup::Origin( const devCANFrame& canframe )
{ return devGroup::Origin( canframe.Id() ); }

// Query a group of puck. For now this is only implemented for position queries.
// Unlike devPuck::GetProperty, this doesn't process the pucks replies.
bool devGroup::GetProperty( devPropertyID propid ){ 

  // ensure that the property being queried is motor position and that this
  // group is the broadcast group
  if( propid  == devPropertyMotor::POS && groupid == devGroup::BROADCAST ){

    // pack the query in a CAN frame
    devCANFrame canframe;
    if( PackProperty( canframe, propid, 0, false) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the property " << propid 
			<< std::endl;
      return true;
    }

    // send the CAN frame
    if( canbus->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to querry puck."
			<< std::endl;
      return true;
    }

    // wait for the pucks replies somewhere else...
    return false;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Only position group queries." 
		      << std::endl;
    return true;
  }  

}

// Set the properties of a group
// Unlike devPuck::SetProperty, this doesn't verify the pucks values
bool devGroup::SetProperty( devPropertyID propid, 
			    devPropertyValue propval ){

  // valid property ID and property value combinaison?
  if( IsValid( propid, propval ) ){

    // pack the "set" command in a CAN frame
    devCANFrame canframe;
    if( PackProperty( canframe, propid, propval, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the property " << propid 
			<< std::endl;
      return true;
    }
    
    // Send the CAN frame
    if( canbus->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to set the puck."
			<< std::endl;
      return true;
    }
    // don't double check the property value
    return false;
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Illegal property."
		      << std::endl;
    return true;
  }
}

// This packs a frame originating from the host and destined to the puck
bool devGroup::PackProperty( devCANFrame& canframe,
			     devPropertyID propid,
			     devPropertyValue propval,
			     bool set ){

  // valid property ID and property value combinaison?
  if( IsValid( propid, propval ) ){
    
    unsigned char data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char length = 1;   // default messagne length (this is for a querry)
    
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
    
    canframe = devCANFrame( devGroup::CANId( groupid ), data, length );
    return false;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Invalud property ID/value" 
		      << std::endl;
    return true;
  }
}

