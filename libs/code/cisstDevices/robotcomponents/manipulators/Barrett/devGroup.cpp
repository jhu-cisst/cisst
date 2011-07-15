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

#include <cisstDevices/robotcomponents/manipulators/Barrett/devGroup.h>
#include <cisstCommon/cmnLogger.h>

devGroup::ID operator++( devGroup::ID& gid, int ){

  if( gid==devGroup::BROADCAST )
    { return gid = devGroup::UPPERARM; }

  if( gid==devGroup::UPPERARM )
    { return gid = devGroup::FOREARM; }

  if( gid==devGroup::FOREARM )
    { return gid = devGroup::POSITION; }

  if( gid==devGroup::POSITION )
    { return gid = devGroup::UPPERARM_POSITION; }

  if( gid==devGroup::UPPERARM_POSITION ) 
    { return gid = devGroup::FOREARM_POSITION; }

  if( gid==devGroup::FOREARM_POSITION )
    { return gid = devGroup::PROPERTY; }

  if( gid==devGroup::PROPERTY ) 
    { return gid = devGroup::HAND; }

  if( gid==devGroup::HAND )
    { return gid = devGroup::HAND_POSITION; }

  if( gid==devGroup::HAND_POSITION )
    { return gid = devGroup::LASTGROUP; }

  return gid = devGroup::LASTGROUP;

}

// default constructor
devGroup::devGroup(devGroup::ID id, devCAN* canbus){
  this->id = id;
  this->canbus = canbus;
}

// STATIC return true of the CAN frame is destined to a group
// For this we test the CAN ID for the GROUPTAG bit
bool devGroup::IsDestinationAGroup( const devCAN::Frame canframe )
{ return (canframe.GetID() & devGroup::GROUP_CODE) == devGroup::GROUP_CODE; }

// return the group id
devGroup::ID devGroup::GetID() const { return id; }

// STATIC return the CAN of a message from the host (00000) to a group ID
// A group ID is represented by 5 bits whereas a CAN ID has 11
devCAN::Frame::ID devGroup::CANID( devGroup::ID id )
{ return (devCAN::Frame::ID)( devGroup::GROUP_CODE | (0x1F & id) ); }

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool devGroup::IsSetFrame( const devCAN::Frame& canframe ){
  const devCAN::Frame::Data* data = canframe.GetData();
  return ( data[0] & devProperty::SET_CODE) == devProperty::SET_CODE;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a group (as opposed to a puck)
// The destination group ID compose the 5 LSB of a CAN ID
devGroup::ID devGroup::DestinationID( devCAN::Frame::ID cid )
{ return (devGroup::ID) ( cid & 0x001F); }

// STATIC extract the destination group ID
devGroup::ID devGroup::DestinationID( const devCAN::Frame& canframe )
{ return devGroup::DestinationID( canframe.GetID() ); }

// STATIC extract the origin group ID
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
devGroup::ID devGroup::OriginID( devCAN::Frame::ID cid )
{ return (devGroup::ID) ((cid>>5) & 0x001F); }

// STATIC extract the origin group ID
devGroup::ID devGroup::OriginID( const devCAN::Frame& canframe )
{ return devGroup::OriginID( canframe.GetID() ); }

// Query a group of puck. For now this is only implemented for position queries.
// Unlike devPuck::GetProperty, this doesn't process the pucks replies.
devGroup::Errno devGroup::GetProperty( devProperty::ID propid ){ 

  // ensure that the property being queried is motor position and that this
  // group is the broadcast group
  if( (propid  == devProperty::POS) ){

    // pack the query in a CAN frame
    devCAN::Frame canframe;
    if( PackProperty( canframe, devProperty::GET, devProperty::POS ) 
	!= devGroup::ESUCCESS){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the property " << devProperty::POS
			<< std::endl;
      return devGroup::EFAILURE;
    }

    // send the CAN frame
    if( canbus->Send( canframe ) != devCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to querry group." 
			<< std::endl;
      return devGroup::EFAILURE;
    }
    return devGroup::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Only position group queries." 
			<< std::endl;
    return devGroup::EFAILURE;
  }  

}

// Set the properties of a group
// Unlike devPuck::SetProperty, this doesn't verify the pucks values
devGroup::Errno devGroup::SetProperty( devProperty::ID propid, 
				       devProperty::Value propval,
				       bool verify){

  // pack the "set" command in a CAN frame
  devCAN::Frame canframe;
  if( PackProperty( canframe, devProperty::SET, propid, propval )
      != devGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to pack the property " << propid 
		      << std::endl;
    return devGroup::EFAILURE;
  }
  
  // Send the CAN frame
  if( canbus->Send( canframe ) != devCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to send the CAN frame."
		      << std::endl;
    return devGroup::EFAILURE;
  }

  if( verify ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Verify is not implemented for groups."
			<< std::endl;
  }
  
  return devGroup::ESUCCESS;

}

// This packs a frame originating from the host and destined to the puck
devGroup::Errno devGroup::PackProperty( devCAN::Frame& canframe,
					devProperty::Command cmd,
					devProperty::ID propid,
					devProperty::Value propval ){

  devCAN::Frame::DataField data={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00};
  devCAN::Frame::DataLength length=1;  // default message length (for a query)
  
  // See Barrett's documentation to understand the format
  data[0] = propid & 0x7F;                                  // data[0] = APPPPPP
                                                            // data[1] = 0000000

  if(cmd == devProperty::SET){                              // this is a 'SET'
    data[0] |= 0x80;                                        // data[0] = 1PPPPPP
    
    // fill the rest of the bytes with the property value
    for(size_t i=2; i<6; i++){
      data[i] = (devCAN::Frame::Data)( propval & 0x000000FF);// data[i] = values
      propval >>= 8;
    }
    length = 6; // packed 6 bytes 
  }

  // create a new CAN frame
  canframe = devCAN::Frame( devGroup::CANID( GetID() ), data, length );

  return devGroup::ESUCCESS;
}
