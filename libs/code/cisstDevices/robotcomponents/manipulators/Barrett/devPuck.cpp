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

#include <cisstDevices/robotcomponents/manipulators/Barrett/devPuck.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devGroup.h>
#include <cisstCommon/cmnLogger.h>

devPuck::ID operator++( devPuck::ID& pid, int  ){
  // pucks for the arm
  if( pid==devPuck::PUCK_ID1 ) { return pid = devPuck::PUCK_ID2;}
  if( pid==devPuck::PUCK_ID2 ) { return pid = devPuck::PUCK_ID3;}
  if( pid==devPuck::PUCK_ID3 ) { return pid = devPuck::PUCK_ID4;}
  if( pid==devPuck::PUCK_ID4 ) { return pid = devPuck::PUCK_ID5;}
  if( pid==devPuck::PUCK_ID5 ) { return pid = devPuck::PUCK_ID6;}
  if( pid==devPuck::PUCK_ID6 ) { return pid = devPuck::PUCK_ID7;}
  if( pid==devPuck::PUCK_ID7 ) { return pid = devPuck::SAFETY_MODULE_ID;}
  if( pid==devPuck::SAFETY_MODULE_ID ) 
    { return pid = devPuck::SAFETY_MODULE_ID;}

  // pucks for the hand
  if( pid==devPuck::PUCK_IDF1 ) { return pid = devPuck::PUCK_IDF2;}
  if( pid==devPuck::PUCK_IDF2 ) { return pid = devPuck::PUCK_IDF3;}
  if( pid==devPuck::PUCK_IDF3 ) { return pid = devPuck::PUCK_IDF4;}
  if( pid==devPuck::PUCK_IDF4 ) { return pid = devPuck::SAFETY_MODULE_ID;}
  if( pid==devPuck::SAFETY_MODULE_ID ) 
    { return pid = devPuck::SAFETY_MODULE_ID;}

}

devPuck::devPuck(){}

// Initialize the puck to its ID and the CAN bus
devPuck::devPuck( devPuck::ID id, devCAN* canbus ){
  this->id = id;
  this->canbus = canbus;

  // Add a filter for the puck property
  devCAN::Frame::ID filterid = 0x0000;
  filterid |= ( 0x00000001 << 10 );
  filterid |= ( GetID() << 5 );
  canbus->AddFilter( devCAN::Filter( 0x05FF, (filterid | 0x0006) ) );
}

// return the puck ID
devPuck::ID devPuck::GetID() const { return id; }  

// return the motor constant
devProperty::Value devPuck::IpNm()                const { return ipnm; }

// return the encoder constant
devProperty::Value devPuck::CountsPerRevolution() const { return cntprev; }

// return pucks index within its group
devProperty::Value devPuck::GroupIndex()          const { return grpidx; }

// STATIC return the CAN ID of a message from the host (00000) to a puck ID
// A puck ID is represented by 5 bits whereas a CAN ID has 11
devCAN::Frame::ID devPuck::CANID( devPuck::ID id )
{ return (devCAN::Frame::ID)(0x1F & id); }

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool devPuck::IsSetFrame( const devCAN::Frame& canframe ){
  const devCAN::Frame::Data* data = canframe.GetData();
  return ( data[0] & devProperty::SET_CODE ) == devProperty::SET_CODE;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a puck (as opposed to a group)
// The destination puck ID compose the 5 LSB of a CAN ID
devPuck::ID devPuck::DestinationID( devCAN::Frame::ID cid )
{ return (devPuck::ID)( cid & 0x1F); }

// STATIC returns the destination of a CAN frame
devPuck::ID devPuck::DestinationID( const devCAN::Frame& canframe )
{ return devPuck::DestinationID( canframe.GetID() ); }

// STATIC returns the origin of a CAN id.
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
devPuck::ID devPuck::OriginID( devCAN::Frame::ID cid ) 
{ return (devPuck::ID)((cid>>5) & 0x1F); }

// STATIC returns the origin of a CAN frame.
devPuck::ID devPuck::OriginID( const devCAN::Frame& canframe ) 
{ return devPuck::OriginID( canframe.GetID() ); }

// Get a property from the puck. this sends a query to the puck and wait for
// its reply
devPuck::Errno devPuck::GetProperty( devProperty::ID propid,
 				     devProperty::Value& propvalue ){ 

  // empty CAN frame
  devCAN::Frame sendcanframe;
    
  // pack the query in a can frame

  CMN_LOG_RUN_VERBOSE << "Packing the property" << std::endl;
  if(PackProperty(sendcanframe, devProperty::GET, propid) != devPuck::ESUCCESS){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack property " << propid
			<< std::endl;
    return devPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << "Property packed" << std::endl;
  
  // send the CAN frame
  CMN_LOG_RUN_VERBOSE << "Sending property" << std::endl;
  if( canbus->Send( sendcanframe ) != devCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to querry puck." 
		      << std::endl;
    return devPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << "Property sent" << std::endl;
  
  // empty CAN frame
  devCAN::Frame recvcanframe;

  // receive the response in a CAN frame
  CMN_LOG_RUN_VERBOSE << "Waiting for answer" << std::endl;
  if( canbus->Recv( recvcanframe ) != devCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to receive property." 
		      << std::endl;
    return devPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << "Received property" << std::endl;
  
  // unpack the can frame
  devProperty::ID recvpropid;
  CMN_LOG_RUN_VERBOSE << "Unpacking property" << std::endl;
  if(UnpackCANFrame(recvcanframe, recvpropid, propvalue) != devPuck::ESUCCESS){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to unpack CAN frame."
			<< std::endl;
    return devPuck::EFAILURE;
  }
  CMN_LOG_RUN_VERBOSE << "Property unpacked" << std::endl;
  
  // make sure that the property received is the one we asked for
  if( propid != recvpropid ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Oop! Unexpected property ID." 
			<< std::endl;
    return devPuck::EFAILURE;
  }

  return devPuck::ESUCCESS;
}

// this sets the property of a puck to a value
devPuck::Errno devPuck::SetProperty( devProperty::ID propid, 
				     devProperty::Value propval,
				     bool verify){

  // empty CAN frame
  devCAN::Frame canframe;

  // pack the property ID and value in a "set" CAN frame 
  if( PackProperty( canframe, devProperty::SET, propid, propval ) 
      != devPuck::ESUCCESS ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": Failed to pack property " << propid
			<< std::endl;
    return devPuck::EFAILURE;
  }
  
  // send the CAN frame
  if( canbus->Send( canframe ) != devCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR<< CMN_LOG_DETAILS
		     << ": Failed to send the CAN frame." 
		     << std::endl;
    return devPuck::EFAILURE;
  }
  
  // do we double check that the value was set?
  if( verify ){
    
    // If we just changed the status of the puck, give it a bit of time to
    // initialize itself
    if( propid  == devProperty::STATUS && propval == devPuck::STATUS_READY )
      usleep(1000000);

    // query the puck to make sure that the property is set
    devProperty::Value recvpropval = rand();
    if( GetProperty( propid, recvpropval ) != devPuck::ESUCCESS ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Failed to verify the puck's property." 
			  << std::endl;
      return devPuck::EFAILURE;
    }
    
    if( propval != recvpropval ){
      CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			  << ": Oop! Unexpected property value." 
			  << std::endl;
      return devPuck::EFAILURE;
    }
  }
  
  return devPuck::ESUCCESS;
}

// This packs a frame originating from the host and destined to the puck
devPuck::Errno devPuck::PackProperty( devCAN::Frame& canframe, 
				      devProperty::Command cmd,
				      devProperty::ID propid,
				      devProperty::Value propval ){

  // Can message is 8 bytes long
  devCAN::Frame::DataField data={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  devCAN::Frame::DataLength length=1;  // default message length (for a query)
  
  // See Barrett's documentation to understand the format
  data[0] = propid & 0x7F;                                  // data[0] = APPPPPP
                                                            // data[1] = 0000000

  if(cmd == devProperty::SET){                              // this is a 'SET'
    data[0] |= 0x80;                                        // data[0] = 1PPPPPP
    
    // fill the rest of the bytes with the property value
    for(size_t i=2; i<6; i++){
      data[i] = (devCAN::Frame::Data)( propval & 0xFF);       // data[i] = values
      propval >>= 8;
    }
    length = 6; // packed 6 bytes 
  }
  
  // create a new CAN frame
  canframe = devCAN::Frame( devPuck::CANID( GetID() ), data, length );

  return devPuck::ESUCCESS;
}

// this unpacks a frame originating from the puck and destined to the host
// this is a bit backwards, because this methods is usually called from the 
// perspective of the host. Therefore, this is akin to asking a puck to unpack
// a message that it sent...whatever
devPuck::Errno devPuck::UnpackCANFrame(const devCAN::Frame& canframe,
				       devProperty::ID& propid,
				       devProperty::Value& propval ){

  // get the data and the data length
  const devCAN::Frame::Data* data = canframe.GetData();
  devCAN::Frame::DataLength length = canframe.GetLength();

  // Ensure that the CAN frame originated from this puck!
  if( OriginID(canframe) == GetID() ){

    // Replies to position queries are ALWAYS "set" frames addressed to group 3
    if(IsSetFrame(canframe)                                   && // a SET frame?
       devGroup::IsDestinationAGroup(canframe)                && // to a group?
       devGroup::DestinationID(canframe) == devGroup::POSITION){ // for group 3?

      //std::cout << canframe << std::endl;

      // at this point we know that the CAN frame contain a motor position so
      // set the property ID to position
      propid = devProperty::POS;

      // decode the position payload
      propval = 0;
      propval |= ( (devProperty::Value)data[0] << 16) & 0x003F0000;
      propval |= ( (devProperty::Value)data[1] << 8 ) & 0x0000FF00;
      propval |= ( (devProperty::Value)data[2] )      & 0x000000FF;
      
      if(propval & 0x00200000)              // If negative 
	{ propval |= 0xffffffffFFC00000LL; }  // Sign-extend (64 bits)

      return devPuck::ESUCCESS;     // done and done
    }
    // does the CAN frame contain a "set" command for the puck?
    if( IsSetFrame( canframe ) ){                  // is a "normal" SET?

      propid = (devProperty::ID)(data[0] & 0x7F);  // extract the property ID
      
      propval = 0;                                 // decode the payload
      devCAN::Frame::DataLength i;
      for(i=0; i<length-2; i++){
	propval |= ((devProperty::Value)data[i+2]<<(i*8)) & (0xFF<<(i*8));
      }

      if(propval & (1 << ((i*8) - 1)))             // Sign extend the value 
	{ propval |= 0xffffffffFFFFFFFFLL << (i*8); }// ???? Extend ????
      //{ propval |= 0xFFFFFFFF << (i*8); }
      
      return devPuck::ESUCCESS;
    }
    
    // Assume firmware request (GET). Is this ever used?
    // This is remnants from old code
    propid = (devProperty::ID)(-(data[0] & 0x7F)); // extract the property ID
    propval = 0;                                   // set the propval
    
    return devPuck::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		    << "Frame ID = " << OriginID(canframe) 
		    << " does not match puck ID = " << GetID()
		    << std::endl;
  
  return devPuck::EFAILURE;
}

// configure the status of the puck and the motor/encoder constants
devPuck::Errno devPuck::Configure(){

  // query the status of the puck
  CMN_LOG_INIT_VERBOSE << std::endl
		       << "Querying the status of puck " << (int)GetID()
		       << std::endl;
  devProperty::Value status;
  if( GetProperty( devProperty::STATUS, status ) != devPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to query the status of puck " << (int)GetID()
		      << std::endl;
    return devPuck::EFAILURE;
  }
  
  // if the puck is "ready"
  if( status == devPuck::STATUS_READY ){
   
    // set puck mode to idle
    CMN_LOG_INIT_VERBOSE << "Idling the puck " << (int)GetID() << std::endl;
    if( SetProperty( devProperty::MODE, devPuck::MODE_IDLE, true ) 
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to idle the puck #" << (int)GetID()
			<< std::endl;
      return devPuck::EFAILURE;
    }

    // get the encoder constant
    CMN_LOG_INIT_VERBOSE << "Querying the encoder resolution." << std::endl;
    if( GetProperty( devProperty::COUNTSPERREV, cntprev )
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to query the counts/rev."
			<< std::endl;
      return devPuck::EFAILURE;
    }
    CMN_LOG_INIT_VERBOSE << "Encoder ticks: " << cntprev << std::endl;

    // get the motor torque constant
    CMN_LOG_INIT_VERBOSE << "Querying the motor constant." << std::endl;
    if( GetProperty( devProperty::IPNM, ipnm ) 
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to query the motor constant."
			<< std::endl;
      return devPuck::EFAILURE;
    }
    CMN_LOG_INIT_VERBOSE << "Motor constant: " << ipnm << "I/Nm." << std::endl;

    // get the puck's index within its group
    CMN_LOG_INIT_VERBOSE << "Querying the group index." << std::endl;
    if( GetProperty( devProperty::PUCKINDEX, grpidx )
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to query the group index."
			<< std::endl;
      return devPuck::EFAILURE;
    }

    CMN_LOG_INIT_VERBOSE << "Group index: " << grpidx << std::endl;
    devProperty::Value vala, valb, valc;
    GetProperty( devProperty::GROUPA, vala );
    GetProperty( devProperty::GROUPB, valb );
    GetProperty( devProperty::GROUPC, valc );
    CMN_LOG_INIT_VERBOSE << "Group A: " << vala 
			 << " Group B: " << valb 
			 << " Group C: " << valc << std::endl << std::endl;

    return devPuck::ESUCCESS;
  }

  // if the puck is not ready
  if( status == devPuck::STATUS_RESET ){
    // change its status to ready
    if( SetProperty( devProperty::STATUS, devPuck::STATUS_READY, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to wake up puck #" << (int)GetID()
			<< std::endl;
      return devPuck::EFAILURE;
    }
  }

  return devPuck::ESUCCESS;
}

