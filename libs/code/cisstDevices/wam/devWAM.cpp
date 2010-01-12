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

#include <cisstDevices/wam/devWAM.h>
#include <cisstCommon/cmnLogger.h>
using namespace std;

CMN_IMPLEMENT_SERVICES(devWAM);

const std::string devWAM::PositionInterfaceName = "JointsPositionsInterface";
const std::string devWAM::ReadPositionsCommandName = "ReadJointsPositions";
const std::string devWAM::WritePositionsCommandName = "WriteJointsPositions";

const std::string devWAM::TorqueInterfaceName   = "JointsTorquesInterface";
const std::string devWAM::ReadTorquesCommandName= "ReadJointsTorques";

// main constructor
devWAM::devWAM( const std::string& taskname, 
		double period, 
		devCAN* candev, 
		size_t N ) : 
  mtsTaskPeriodic( taskname, period, true ) {

  firstruns = 0;

  // The WAM provides a position interface
  mtsProvidedInterface* positioninterface = 
    AddProvidedInterface( PositionInterfaceName );

  if( positioninterface ){

    jointspositions.SetSize(7);
    jointspositions.SetAll(0.0);

    StateTable.AddData( jointspositions, "JointsPositions" );

    // the position interface has a "SetPositions" command
    positioninterface->AddCommandWrite( &devWAM::SendPositions, 
					this, 
					WritePositionsCommandName );

    // the position interface has a "GetPositions" command
    positioninterface->AddCommandReadState( StateTable, 
					    jointspositions,
					    ReadPositionsCommandName );
  }

  // The WAM requires a torque interface
  AddRequiredInterface( devWAM::TorqueInterfaceName );

  // only 4 or 7 pucks are allowed
  if( N != 4 && N!= 7 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Expected 4 or 7 pucks. Got " << N << "."
		      << std::endl;
    exit(-1);
  }
  pucks.resize( N );  // reserve N pucks

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

void devWAM::Startup( ){
  mtsDeviceInterface* torqueinterface =
    GetProvidedInterfaceFor( devWAM::TorqueInterfaceName );
  
  if( torqueinterface ){
    ReadJointsTorques.Bind( torqueinterface,
			      devWAM::ReadTorquesCommandName );
  }

}

void devWAM::Cleanup()
{ candev->Close(); }

void devWAM::Configure( const std::string& ){

  // open the can device
  if( candev->Open() ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Failed to open the CAN device."
		      << std::endl;
    exit(-1);
  }
  
  CMN_LOG_INIT_VERBOSE << "The CAN device is opened." << std::endl;

  // First thing first. Set up the safety module
  // Create the puck representing the safety module
  safetymodule = devPuck( devWAM::SAFETYMODULE_PID, candev );

  CMN_LOG_INIT_VERBOSE << "Querying the status of the safety module."<<std::endl;

  // Configure the safety module
  // Query the status of the safety module
  if( safetymodule.GetProperty( devPropertyCommon::STATUS ) != 
      devPuckStatus::READY ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Safety module is offline." 
		      << std::endl;
    exit(-1);
  }

  CMN_LOG_INIT_VERBOSE << "The safety module is online." << std::endl;
  
  // This sets the warning for high velocity
  CMN_LOG_INIT_VERBOSE << "Setting velocity warning." << std::endl;
  if( safetymodule.SetProperty( devPropertySM::VELWARNING, 4000, true ) ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Unable to set the velocity warning." 
		      << std::endl;
    exit(-1);
  }

  // This sets the fault for high velocity
  CMN_LOG_INIT_VERBOSE << "Setting velocity fault." << std::endl;
  if( safetymodule.SetProperty( devPropertySM::VELFAULT, 8000, true ) ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Unable to set the velocity fault." 
		      << std::endl;
    exit(-1);
  }

  // This sets the warning for high torques
  CMN_LOG_INIT_VERBOSE << "Setting torque warning." << std::endl;
  if( safetymodule.SetProperty( devPropertySM::TRQWARNING, 4000, true ) ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Unable to set the torques fault." 
		      << std::endl;
    exit(-1);
  }

  // This sets the fault for high torques
  CMN_LOG_INIT_VERBOSE << "Setting torque fault." << std::endl;
  if( safetymodule.SetProperty( devPropertySM::TRQFAULT, 8000, true ) ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Unable to set the torques fault." 
		      << std::endl;
    exit(-1);
  }

  CMN_LOG_INIT_VERBOSE << "Safety module is good to go" << std::endl;

  // create'n initialize the pucks with their ID and the CAN device. Note that
  // the pucks IDs start at 1.
  for( size_t i=0; i<pucks.size(); i++ )
    pucks[i] = devPuck( (devPuckID)(i+1), candev );

  //
  // Now set up the groups
  //

  groups.clear();
  // create'n initialize the groups with their ID and the canbus
  // The group ID are available in devGroup.
  for(devGroupID i=devGroup::BROADCAST; i<devGroup::PROPFEEDBACK; i++)
    groups.push_back( devGroup( i, candev ) );
  
  // Add the pucks ID to each group
  // The first loop handles the pucks in the upper arm
  // Pucks ID indices start at 1
  for(devPuckID i=1; i<=4; i++){
    groups[devGroup::BROADCAST].AddPuckIDToGroup( i );
    groups[devGroup::UPPERARM].AddPuckIDToGroup( i );
    groups[devGroup::UPPERARMPROP].AddPuckIDToGroup( i );
  }
  
  // The second loop handles the pucks in the forearm (if any)
  if( pucks.size() == 7 ){
    for(devPuckID i=5; i<=7; i++){
      groups[devGroup::BROADCAST].AddPuckIDToGroup( i );
      groups[devGroup::FOREARM].AddPuckIDToGroup( i );
      groups[devGroup::FOREARMPROP].AddPuckIDToGroup( i );
    }
  }
  
  // wake up all the pucks. This broadcast the command that every puck should
  // change its status to ready 
  CMN_LOG_INIT_VERBOSE << "Waking up the pucks" << std::endl;
  if( groups[devGroup::BROADCAST].SetProperty( devPropertyCommon::STATUS, 
					       devPuckStatus::READY ) ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			<< ": Failed to wake up the pucks." 
		      << std::endl;
    exit(-1);
  }
  // Wait a bit to let the pucks change their status
  usleep(300000);

  // count number of pucks that are ready
  size_t npucksready = 0;
  CMN_LOG_INIT_VERBOSE << "Querying the status of the pucks" << std::endl;
  for(size_t i=0; i<pucks.size(); i++){
    if(pucks[i].GetProperty(devPropertyCommon::STATUS) == devPuckStatus::READY){
      CMN_LOG_INIT_VERBOSE << "Puck " << i << " is online." << std::endl;
      npucksready++;
    }
    else{
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			<< ": Puck " << i << " is offline." 
			<< std::endl;
    }
  }

  // make sure that all the pucks are ready
  if( npucksready != pucks.size() ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		      << ": Found " << npucksready << ". "
		      << "Expected " << pucks.size() 
		      << std::endl;
    exit(-1);
  }
  
  CMN_LOG_INIT_VERBOSE << "All the pucks on online." << std::endl;
  
  // configure the pucks
  for(size_t i=0; i<pucks.size(); i++){
    if( pucks[i].Configure() ){
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			<< ": Failed to configure puck #" << (int)pucks[i].ID()
			<< std::endl;
      exit(-1);
    }
  }
 
  if( pucks.size() == 7 ){
    mpos2jpos.SetSize( 7, 7, VCT_ROW_MAJOR );
    jpos2mpos.SetSize( 7, 7, VCT_ROW_MAJOR );
    jtrq2mtrq.SetSize( 7, 7, VCT_ROW_MAJOR );
  }
  else if( pucks.size() == 4 ){
    mpos2jpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jpos2mpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jtrq2mtrq.SetSize( 4, 4, VCT_ROW_MAJOR );
  }

  mpos2jpos.SetAll( 0.0 );
  jpos2mpos.SetAll( 0.0 );
  jtrq2mtrq.SetAll( 0.0 );

  mpos2jpos[0][0] = -0.0238095;
  mpos2jpos[1][1] =  0.0176991;   mpos2jpos[1][2] = -0.0176991;
  mpos2jpos[2][1] = -0.0297345;   mpos2jpos[2][2] = -0.0297345;
  mpos2jpos[3][3] = -0.0555556;

  jpos2mpos[0][0] = -42.0;
  jpos2mpos[1][1] =  28.25;       jpos2mpos[1][2] = -16.8155;
  jpos2mpos[2][1] = -28.25;       jpos2mpos[2][2] = -16.8155;
  jpos2mpos[3][3] = -18.0;

  jtrq2mtrq[0][0] = -0.0238095;
  jtrq2mtrq[1][1] =  0.0176991;   jtrq2mtrq[1][2] = -0.0297345;
  jtrq2mtrq[2][1] = -0.0176991;   jtrq2mtrq[2][2] = -0.0297345;
  jtrq2mtrq[3][3] = -0.0555556;

  if( pucks.size() == 7 ){

    mpos2jpos[4][4] =  0.0527426; mpos2jpos[4][5] = 0.0527426;
    mpos2jpos[5][4] = -0.0527426; mpos2jpos[5][5] = 0.0527426;
    mpos2jpos[6][6] = -0.0669792;

    jpos2mpos[4][4] =   9.48;     jpos2mpos[4][5] = -9.48;
    jpos2mpos[5][4] =   9.48;     jpos2mpos[5][5] =  9.48;
    jpos2mpos[6][6] = -14.93;

    jtrq2mtrq[4][4] =  0.0527426; jtrq2mtrq[4][5] = -0.0527426;
    jtrq2mtrq[5][4] =  0.0527426; jtrq2mtrq[5][5] =  0.0527426;
    jtrq2mtrq[6][6] = -0.0669792;

  }

  vctDynamicVector<double> qi(7, 0.0);
  qi[1] = -2.0;
  qi[3] =  M_PI;
  
  SendPositions( qi );
  
}

void devWAM::Run(){

  ProcessQueuedCommands();

  // Querry the pucks to get the joints positions
  if( RecvPositions( jointspositions ) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to receive joints positions." 
		      << std::endl;
  }
  
  // Querry the controller to get the joints torques
  mtsVector<double> t;;

  ReadJointsTorques( t );
  if( firstruns<1000 ){
    t.SetAll(0.0);
    firstruns++;
  }

  SendTorques( t );

}

// pack motor torques in a CAN frame
bool devWAM::PackCurrents( devCANFrame& canframe, 
			   devGroupID gid, 
			   const double I[4]) {

  // we can only pack torques for the upper arm and forearm groups
  if(gid == devGroup::UPPERARM || gid == devGroup::FOREARM){

    // copy each motor torques in this array with the correct index
    devPropertyValue currents[4] = {0, 0, 0, 0};

    // for each puck in the group
    for(devPuckID i=groups[gid].FirstMember()-1; // -1 because of zero index
	          i<=groups[gid].LastMember()-1; // -1 because of zero index
	          i++){
      
      // get the index of the puck within its group [0,1,2,3]
      int idx =  pucks[i].Index()-1;             // -1 because of zero index
      if( idx < 0 || 3 < idx ){                  // sanity check
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Illegal index." 
			  << std::endl;
	return true;
      }
      currents[ idx ] = (devPropertyValue)I[idx];   // cast the torque
      
    }

    // pack the torques in a 8 bytes message (see the documentation)
    unsigned char msg[8];
    msg[0]= devPropertyMotor::TRQ | 0x80;
    msg[1]=(unsigned char)(( currents[0]>>6)&0x00FF);
    msg[2]=(unsigned char)(((currents[0]<<2)&0x00FC)|((currents[1]>>12)&0x0003));
    msg[3]=(unsigned char)(( currents[1]>>4)&0x00FF);
    msg[4]=(unsigned char)(((currents[1]<<4)&0x00F0)|((currents[2]>>10)&0x000F));
    msg[5]=(unsigned char)(( currents[2]>>2)&0x00FF);
    msg[6]=(unsigned char)(((currents[2]<<6)&0x00C0)|((currents[3]>>8) &0x003F));
    msg[7]=(unsigned char)(  currents[3]    &0x00FF);
    
    // build a can frame addressed to the group ID 
    canframe = devCANFrame( devGroup::CANId( gid ), msg, 8 );
    return false;
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Invalid group ID." 
		      << std::endl;
    return true;
  }
}

// set the motor positions 
void devWAM::SendPositions( const mtsVector<double>& jq ){
  
  if( jq.size() != pucks.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected " << pucks.size() << " joint angles. "
		      << "Got " << jq.size()
		      << std::endl;
  }

  // let the safety module ignore a few faults
  // this is necessary because otherwise the safety module will monitor a large
  // change of joint position in a short amount of time and trigger a velocity 
  // fault.
  if( safetymodule.SetProperty( devPropertySM::IGNOREFAULT, 8 ) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to configure the safety module" 
		      << std::endl;
  }
  
  // convert the joints positions to motor positions
  vctDynamicVector<double> mq = JointsPos2MotorsPos( jq );

  // for each puck, send a position 
  for(size_t i=0; i<pucks.size(); i++){
    devPropertyValue position;  // this is the position in encoder ticks
    
    // convert the motor positions to encoder ticks
    position = (devPropertyValue)floor( (mq[i]*pucks[i].CountsPerRevolution()) /
					(2.0*M_PI) );
    
    // Set the motor position. Don't double check the position because the 
    // noise might change the encoder.
    if( pucks[i].SetProperty( devPropertyMotor::POS, position, false ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to set pos of puck#: " <<(int)pucks[i].ID()
			<< std::endl;
    }

    usleep(1000); // don't know if this is necessary anymore
  }

  // reset the safety module
  if( safetymodule.SetProperty( devPropertySM::IGNOREFAULT, 1 ) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to configure the safety module>" 
		      << std::endl;
  }
}

// Send joint torques
void devWAM::SendTorques( const mtsVector<double>& jt ){

  if( jt.size() != pucks.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected " << pucks.size() << " joint torques. "
		      << "Got " << jt.size()
		      << std::endl;
  }

  // convert the joint torques to motor torques
  vctDynamicVector<double> mt = JointsTrq2MotorsTrq( jt );

  // Is the upperarm group empty?
  if( !groups[devGroup::UPPERARM].Empty() ){

    // copy the first 4 torques in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<4; i++)
      currents[i] = mt[i] * pucks[i].IpNm();

    // pack the torques in a can frames
    devCANFrame canframe;
    if( PackCurrents( canframe, devGroup::UPPERARM, currents ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the torques."
			<< std::endl;
    }

    // send the canframe (blocking)
    if( candev->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to send upper arm torques." 
			<< std::endl;
    }

  }
  else{
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
			<< ": The upper arm group is empty!!!"
			<< std::endl;
  }

  // Is the forearm group empty?
  if( !groups[devGroup::FOREARM].Empty() ){

    // if using the forearm, make sure that 7 torques are available
    if( mt.size() != 7 ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Expected 7 torques. Got " << mt.size()
			<< std::endl;
    }
    
    // copy the last 3 torques in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<3; i++)
      currents[i] = mt[i+4] * pucks[i+4].IpNm();

    // pack the torques in a CAN frame
    devCANFrame canframe;
    if( PackCurrents( canframe, devGroup::FOREARM, currents ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to pack the torques."
			<< std::endl;
    }

    // Send the CAN frame
    if( candev->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to send forearm torques." 
			<< std::endl;
    }
  }
}

// query the joint positions
bool devWAM::RecvPositions( vctDynamicVector<double>& jq ){

  // Query all the motor (broadcast group)
  if( groups[ devGroup::BROADCAST ].GetProperty( devPropertyMotor::POS ) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to query the pucks."
		      << std::endl;
    return true;
  }

  // this will hold the motor positions
  vctDynamicVector<double> mq( pucks.size(), 0.0 );

  // wait for the pucks to respond
  for(size_t i=0; i<pucks.size(); i++){

    // receive a response from a puck
    devCANFrame canframe;
    if( candev->Recv( canframe, true ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Failed to receive a CAN frame."
			<< std::endl;
    }

    else{

      // determine where the canframe originated (-1 is for zero indexing)
      devPuckID pid = devPuck::Origin( canframe ) - 1;

      devPropertyID propid;
      devPropertyValue position;
      // Unpack the can frame
      if( pucks[pid].UnpackCANFrame( canframe, propid, position ) ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Failed to unpack a CAN frame."
			  << std::endl;
      }
      
      // Ensure that the can frame provides a motor position
      if( propid != devPropertyMotor::POS){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << ": Oops! Didn't receive a position."
			  << std::endl;
      }

      // convert the position from encoder ticks to radians
      mq[ pid ] = ( ((double)position) * 2.0 * M_PI  /
		   ((double)pucks[pid].CountsPerRevolution() ) );
    }
  }
  
  // convert the motor positions to joints positions
  jq = MotorsPos2JointsPos( mq );
  
  return false;
}

vctDynamicVector<double> 
devWAM::MotorsPos2JointsPos( const vctDynamicVector<double>& mq )
{  return mpos2jpos*mq;  }

vctDynamicVector<double> 
devWAM::JointsPos2MotorsPos( const vctDynamicVector<double>& jq )
{  return jpos2mpos*jq;  }

vctDynamicVector<double> 
devWAM::JointsTrq2MotorsTrq( const vctDynamicVector<double>& jt )
{  return jtrq2mtrq*jt;  }
