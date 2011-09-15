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

#include <cisstBarrett/osaPuck.h>
#include <cisstBarrett/osaGroup.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstCommon/cmnLogger.h>

osaPuck::ID operator++( osaPuck::ID& pid, int  ){
  // pucks for the arm
  if( pid==osaPuck::PUCK_ID1 ) { return pid = osaPuck::PUCK_ID2;}
  if( pid==osaPuck::PUCK_ID2 ) { return pid = osaPuck::PUCK_ID3;}
  if( pid==osaPuck::PUCK_ID3 ) { return pid = osaPuck::PUCK_ID4;}
  if( pid==osaPuck::PUCK_ID4 ) { return pid = osaPuck::PUCK_ID5;}
  if( pid==osaPuck::PUCK_ID5 ) { return pid = osaPuck::PUCK_ID6;}
  if( pid==osaPuck::PUCK_ID6 ) { return pid = osaPuck::PUCK_ID7;}
  if( pid==osaPuck::PUCK_ID7 ) { return pid = osaPuck::SAFETY_MODULE_ID;}
  if( pid==osaPuck::SAFETY_MODULE_ID ) 
    { return pid = osaPuck::SAFETY_MODULE_ID;}

  // pucks for the hand
  if( pid==osaPuck::PUCK_IDF1 ) { return pid = osaPuck::PUCK_IDF2;}
  if( pid==osaPuck::PUCK_IDF2 ) { return pid = osaPuck::PUCK_IDF3;}
  if( pid==osaPuck::PUCK_IDF3 ) { return pid = osaPuck::PUCK_IDF4;}
  if( pid==osaPuck::PUCK_IDF4 ) { return pid = osaPuck::SAFETY_MODULE_ID;}
  if( pid==osaPuck::SAFETY_MODULE_ID ) 
    { return pid = osaPuck::SAFETY_MODULE_ID;}

}

osaPuck::osaPuck(){}

// Initialize the puck to its ID and the CAN bus
osaPuck::osaPuck( osaPuck::ID id, cisstCAN* canbus ){
  this->id = id;
  this->canbus = canbus;

  // Add a filter for the puck property
  cisstCANFrame::ID filterid = 0x0000;
  filterid |= ( 0x00000001 << 10 );
  filterid |= ( GetID() << 5 );
  canbus->AddFilter( cisstCAN::Filter( 0x05FF, (filterid | 0x0006) ) );

}

std::string osaPuck::LogPrefix(){

  std::ostringstream oss;
  oss << "Puck " << GetID() << ": ";
  return std::string( oss.str() );

}

// return the puck ID
osaPuck::ID osaPuck::GetID() const { return id; }  

// return the motor constant
Barrett::Value osaPuck::IpNm()                const { return ipnm; }

// return the encoder constant
Barrett::Value osaPuck::CountsPerRevolution() const { return cntprev; }

// return pucks index within its group
Barrett::Value osaPuck::GroupIndex()          const { return grpidx; }

// STATIC return the CAN ID of a message from the host (00000) to a puck ID
// A puck ID is represented by 5 bits whereas a CAN ID has 11
cisstCANFrame::ID osaPuck::CANID( osaPuck::ID id )
{ return (cisstCANFrame::ID)(0x1F & id); }

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool osaPuck::IsSetFrame( const cisstCANFrame& canframe ){
  const cisstCANFrame::Data* data = canframe.GetData();
  return ( data[0] & Barrett::SET_CODE ) == Barrett::SET_CODE;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a puck (as opposed to a group)
// The destination puck ID compose the 5 LSB of a CAN ID
osaPuck::ID osaPuck::DestinationID( cisstCANFrame::ID cid )
{ return (osaPuck::ID)( cid & 0x1F); }

// STATIC returns the destination of a CAN frame
osaPuck::ID osaPuck::DestinationID( const cisstCANFrame& canframe )
{ return osaPuck::DestinationID( canframe.GetID() ); }

// STATIC returns the origin of a CAN id.
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
osaPuck::ID osaPuck::OriginID( cisstCANFrame::ID cid ) 
{ return (osaPuck::ID)((cid>>5) & 0x1F); }

// STATIC returns the origin of a CAN frame.
osaPuck::ID osaPuck::OriginID( const cisstCANFrame& canframe ) 
{ return osaPuck::OriginID( canframe.GetID() ); }

// Get a property from the puck. this sends a query to the puck and wait for
// its reply
osaPuck::Errno osaPuck::GetProperty( Barrett::ID propid,
 				     Barrett::Value& propvalue ){ 

  // empty CAN frame
  cisstCANFrame sendframe;
    
  // pack the query in a can frame
  if( PackProperty( sendframe, Barrett::GET, propid ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to pack property" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  // send the CAN frame
  if( canbus->Send( sendframe ) != cisstCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to querry puck" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  // empty CAN frame
  cisstCANFrame recvframe;

  // receive the response in a CAN frame
  if( canbus->Recv( recvframe ) != cisstCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to receive property"
		      << std::endl;
    return osaPuck::EFAILURE;
  }
  
  // unpack the can frame
  Barrett::ID recvpropid;
  if(UnpackCANFrame( recvframe, recvpropid, propvalue ) != osaPuck::ESUCCESS){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to unpack CAN frame."
		      << std::endl;
    return osaPuck::EFAILURE;
  }

  // make sure that the property received is the one we asked for
  if( propid != recvpropid ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Oop! Unexpected property ID. "
		      << "Expected " << propid << " got " << recvpropid
		      << std::endl;
    return osaPuck::EFAILURE;
  }
  
  return osaPuck::ESUCCESS;
}

// this sets the property of a puck to a value
osaPuck::Errno osaPuck::SetProperty( Barrett::ID propid, 
				     Barrett::Value propval,
				     bool verify){

  // empty CAN frame
  cisstCANFrame frame;

  // pack the property ID and value in a "set" CAN frame 
  if( PackProperty( frame, Barrett::SET, propid, propval )!=osaPuck::ESUCCESS ){
    CMN_LOG_RUN_WARNING << LogPrefix() << "Failed to pack property " << propid
			<< std::endl;
    return osaPuck::EFAILURE;
  }
  
  // send the CAN frame
  if( canbus->Send( frame ) != cisstCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to send the CAN frame." 
		      << std::endl;
    return osaPuck::EFAILURE;
  }
  
  // do we double check that the value was set?
  if( verify ){
    
    // If we just changed the status of the puck, give it a bit of time to
    // initialize itself
    if( propid  == Barrett::STATUS && propval == osaPuck::STATUS_READY )
      osaSleep( 1.0 );
    else
      osaSleep( 0.01 );

    // query the puck to make sure that the property is set
    Barrett::Value recvpropval = rand();
    if( GetProperty( propid, recvpropval ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_WARNING << LogPrefix()<<"Failed to get puck property"
			  << std::endl;
      return osaPuck::EFAILURE;
    }

    if( propval != recvpropval ){
      CMN_LOG_RUN_WARNING << LogPrefix() << "Oop! Unexpected property value. " 
			  << "Expected " << propval << " got " << recvpropval
			  << std::endl;
      return osaPuck::EFAILURE;
    }

  }
  
  return osaPuck::ESUCCESS;
}

// This packs a frame originating from the host and destined to the puck
osaPuck::Errno osaPuck::PackProperty( cisstCANFrame& canframe, 
				      Barrett::Command cmd,
				      Barrett::ID propid,
				      Barrett::Value propval ){

  // Can message is 8 bytes long
  cisstCANFrame::DataField data={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  cisstCANFrame::DataLength length=1;  // default message length (for a query)
  
  // See Barrett's documentation to understand the format
  data[0] = propid & 0x7F;                                  // data[0] = APPPPPP
                                                            // data[1] = 0000000

  if(cmd == Barrett::SET){                                  // this is a 'SET'
    data[0] |= 0x80;                                        // data[0] = 1PPPPPP
    
    // fill the rest of the bytes with the property value
    for(size_t i=2; i<6; i++){
      data[i] = (cisstCANFrame::Data)( propval & 0xFF);     // data[i] = values
      propval >>= 8;
    }
    length = 6; // packed 6 bytes 
  }
  
  // create a new CAN frame
  canframe = cisstCANFrame( osaPuck::CANID( GetID() ), data, length );

  return osaPuck::ESUCCESS;
}

// this unpacks a frame originating from the puck and destined to the host
// this is a bit backwards, because this methods is usually called from the 
// perspective of the host. Therefore, this is akin to asking a puck to unpack
// a message that it sent...whatever
osaPuck::Errno osaPuck::UnpackCANFrame(const cisstCANFrame& canframe,
				       Barrett::ID& propid,
				       Barrett::Value& propval ){

  // get the data and the data length
  const cisstCANFrame::Data* data = canframe.GetData();
  cisstCANFrame::DataLength length = canframe.GetLength();

  // Ensure that the CAN frame originated from this puck!
  if( OriginID(canframe) == GetID() ){

    // Replies to position queries are ALWAYS "set" frames addressed to group 3
    if(IsSetFrame(canframe)                                   && // a SET frame?
       osaGroup::IsDestinationAGroup(canframe)              && // to a group?
       osaGroup::DestinationID(canframe) == osaGroup::POSITION){// for #3?

      //std::cout << canframe << std::endl;

      // at this point we know that the CAN frame contain a motor position so
      // set the property ID to position
      propid = Barrett::POS;

      // decode the position payload
      propval = 0;
      propval |= ( (Barrett::Value)data[0] << 16) & 0x003F0000;
      propval |= ( (Barrett::Value)data[1] << 8 ) & 0x0000FF00;
      propval |= ( (Barrett::Value)data[2] )      & 0x000000FF;
      
      if(propval & 0x00200000)              // If negative 
	{ propval |= 0xffffffffFFC00000LL; }  // Sign-extend (64 bits)

      return osaPuck::ESUCCESS;     // done and done
    }
    // does the CAN frame contain a "set" command for the puck?
    if( IsSetFrame( canframe ) ){                  // is a "normal" SET?

      propid = (Barrett::ID)(data[0] & 0x7F);  // extract the property ID
      
      propval = 0;                                 // decode the payload
      cisstCANFrame::DataLength i;
      for(i=0; i<length-2; i++){
	propval |= ((Barrett::Value)data[i+2]<<(i*8)) & (0xFF<<(i*8));
      }

      if(propval & (1 << ((i*8) - 1)))             // Sign extend the value 
	{ propval |= 0xffffffffFFFFFFFFLL << (i*8); }// ???? Extend ????
      //{ propval |= 0xFFFFFFFF << (i*8); }
      
      return osaPuck::ESUCCESS;
    }
    
    // Assume firmware request (GET). Is this ever used?
    // This is remnants from old code
    propid = (Barrett::ID)(-(data[0] & 0x7F)); // extract the property ID
    propval = 0;                                   // set the propval
    
    return osaPuck::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << LogPrefix() << "Frame ID = " << OriginID(canframe) 
		    << " does not match puck ID = "
		    << std::endl;
  
  return osaPuck::EFAILURE;
}

// configure the status of the puck and the motor/encoder constants
osaPuck::Errno osaPuck::InitializeMotor(){

  Barrett::Value status;
  if( GetStatus( status ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to query the status" 
		      << std::endl;
    return osaPuck::EFAILURE;
  }
  
  // if the puck is "ready"
  if( status == osaPuck::STATUS_READY ){
   
    // set puck mode to idle
    if( SetMode( osaPuck::MODE_IDLE ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to idle" << std::endl;
      return osaPuck::EFAILURE;
    }

    // get the encoder constant
    if( GetCountsPerRev() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get Cnt/Rev" << std::endl;
      return osaPuck::EFAILURE;
    }

    // get motor constant
    if( GetIpNm() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get I/Nm" << std::endl;
      return osaPuck::EFAILURE;
    }
    
    // get group index
    if( GetGroupIndex() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get index" << std::endl;
      return osaPuck::EFAILURE;
    }
    
    // get group index
    if( GetMembership() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() <<"Failed to get membership"<<std::endl;
      return osaPuck::EFAILURE;
    }
    
    return osaPuck::ESUCCESS;

  }

  // if the puck is not ready
  if( status == osaPuck::STATUS_RESET ){
    CMN_LOG_RUN_WARNING << LogPrefix() << "Puck is resetting. Trying again." 
			<< std::endl;

    // change its status to ready
    if( SetProperty( Barrett::STATUS, osaPuck::STATUS_READY, true ) ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to wake up" << std::endl;
      return osaPuck::EFAILURE;
    }
    osaSleep( 1.0 );
    InitializeMotor();
  }

  return osaPuck::ESUCCESS;
}

osaPuck::Errno osaPuck::Reset(){
  if( SetProperty( Barrett::STATUS, osaPuck::STATUS_RESET, false ) != 
      osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to reset" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::Ready(){
  if( SetProperty( Barrett::STATUS, osaPuck::STATUS_READY, false ) != 
      osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to ready" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetPosition( Barrett::Value& position ){

  if( GetProperty( Barrett::POS, position ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to query position" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::InitializeSM(){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){

    CMN_LOG_RUN_VERBOSE << LogPrefix() <<"Querying the status"<<std::endl;
    Barrett::Value smstatus;
    if( GetProperty( Barrett::STATUS, smstatus ) != osaSafetyModule::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to query the safety module."
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    
    // check the safety module is "ready"
    if( smstatus != osaSafetyModule::STATUS_READY ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "The safety module is offline"
			<< std::endl;
      return osaPuck::EFAILURE;
    }  
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "The safety module is online" 
			<< std::endl;
  
    // Set the velocity warning
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Setting velocity warning" 
			<< std::endl;
    if( SetVelocityWarning( 4000 ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "velocity warning not set"
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Velocity warning set" << std::endl;

    // Set the velocity fault
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Setting velocity fault" << std::endl;
    if( SetVelocityFault( 8000 ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "velocity fault not set"
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Velocity fault set" << std::endl;
    
    // Set the torque warning
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Setting torque warning" << std::endl;
    if( SetTorqueWarning( 4000 ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "torque warning not set"
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Torque warning set" << std::endl;

    // Set the torque fault
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Setting torque fault" << std::endl;
    if( SetTorqueFault( 8000 ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "torque fault not set"
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Torque fault set" << std::endl;

    CMN_LOG_RUN_VERBOSE << LogPrefix() << "The safety module is good to go" 
			<< std::endl;

    return osaPuck::ESUCCESS;
  }
  
  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}


osaPuck::Errno osaPuck::SetVelocityWarning( Barrett::Value vw ){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if(SetProperty( Barrett::VELWARNING, vw, true )!=osaSafetyModule::ESUCCESS){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Unable to set the velocity warning." 
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    return osaPuck::ESUCCESS;
  }
  
  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}

osaPuck::Errno osaPuck::SetVelocityFault( Barrett::Value vf ){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if(SetProperty( Barrett::VELFAULT, vf, true ) != osaSafetyModule::ESUCCESS){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Unable to set the velocity fault" 
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    return osaPuck::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}

osaPuck::Errno osaPuck::SetTorqueWarning( Barrett::Value tw ){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if(SetProperty( Barrett::TRQWARNING, tw, true )!=osaSafetyModule::ESUCCESS){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Unable to set the torques warning" 
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    return osaPuck::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}

osaPuck::Errno osaPuck::SetTorqueFault( Barrett::Value tf ){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if(SetProperty( Barrett::TRQFAULT, tf, false )!= osaSafetyModule::ESUCCESS){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Unable to set the torques fault" 
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    return osaPuck::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}


osaPuck::Errno osaPuck::IgnoreFault( Barrett::Value fault ){

  if( GetID() == osaPuck::SAFETY_MODULE_ID ){
    if( SetProperty( Barrett::IGNOREFAULT, fault, true ) ){
      CMN_LOG_RUN_ERROR << LogPrefix() << " Unable to set fault tolerance"
			<< std::endl;
      return osaPuck::EFAILURE;
    }
    return osaPuck::ESUCCESS;
  }
  
  CMN_LOG_RUN_ERROR << LogPrefix() << "Not a safety module" << std::endl;

  return osaPuck::EFAILURE;

}

osaPuck::Errno osaPuck::SetPosition( double q ){

  Barrett::Value position;  // this is the position in encoder ticks
  
  // convert the motor positions to encoder ticks
  position = (Barrett::Value)floor( (q*CountsPerRevolution()) / (2.0*M_PI) );
  
  // Set the motor position. Don't double check the position because the 
  // noise might change the encoder.
  if( SetProperty( Barrett::POS, position, false ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set pos of puck#: " 
		      << (int)GetID()
		      << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetStatus( Barrett::Value& status ){

  if( GetProperty( Barrett::STATUS, status ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to query the status" 
		      << std::endl;
    return osaPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << LogPrefix() << "Status " << status << std::endl;

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::SetMode( Barrett::Value mode ){

  // set puck mode
  if( SetProperty( Barrett::MODE, mode, true ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set mode" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetCountsPerRev(){

  // get the encoder constant
  if( GetProperty( Barrett::COUNTSPERREV, cntprev ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get resolution" <<std::endl;
    return osaPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << LogPrefix() << "Resolution: " << cntprev << std::endl;
  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetIpNm(){

  // get the motor torque constant
  if( GetProperty( Barrett::IPNM, ipnm ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get I/Nm" << std::endl;
    return osaPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << LogPrefix() << "I/Nm: " << ipnm << std::endl;

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetGroupIndex(){

  // get the puck index
  if( GetProperty( Barrett::PUCKINDEX, grpidx ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get index" << std::endl;
    return osaPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << LogPrefix() << "Index: " << grpidx << std::endl;
  
  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetMembership(){

  if( GetGroupA() != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group A" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  if( GetGroupB() != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group B" << std::endl;
    return osaPuck::EFAILURE;
  }

  if( GetGroupC() != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group C" << std::endl;
    return osaPuck::EFAILURE;
  }

  if( GetID() == osaPuck::PUCK_ID5 || 
      GetID() == osaPuck::PUCK_ID6 || 
      GetID() == osaPuck::PUCK_ID7 ){

    if( groupC != 5 ){ 
      CMN_LOG_RUN_WARNING << LogPrefix() << "Fixing membership of group C" 
			  << std::endl;
      
      if( SetGroupC( 5 ) != osaPuck::ESUCCESS ){
	CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set group C" <<std::endl;
	return osaPuck::EFAILURE;
      }

      if( GetGroupC() != osaPuck::ESUCCESS ){
	CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group C" <<std::endl;
	return osaPuck::EFAILURE;
      }

      if( groupC == 5 )
	{ CMN_LOG_RUN_WARNING << LogPrefix() << "Fix succeeded" << std::endl; }
      else
	{ CMN_LOG_RUN_WARNING << LogPrefix() << "Fix failed" << std::endl; }
      
    }
    
  }

  CMN_LOG_RUN_VERBOSE << LogPrefix() 
		      << "Member of group:"
		      << " " << groupA
		      << " " << groupB
		      << " " << groupC
		      << std::endl;

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::GetGroupA(){

  if( GetProperty( Barrett::GROUPA, groupA ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group A" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  return osaPuck::ESUCCESS;
}

osaPuck::Errno osaPuck::GetGroupB(){

  if( GetProperty( Barrett::GROUPB, groupB ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group B" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  return osaPuck::ESUCCESS;
}

osaPuck::Errno osaPuck::GetGroupC(){

  if( GetProperty( Barrett::GROUPC, groupC ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get group C" << std::endl;
    return osaPuck::EFAILURE;
  }
  
  return osaPuck::ESUCCESS;
}


osaPuck::Errno osaPuck::SetGroupA( Barrett::Value grp ){

  if( SetProperty( Barrett::GROUPA, grp, true ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set group A" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::SetGroupB( Barrett::Value grp ){

  if( SetProperty( Barrett::GROUPB, grp, true ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set group B" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}

osaPuck::Errno osaPuck::SetGroupC( Barrett::Value grp ){

  if( SetProperty( Barrett::GROUPC, grp, true ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set group C" << std::endl;
    return osaPuck::EFAILURE;
  }

  return osaPuck::ESUCCESS;

}
