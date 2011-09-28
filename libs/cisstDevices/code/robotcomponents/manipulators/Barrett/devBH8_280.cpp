/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstDevices/robotcomponents/manipulators/Barrett/devBH8_280.h>
#include <cisstCommon/cmnLogger.h>

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

using namespace std;

//CMN_IMPLEMENT_SERVICES(devBH8_280);

// main constructor
devBH8_280::devBH8_280( const std::string& taskname, 
			double period, 
			osaCPUMask cpumask,
			devCAN* candev ):
  devManipulator( taskname,
		  period, 
		  devManipulator::ENABLED, 
		  cpumask, 
		  devManipulator::FORCETORQUE ){

  input = ProvideInputRn( devManipulator::Input,
                          devRobotComponent::POSITION |
			  devRobotComponent::FORCETORQUE,
			  4 );

  output = ProvideOutputRn( devManipulator::Output,
			    devRobotComponent::POSITION,
			    4 );
  
  // Adjust the pucks vector to the number of requested joints
  pucks.resize( 4 );

  // must have a CAN device
  if( candev == NULL ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": CAN device missing."
		       << std::endl;
    exit(-1);
  }

  // copy the CAN device
  this->candev = candev;

}

void devBH8_280::Cleanup()
{ candev->Close(); }

void devBH8_280::Configure( const std::string& ){

  //
  // open the can device
  //

  CMN_LOG_INIT_VERBOSE << "Opening the CAN device." << std::endl;
  if( candev->Open() != devCAN::ESUCCESS ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to open the CAN device. Exiting."
		       << std::endl;
    exit(-1);
  }
  CMN_LOG_INIT_VERBOSE << "The CAN device is opened." << std::endl;
  
  // This comes from the actual query to the torque fault!!!!
  candev->AddFilter( devCAN::Filter( 0x051F, 0x0503 ) );

  //
  // Now set up the groups
  //
  groups.clear();

  CMN_LOG_INIT_VERBOSE << "Creating groups." << std::endl;
  // create'n initialize the groups with their ID and the canbus
  // The group ID are available in devGroup.h.
  //for( devGroup::ID gid=devGroup::BROADCAST; gid<devGroup::G8; gid++ )
  for( devGroup::ID gid=devGroup::BROADCAST; gid<devGroup::LASTGROUP; gid++ )
    { groups.push_back( devGroup( gid, candev ) ); }
  
  groups[devGroup::HAND_POSITION].AddPuckIDToGroup( devPuck::PUCK_IDF1 );
  groups[devGroup::HAND_POSITION].AddPuckIDToGroup( devPuck::PUCK_IDF2 );
  groups[devGroup::HAND_POSITION].AddPuckIDToGroup( devPuck::PUCK_IDF3 );
  groups[devGroup::HAND_POSITION].AddPuckIDToGroup( devPuck::PUCK_IDF4 );

  //
  // Now set up the pucks
  //

  // create'n initialize the pucks with their ID and the CAN device. Note that
  // the pucks IDs are 1-index and the vector of pucks is 0-index.
  CMN_LOG_INIT_VERBOSE << "Creating pucks." << std::endl;

  devPuck::ID pid = devPuck::PUCK_IDF1;
  for( size_t i=0; i<pucks.size(); i++ )
    { pucks[i] = devPuck( pid, candev ); pid++; } 

  //candev->AddFilter( devCAN::Filter( 0x051F, 0x0506 ) );
  
  // wake up all the pucks. This broadcast the command that every puck should
  // change its status to "ready" 
  CMN_LOG_INIT_VERBOSE << "Waking up the pucks" << std::endl;
  if( SetPucksStatus( devPuck::STATUS_RESET, false ) != devBH8_280::ESUCCESS ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to wake up the pucks. Exiting." 
		       << std::endl;
    exit(-1);
  }
  usleep( 2000000 );

  CMN_LOG_INIT_VERBOSE << "Waking up the pucks" << std::endl;
  if( SetPucksStatus( devPuck::STATUS_READY, false ) != devBH8_280::ESUCCESS ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to wake up the pucks. Exiting." 
		       << std::endl;
    exit(-1);
  }
  usleep( 2000000 );


  // count number of pucks that are "ready"
  CMN_LOG_INIT_VERBOSE << "Querying the status of the pucks" << std::endl;
  size_t npucksready = 0;
  for(size_t i=0; i<pucks.size(); i++){

    devProperty::Value pstatus;
    if(pucks[i].GetProperty(devProperty::STATUS, pstatus) != devPuck::ESUCCESS){
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			 << ": Failed to query the status of puck "
			 << (int)pucks[i].GetID() << ". Exiting."
			 << std::endl;
      exit(-1);
    }

    if( pstatus == devPuck::STATUS_READY){
      CMN_LOG_INIT_VERBOSE << "Puck " << (int)pucks[i].GetID() << " is online." 
			   << std::endl;
      npucksready++;
    }
    else{
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			 << ": Puck " << (int)pucks[i].GetID() << " is offline."
			 << std::endl;
    }
  }

  

  // make sure that all the pucks are ready
  if( npucksready != pucks.size() ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Found " << npucksready << ". "
		       << "Expected " << pucks.size() << ". Exiting." 
		       << std::endl;
    exit(-1);
  }  
  CMN_LOG_INIT_VERBOSE << "All the pucks on online." << std::endl;
  
  // configure the pucks
  for(size_t i=0; i<pucks.size(); i++){
    if( pucks[i].Configure() != devPuck::ESUCCESS ){
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			 << ": Failed to configure puck "<<(int)pucks[i].GetID()
			 << std::endl;
      exit(-1);
    }
  }

  //
  // Set up the convertion matrices
  //

  // This sets the matrices for converting motor spaces to joint spaces
  if( pucks.size() == 4 ){
    mpos2jpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jpos2mpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jtrq2mtrq.SetSize( 4, 4, VCT_ROW_MAJOR );
  }

  mpos2jpos.SetAll( 0.0 );
  jpos2mpos.SetAll( 0.0 );
  jtrq2mtrq.SetAll( 0.0 );

  mpos2jpos[0][0] = -0.0077437;
  mpos2jpos[1][1] = -0.0077437;
  mpos2jpos[2][2] = -0.0077437;
  mpos2jpos[3][3] = -0.057268;

  jpos2mpos[0][0] = 1.0 / mpos2jpos[0][0];
  jpos2mpos[1][1] = 1.0 / mpos2jpos[1][1]; 
  jpos2mpos[2][2] = 1.0 / mpos2jpos[2][2];
  jpos2mpos[3][3] = 1.0 / mpos2jpos[3][3];

  jtrq2mtrq[0][0] = -1.0;
  jtrq2mtrq[1][1] = -1.0; 
  jtrq2mtrq[2][2] = -1.0;
  jtrq2mtrq[3][3] = -1.0;

  //
  // Initialize the position of the BH8_280
  //
  
  for( size_t i=0; i<4; i++ ){
    pucks[i].SetProperty( devProperty::TIME2STOP, 250, true );
    pucks[i].SetProperty( devProperty::MAXTRQ, 1500, true );
  }
  
  Hi();
  osaSleep( 1.0 );

}

void devBH8_280::Hi(){
  for( size_t i=0; i<4; i++ )
    { pucks[i].SetProperty( devProperty::COMMAND, 13, false ); }
}

void devBH8_280::Open(){
  for( size_t i=0; i<4; i++ )
    { pucks[i].SetProperty( devProperty::COMMAND, 20 , false ); }
}

void devBH8_280::Close(){
  for( size_t i=0; i<4; i++ )
    { pucks[i].SetProperty( devProperty::COMMAND, 18 , false ); }
}

void devBH8_280::Home(){
  for( size_t i=0; i<4; i++ )
    { pucks[i].SetProperty( devProperty::COMMAND, 7 , false ); }
}

void devBH8_280::Default(){
  for( size_t i=0; i<4; i++ )
    { pucks[i].SetProperty( devProperty::COMMAND, 19 , false ); }
}

void devBH8_280::Read(){
  vctDynamicVector<double> q;
  RecvPositions( q );
  output->SetPosition( q );
}

void devBH8_280::Write(){

  vctDynamicVector<double> q;
  double t;
  input->GetPosition( q, t );
  
  if( q.size() == 4 ){
    vctDynamicVector<double> qtmp( q );

    for( size_t i=0; i<qtmp.size(); i++ )
      { if( qtmp[i] <= 0 ) { qtmp[i] = 0; } }

    SendPositions( qtmp );

  }

}

devBH8_280::Errno devBH8_280::SetPucksStatus( devProperty::Value ps, bool ver ){

  for( size_t i=0; i<pucks.size(); i++ ){

    if( pucks[i].SetProperty( devProperty::STATUS, ps, ver ) 
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to wake up the puck #" << i+1
			<< std::endl;
      return devBH8_280::EFAILURE;
    }

  }

  return devBH8_280::ESUCCESS;
}



// set the motor positions 
devBH8_280::Errno 
devBH8_280::SendPositions( const vctDynamicVector<double>& jq ){

  if( jq.size() != pucks.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected " << pucks.size() << " joint angles. "
		      << "Got " << jq.size()
		      << std::endl;
    return devBH8_280::EFAILURE;
  }

  // convert the joints positions to motor positions
  vctDynamicVector<double> mq = JointsPos2MotorsPos( jq );

  // for each puck, send a position 
  for(size_t i=0; i<pucks.size(); i++){
    devProperty::Value position;  // this is the position in encoder ticks
    
    // convert the motor positions to encoder ticks
    position = (devProperty::Value)floor((mq[i]*pucks[i].CountsPerRevolution())/
					 (2.0*M_PI) );

    // Set the motor position. Don't double check the position because the 
    // noise might change the encoder.
    if( pucks[i].SetProperty( devProperty::MECHANGLE_ENC, position, false ) 
	!= devPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to set pos of puck#: " 
			<< (int)pucks[i].GetID()
			<< std::endl;
    }

  }

  return devBH8_280::ESUCCESS;
}


// query the joint positions
devBH8_280::Errno devBH8_280::RecvPositions( vctDynamicVector<double>& jq ){

  // Query all the motor (broadcast group)
  if( groups[ devGroup::HAND_POSITION ].GetProperty( devProperty::POS ) 
      //if( groups[ devGroup::HAND_POSITION ].GetProperty( devProperty::POS ) 
      != devGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to query the pucks."
		      << std::endl;
    return devBH8_280::EFAILURE;
  }

  // this will hold the motor positions
  vctDynamicVector<double> mq( pucks.size(), 0.0 );

  // wait for the pucks to respond
  for(size_t i=0; i<pucks.size(); i++){

    // receive a response from a puck
    devCAN::Frame canframe;
    if( candev->Recv( canframe ) != devCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to receive a CAN frame."
			<< std::endl;
    }
    
    else{

      // determine where the canframe originated (-1 is for zero indexing)
      devPuck::ID pid = devPuck::OriginID( canframe );
      devProperty::ID propid;
      devProperty::Value position;

      // Unpack the can frame
      if( pucks[pid-11].UnpackCANFrame( canframe, propid, position ) 
	  != devPuck::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to unpack a CAN frame."
			  << std::endl;
      }

      // Ensure that the can frame provides a motor position
      if( propid != devProperty::POS){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Oops! Didn't receive a position."
			  << std::endl;
      }
      
      // convert the position from encoder ticks to radians
      mq[pid-11] = ( ((double)position) * 2.0 * M_PI  /
		     ((double)pucks[pid-11].CountsPerRevolution() ) );
    }
    
  }

  // convert the motor positions to joints positions
  jq = MotorsPos2JointsPos( mq );

  return devBH8_280::ESUCCESS;

}

// Send joint torques
devBH8_280::Errno devBH8_280::SendTorques( const vctDynamicVector<double>& jt ){
  
  if( jt.size() != pucks.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected " << pucks.size() << " joint torques. "
		      << "Got " << jt.size()
		      << std::endl;
    return devBH8_280::EFAILURE;
  }

  // convert the joint torques to motor torques
  vctDynamicVector<double> mt = JointsTrq2MotorsTrq( jt );

  // Is the upperarm group empty?
  if( !groups[devGroup::UPPERARM].Empty() ){

    // TODO: this block should be in devGroup

    // copy the first 4 torques in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<4; i++)
      { currents[i] = mt[i] * pucks[i].IpNm(); }
    
    // pack the torques in a can frames
    devCAN::Frame canframe;
    if( PackCurrents( canframe, devGroup::UPPERARM, currents ) 
	!= devBH8_280::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the torques."
			<< std::endl;
      return devBH8_280::EFAILURE;
    }

    // send the canframe (blocking)
    if( candev->Send( canframe ) != devCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to send upper arm torques." 
			<< std::endl;
      return devBH8_280::EFAILURE;
    }

  }
  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The upper arm group is empty!!!"
			<< std::endl;
    return devBH8_280::EFAILURE;
  }

  // Is the forearm group empty?
  if( !groups[devGroup::FOREARM].Empty() ){

    // if using the forearm, make sure that 7 torques are available
    if( mt.size() != 7 ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Expected 7 torques. Got " << mt.size()
			<< std::endl;
      return devBH8_280::EFAILURE;
    }
    
    // TODO: this block should be in devGroup

    // copy the last 3 torques in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<3; i++)
      { currents[i] = mt[i+4] * pucks[i+4].IpNm(); }

    // pack the torques in a CAN frame
    devCAN::Frame canframe;
    if( PackCurrents( canframe, devGroup::FOREARM, currents ) 
	!= devBH8_280::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the torques."
			<< std::endl;
      return devBH8_280::EFAILURE;
    }

    // Send the CAN frame
    if( candev->Send( canframe ) != devCAN::ESUCCESS ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to send forearm torques." 
			<< std::endl;
      return devBH8_280::EFAILURE;
    }
  }

  return devBH8_280::ESUCCESS;
}


// pack motor torques in a CAN frame
// this should go into devGroup
devBH8_280::Errno devBH8_280::PackCurrents( devCAN::Frame& canframe, 
					    devGroup::ID gid, 
					    const double I[4]) {
  
  // we can only pack torques for the upper arm and forearm groups
  if(gid == devGroup::UPPERARM || gid == devGroup::FOREARM){

    // copy each motor torques in this array with the correct index
    devProperty::Value currents[4] = {0, 0, 0, 0};

    // for each puck in the group
    for(devPuck::ID pid=groups[gid].FirstMember();
	            pid<=groups[gid].LastMember();
	            pid++){
      
      // get the index of the puck within its group [0,1,2,3]
      int idx =  pucks[pid-11].GroupIndex()-1;    // -1 because of zero index
      if( idx < 0 || 3 < idx ){                  // sanity check
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Illegal index." 
			  << std::endl;
	return devBH8_280::EFAILURE;
      }
      currents[ idx ] = (devProperty::Value)I[idx];   // cast the torque      
    }

    // pack the torques in a 8 bytes message (see the documentation)
    unsigned char msg[8];
    msg[0]= devProperty::TRQ | 0x80;
    msg[1]=(unsigned char)(( currents[0]>>6)&0x00FF);
    msg[2]=(unsigned char)(((currents[0]<<2)&0x00FC)|((currents[1]>>12)&0x0003));
    msg[3]=(unsigned char)(( currents[1]>>4)&0x00FF);
    msg[4]=(unsigned char)(((currents[1]<<4)&0x00F0)|((currents[2]>>10)&0x000F));
    msg[5]=(unsigned char)(( currents[2]>>2)&0x00FF);
    msg[6]=(unsigned char)(((currents[2]<<6)&0x00C0)|((currents[3]>>8) &0x003F));
    msg[7]=(unsigned char)(  currents[3]    &0x00FF);
    
    // build a can frame addressed to the group ID 
    canframe = devCAN::Frame( devGroup::CANID( gid ), msg, 8 );
    return devBH8_280::ESUCCESS;
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Invalid group ID." 
		      << std::endl;
    return devBH8_280::EFAILURE;
  }
}

vctDynamicVector<double> 
devBH8_280::MotorsPos2JointsPos( const vctDynamicVector<double>& mq )
{  return mpos2jpos*mq;  }

vctDynamicVector<double> 
devBH8_280::JointsPos2MotorsPos( const vctDynamicVector<double>& jq )
{  return jpos2mpos*jq;  }

vctDynamicVector<double> 
devBH8_280::JointsTrq2MotorsTrq( const vctDynamicVector<double>& jt )
{  return jtrq2mtrq*jt;  }
