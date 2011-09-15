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

#include <cisstBarrett/osaGroup.h>
#include <cisstCommon/cmnLogger.h>


osaGroup::ID operator++( osaGroup::ID& gid, int ){

  if( gid==osaGroup::BROADCAST )
    { return gid = osaGroup::UPPERARM; }

  if( gid==osaGroup::UPPERARM )
    { return gid = osaGroup::FOREARM; }

  if( gid==osaGroup::FOREARM )
    { return gid = osaGroup::POSITION; }

  if( gid==osaGroup::POSITION )
    { return gid = osaGroup::UPPERARM_POSITION; }

  if( gid==osaGroup::UPPERARM_POSITION ) 
    { return gid = osaGroup::FOREARM_POSITION; }

  if( gid==osaGroup::FOREARM_POSITION )
    { return gid = osaGroup::PROPERTY; }

  if( gid==osaGroup::PROPERTY ) 
    { return gid = osaGroup::HAND; }

  if( gid==osaGroup::HAND )
    { return gid = osaGroup::HAND_POSITION; }

  if( gid==osaGroup::HAND_POSITION )
    { return gid = osaGroup::LASTGROUP; }

  return gid = osaGroup::LASTGROUP;

}

// default constructor
osaGroup::osaGroup(osaGroup::ID id, cisstCAN* canbus) : 
  canbus( canbus ),
  id( id ){
  
  // Only for group 3?
  canbus->AddFilter( cisstCAN::Filter( 0x051F, 0x0403 ) );

  switch( GetID() ){

  case osaGroup::BROADCAST:
    AddPuckToGroup( osaPuck::PUCK_ID1 );
    AddPuckToGroup( osaPuck::PUCK_ID2 );
    AddPuckToGroup( osaPuck::PUCK_ID3 );
    AddPuckToGroup( osaPuck::PUCK_ID4 );
    AddPuckToGroup( osaPuck::PUCK_ID5 );
    AddPuckToGroup( osaPuck::PUCK_ID6 );
    AddPuckToGroup( osaPuck::PUCK_ID7 );
    break;

    // used to set torques
  case osaGroup::UPPERARM:
    AddPuckToGroup( osaPuck::PUCK_ID1 );
    AddPuckToGroup( osaPuck::PUCK_ID2 );
    AddPuckToGroup( osaPuck::PUCK_ID3 );
    AddPuckToGroup( osaPuck::PUCK_ID4 );
    break;

    // used to set torques
  case osaGroup::FOREARM:
    AddPuckToGroup( osaPuck::PUCK_ID5 );
    AddPuckToGroup( osaPuck::PUCK_ID6 );
    AddPuckToGroup( osaPuck::PUCK_ID7 );
    break;

  case osaGroup::POSITION:
    break;

    // used to get positions
  case osaGroup::UPPERARM_POSITION:
    AddPuckToGroup( osaPuck::PUCK_ID1 );
    AddPuckToGroup( osaPuck::PUCK_ID2 );
    AddPuckToGroup( osaPuck::PUCK_ID3 );
    AddPuckToGroup( osaPuck::PUCK_ID4 );
    break;

    // used to get positions
  case osaGroup::FOREARM_POSITION:
    AddPuckToGroup( osaPuck::PUCK_ID5 );
    AddPuckToGroup( osaPuck::PUCK_ID6 );
    AddPuckToGroup( osaPuck::PUCK_ID7 );
    break;

  case osaGroup::PROPERTY:
    break;

  case osaGroup::HAND:
    AddPuckToGroup( osaPuck::PUCK_IDF1 );
    AddPuckToGroup( osaPuck::PUCK_IDF2 );
    AddPuckToGroup( osaPuck::PUCK_IDF3 );
    AddPuckToGroup( osaPuck::PUCK_IDF4 );
    break;

  case osaGroup::HAND_POSITION:
    AddPuckToGroup( osaPuck::PUCK_IDF1 );
    AddPuckToGroup( osaPuck::PUCK_IDF2 );
    AddPuckToGroup( osaPuck::PUCK_IDF3 );
    AddPuckToGroup( osaPuck::PUCK_IDF4 );
    break;

  default:
    break;
  }

}


std::string osaGroup::LogPrefix(){
  std::string s;
  switch( GetID() ){
  case BROADCAST:
    s = std::string( "BROADCAST" );
    break;

  case UPPERARM:
    s = std::string( "UPPERARM" );
    break;

  case FOREARM:
    s = std::string( "FOREARM" );
    break;

  case POSITION:
    s = std::string( "POSITION" );
    break;

  case UPPERARM_POSITION:
    s = std::string( "UPPERARM_POSITION" );
    break;

  case FOREARM_POSITION:
    s = std::string( "FOREARM_POSITION" );
    break;

  case PROPERTY:
    s = std::string( "PROPERTY" );
    break;

  case HAND:
    s = std::string( "HAND" );
    break;

  case HAND_POSITION:
    s = std::string( "HAND_POSITION" );
    break;

  default:
    s = std::string( "UNKNOWN" );

  }

  std::ostringstream oss;
  oss << "Group " << s << ": ";
  return std::string( oss.str() );

}

// STATIC return true of the CAN frame is destined to a group
// For this we test the CAN ID for the GROUPTAG bit
bool osaGroup::IsDestinationAGroup( const cisstCANFrame canframe )
{ return (canframe.GetID() & osaGroup::GROUP_CODE) == osaGroup::GROUP_CODE; }

// return the group id
osaGroup::ID osaGroup::GetID() const { return id; }

// STATIC return the CAN of a message from the host (00000) to a group ID
// A group ID is represented by 5 bits whereas a CAN ID has 11
cisstCANFrame::ID osaGroup::CANID( osaGroup::ID id )
{ return (cisstCANFrame::ID)( osaGroup::GROUP_CODE | (0x1F & id) ); }

// STATIC return true if the CAN frame is a "set" command (a command has a 
// message of the form
// [1*** ****][**** ****]...[**** ****]
// thus we test if the MSB of the first byte is set
bool osaGroup::IsSetFrame( const cisstCANFrame& canframe ){
  const cisstCANFrame::Data* data = canframe.GetData();
  return ( data[0] & Barrett::SET_CODE) == Barrett::SET_CODE;
}

// STATIC returns the destination of a CAN id. This assumes that the 
// destination is a group (as opposed to a puck)
// The destination group ID compose the 5 LSB of a CAN ID
osaGroup::ID osaGroup::DestinationID( cisstCANFrame::ID cid )
{ return (osaGroup::ID) ( cid & 0x001F); }

// STATIC extract the destination group ID
osaGroup::ID osaGroup::DestinationID( const cisstCANFrame& canframe )
{ return osaGroup::DestinationID( canframe.GetID() ); }

// STATIC extract the origin group ID
// the origin bits are the bits 5 to 9 (zero index) in a CAN ID
osaGroup::ID osaGroup::OriginID( cisstCANFrame::ID cid )
{ return (osaGroup::ID) ((cid>>5) & 0x001F); }

// STATIC extract the origin group ID
osaGroup::ID osaGroup::OriginID( const cisstCANFrame& canframe )
{ return osaGroup::OriginID( canframe.GetID() ); }

void osaGroup::AddPuckToGroup( osaPuck::ID pid ){

  osaPuck puck( pid, canbus );
  pucks.push_back( puck );

}

// Query a group of puck.
osaGroup::Errno osaGroup::GetProperty( Barrett::ID propid, 
				       std::vector<Barrett::Value>& values ){
  
  // pack the query in a CAN frame
  cisstCANFrame sendframe;
  if( PackProperty( sendframe, Barrett::GET, propid ) != osaGroup::ESUCCESS){
    CMN_LOG_RUN_ERROR << "Failed to pack the property" << std::endl;
    return osaGroup::EFAILURE;
  }
  
  // send the CAN frame
  if( canbus->Send( sendframe ) != cisstCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << ": Failed to querry group" << std::endl;
    return osaGroup::EFAILURE;
  }

  values.clear();
  values.resize( pucks.size() );

  for( size_t i=0; i<pucks.size(); i++ ){

    // empty CAN frame
    cisstCANFrame recvframe;

    // receive the response in a CAN frame
    if( canbus->Recv( recvframe ) != cisstCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to receive property" 
			<< std::endl;
      return osaGroup::EFAILURE;
    }

    // figure which puck send that frame
    osaPuck::ID pid = osaPuck::OriginID( recvframe );
    int pindex = -1;
    for( size_t j=0; j<pucks.size(); j++ ){
      if( pucks[j].GetID() == pid )
	{ pindex = j; }
    }

    // unpack the frame
    if( -1 < pindex ){

      Barrett::ID recvpropid;
      Barrett::Value recvvalue;
      
      // unpack the frame;
      if( pucks[pindex].UnpackCANFrame( recvframe, recvpropid, recvvalue ) 
	  != osaPuck::ESUCCESS){
	CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to unpack CAN frame"
			  << std::endl;
	return osaGroup::EFAILURE;
      }

      // make sure that the property received is the one we asked for
      if( propid != recvpropid ){
	CMN_LOG_RUN_ERROR << LogPrefix() << "Unexpected property ID. "
			  << "Expected " << propid << " got " << recvpropid
			  << std::endl;
	return osaGroup::EFAILURE;
      }

      if( 0 <= pindex && pindex < (int)values.size() )
	{ values[ pindex ] = recvvalue; }
      else
	{ CMN_LOG_RUN_ERROR << LogPrefix() << "Could not index the value vector"
			    << std::endl; }
    }

    else{
      CMN_LOG_RUN_ERROR << LogPrefix() << "Could not index the pucks" 
			<< std::endl;
    }

  }
  
  return osaGroup::ESUCCESS;
   
}


 // Set the properties of a group
 // Unlike devPuck::SetProperty, this doesn't verify the pucks values
 osaGroup::Errno osaGroup::SetProperty( Barrett::ID propid, 
					Barrett::Value propval,
					bool verify){

   // pack the "set" command in a CAN frame
   cisstCANFrame canframe;
   if( PackProperty( canframe, Barrett::SET, propid, propval )
       != osaGroup::ESUCCESS ){
     CMN_LOG_RUN_ERROR << ": Failed to pack the property " << propid 
		       << std::endl;
     return osaGroup::EFAILURE;
   }

   // Send the CAN frame
   if( canbus->Send( canframe ) != cisstCAN::ESUCCESS ){
     CMN_LOG_RUN_ERROR << ": Failed to send the CAN frame."
		       << std::endl;
     return osaGroup::EFAILURE;
   }

   if( verify ){
     CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			 << ": Verify is not implemented for groups."
			 << std::endl;
   }

   return osaGroup::ESUCCESS;

 }

 // This packs a frame originating from the host and destined to the puck
 osaGroup::Errno osaGroup::PackProperty( cisstCANFrame& canframe,
					 Barrett::Command cmd,
					 Barrett::ID propid,
					 Barrett::Value propval ){

   cisstCANFrame::DataField data={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   cisstCANFrame::DataLength length=1;  // default message length (for a query)

   // See Barrett's documentation to understand the format
   data[0] = propid & 0x7F;                                 //data[0] = APPPPPP
   data[1] = 0x00;                                          //data[1] = 0000000

  if(cmd == Barrett::SET){                                  //this is a 'SET'
    data[0] |= 0x80;                                        //data[0] = 1PPPPPP
    
    // fill the rest of the bytes with the property value
    for(size_t i=2; i<6; i++){
      data[i] = (cisstCANFrame::Data)(propval & 0x000000FF);//data[i] = values
      propval >>= 8;
    }
    length = 6; // packed 6 bytes 
  }

  // create a new CAN frame
  canframe = cisstCANFrame( osaGroup::CANID( GetID() ), data, length );

  return osaGroup::ESUCCESS;
}


osaGroup::Errno osaGroup::Reset(){
  if( SetProperty( Barrett::STATUS, osaPuck::STATUS_RESET, false ) != 
      osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to reset" << std::endl;
    return osaGroup::EFAILURE;
  }

  return osaGroup::ESUCCESS;

}


osaGroup::Errno osaGroup::Ready(){
  if( SetProperty( Barrett::STATUS, osaPuck::STATUS_READY, false ) != 
      osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to ready" << std::endl;
    return osaGroup::EFAILURE;
  }

  return osaGroup::ESUCCESS;

}

osaGroup::Errno osaGroup::GetPositions( vctDynamicVector<double>& q ){

  // Query a group of puck.
  std::vector<Barrett::Value> values;

  if( GetProperty( Barrett::POS, values ) != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to get positions" << std::endl;
    return osaGroup::EFAILURE;
  }

  if( values.size() == pucks.size() ){

    q.resize( pucks.size() );
    
    for( size_t i=0; i<pucks.size(); i++ ){
      
      // convert the position from encoder ticks to radians
      q[i] = ( ((double)values[i]) * 2.0 * M_PI  /
	       ((double)pucks[i].CountsPerRevolution() ) );
      
    }

  }
  else{
    CMN_LOG_RUN_ERROR << LogPrefix() << "Expected " << pucks.size() <<" values."
		      << " Got " << values.size() 
		      << std::endl;
    return osaGroup::EFAILURE;    
  }

  return osaGroup::ESUCCESS;

}


osaGroup::Errno osaGroup::SetTorques( const vctFixedSizeVector<double,4>& tau ){

  if( GetID() == osaGroup::UPPERARM || GetID() == osaGroup::FOREARM ){
    
    double currents[4] = {0.0, 0.0, 0.0, 0.0};

    for( size_t i=0; i<4; i++ )
      { currents[i] = tau[i] * pucks[i].IpNm(); }
    
    // pack the torques in a can frames
    cisstCANFrame frame;
    if( PackCurrents( frame, tau ) != osaGroup::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to pack the torques" << std::endl;
      return osaGroup::EFAILURE;
    }

    // send the canframe (blocking)
    if( canbus->Send( frame ) != cisstCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to send upper arm torques" << std::endl;
      return osaGroup::EFAILURE;
    }

  }

  return osaGroup::ESUCCESS;

}


// pack motor torques in a CAN frame
// this should go into devGroup
osaGroup::Errno osaGroup::PackCurrents( cisstCANFrame& frame, 
					const vctFixedSizeVector<double,4>& I ){
  
  // we can only pack torques for the upper arm and forearm groups
  if( GetID() == osaGroup::UPPERARM || GetID() == osaGroup::FOREARM ){

    // copy each motor torques in this array with the correct index
    Barrett::Value values[4] = {0, 0, 0, 0};

    // for each puck in the group
    for( size_t i=0; i<pucks.size(); i++ ){
      
      // get the index of the puck within its group [0,1,2,3]
      int idx =  pucks[i].GroupIndex()-1;          // -1 because of zero index
      if( idx < 0 || 3 < idx ){                    // sanity check
	CMN_LOG_RUN_ERROR << "Illegal index" << std::endl;
	return osaGroup::EFAILURE;
      }

      values[ idx ] = (Barrett::Value)I[i];        // cast the torque      

    }

    // pack the torques in a 8 bytes message (see the documentation)
    unsigned char msg[8];
    msg[0]= Barrett::TRQ | 0x80;
    msg[1]=(unsigned char)(( values[0]>>6)&0x00FF);
    msg[2]=(unsigned char)(((values[0]<<2)&0x00FC)|((values[1]>>12)&0x0003));
    msg[3]=(unsigned char)(( values[1]>>4)&0x00FF);
    msg[4]=(unsigned char)(((values[1]<<4)&0x00F0)|((values[2]>>10)&0x000F));
    msg[5]=(unsigned char)(( values[2]>>2)&0x00FF);
    msg[6]=(unsigned char)(((values[2]<<6)&0x00C0)|((values[3]>>8) &0x003F));
    msg[7]=(unsigned char)(  values[3]    &0x00FF);
    
    // build a can frame addressed to the group ID 
    frame = cisstCANFrame( osaGroup::CANID( GetID() ), msg, 8 );
    return osaGroup::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << "Group " << GetID() << " cannot send torques"<<std::endl;

  return osaGroup::EFAILURE;

}

osaGroup::Errno osaGroup::GetStatus( std::vector<Barrett::Value>& status ){

  if( GetProperty( Barrett::STATUS, status ) != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to query the status" 
		      << std::endl;
    return osaGroup::EFAILURE;
  }

  return osaGroup::ESUCCESS;

}

osaGroup::Errno osaGroup::Initialize(){

  for( size_t i=0; i<pucks.size(); i++ ){

    CMN_LOG_RUN_VERBOSE << LogPrefix() << "Initalizing puck " 
			<< (int)pucks[i].GetID()
			<< std::endl;
    if( pucks[i].InitializeMotor() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to initialize puck"
			<< std::endl;
      return osaGroup::EFAILURE;      
    }

  }

  return osaGroup::ESUCCESS;

}

osaGroup::Errno osaGroup::SetMode( Barrett::Value mode ){
  
  for( size_t i=0; i<pucks.size(); i++ ){
    if( pucks[i].SetMode( mode ) != osaGroup::ESUCCESS ){
      CMN_LOG_RUN_ERROR << LogPrefix() << "Failed to set the mode."
			<< std::endl;
      return osaGroup::EFAILURE;
    }
  }

  return osaGroup::ESUCCESS;

}

