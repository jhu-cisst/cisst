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

#include <cisstDevices/wam/devGroup.hpp>
#include <cisstCommon/cmnLogger.h>

devGroup::devGroup(devGroupID groupid, devCAN* canbus){
  this->groupid=groupid;
  this->canbus=canbus;
}

// STATIC return the CAN of a message from the host (00000) to a group ID
devCANID devGroup::CANId( devGroupID groupid )
{ return (devCANID)( devGroup::GROUPTAG | (0x1F & groupid) ); }

// STATIC return true of the CAN frame is destined to a group
bool devGroup::IsForGroup( const devCANFrame canframe )
{return (canframe.Id() & devGroup::GROUPTAG) == devGroup::GROUPTAG;}

// STATIC return true if the CAN frame contain a "set" command
bool devGroup::IsSetCommand( const devCANFrame& canframe ){
  const unsigned char* data = canframe.Data();
  return ( data[0] & 0x80 ) == 0x80;
}

// return true if the proprety id is valid
bool devGroup::IsValid( devPropertyID propid ) const {
  if( 0 < propid && propid < NUM_PROPERTIES )    return true;
  else    return false;
}

// return true if the property id/value combination is valud
bool devGroup::IsValid( devPropertyID, devPropertyValue )const{return true;}

// STATIC extract the destination group ID
devGroupID devGroup::Destination( devCANID cid )
{ return (devGroupID) ( cid & 0x001F); }

// STATIC extract the destination group ID
devGroupID devGroup::Destination( const devCANFrame& canframe )
{ return devGroup::Destination( canframe.Id() ); }

// STATIC extract the origin group ID
devGroupID devGroup::Origin( devCANID cid )
{ return (devGroupID) ((cid>>5) & 0x001F); }

// STATIC extract the origin group ID
devGroupID devGroup::Origin( const devCANFrame& canframe )
{ return devGroup::Origin( canframe.Id() ); }

// querry a group of puck...for tnow this is only valid for position queries
// unlike devPuck::get, this doesn't wait for the replies
bool devGroup::GetProperty( devPropertyID propid ){ 

  if( propid  == devPropertyMotor::POS && 
      groupid == devGroup::BROADCAST ){

    // pack'n send the querry
    devCANFrame canframe;
    if( PackProperty( canframe, propid, 0, false) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to pack the property " << propid 
			<< std::endl;
      return true;
    }

    // send the canframe and wait for its completion
    if( canbus->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to querry puck."
			<< std::endl;
      return true;
    }

    return false;
  }

  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Only position group queries." 
		      << std::endl;
    return true;
  }  

}

// Set the properties of a group
// unlike devPuck::set, this doesn't wait for the replies
bool devGroup::SetProperty( devPropertyID propid, 
			    devPropertyValue propval){

  if( IsValid( propid, propval ) ){

    // pack'n send
    devCANFrame canframe;
    if( PackProperty( canframe, propid, propval, true ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to pack the property " << propid 
			<< std::endl;
      return true;
    }
    
    if(canbus->Send( canframe, true) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to set the puck."
			<< std::endl;
      return true;
    }

    return false;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
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
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Invalud property ID/value" 
		      << std::endl;
    return true;
  }
}

