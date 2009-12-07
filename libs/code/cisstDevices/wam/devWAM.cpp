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

#include <cisstDevices/wam/devWAM.hpp>
#include <cisstCommon/cmnLogger.h>

devWAM::devWAM( devCAN* canbus, size_t N ){
  // copy the can bus
  this->canbus = canbus;

  // The 
  safetymodule = devPuck( 10, canbus );

  pucks.clear();         // clear the pucks

  // create'n initialize the pucks with their ID and the canbus
  for(size_t i=1; i<=N; i++)
    pucks.push_back( devPuck( (devPuckID)(i), canbus ) );

  groups.clear();
  // create'n initialize the groups with their ID and the canbus
  for(devGroupID i=devGroup::BROADCAST; i<devGroup::PROPFEEDBACK; i++)
    groups.push_back( devGroup( i, canbus ) );

  // Add the pucks ID to each group
  // Pucks ID indices start at 1
  for(devPuckID i=1; i<=4; i++){
    groups[devGroup::BROADCAST].AddPuckIDToGroup( i );
    groups[devGroup::UPPERARM].AddPuckIDToGroup( i );
    groups[devGroup::UPPERARMPROP].AddPuckIDToGroup( i );
  }

  if( N == 7 ){
    for(devPuckID i=5; i<=7; i++){
      groups[devGroup::BROADCAST].AddPuckIDToGroup( i );
      groups[devGroup::FOREARM].AddPuckIDToGroup( i );
      groups[devGroup::FOREARMPROP].AddPuckIDToGroup( i );
    }
  }

  // wake up everyone
  if( groups[devGroup::BROADCAST].SetProperty( devPropertyCommon::STATUS, 
					       devGroupStatus::READY ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to wake up the pucks." 
		      << std::endl;
    exit(-1);
  }
  usleep(300000);

  // make sure that the safety module is there
  if( safetymodule.GetProperty( devPropertyCommon::STATUS ) != 
      devPuckStatus::READY ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": safety module is offline." 
		      << std::endl;
    exit(-1);
  }
  CMN_LOG_RUN_WARNING << "Safety module online...[ OK ]" << std::endl;

  // configure the safety module?
  safetymodule.SetProperty( devPropertySM::VELWARNING, 2000 );
  safetymodule.SetProperty( devPropertySM::VELFAULT, 6000 );
  //safetymodule.set( SMProperty::TRQWARNING,1000 );
  //safetymodule.set( SMProperty::TRQFAULT, 2000 );
  safetymodule.SetProperty( devPropertySM::TRQWARNING,2000 );
  safetymodule.SetProperty( devPropertySM::TRQFAULT, 4000 );

  // count number of pucks detected
  size_t numpucks = 0;
  for(size_t i=0; i<pucks.size(); i++){
    if(pucks[i].GetProperty( devPropertyCommon::STATUS) == devPuckStatus::READY)
      numpucks++;
  }

  // make sure that all the pucks are there
  if( numpucks != pucks.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Found " << numpucks << ". "
		      << "Expected " << pucks.size() 
		      << std::endl;
    exit(-1);
  }
  std::cout << "Number of pucks online...[ OK ]" << std::endl;
  
  // configure the pucks
  for(size_t i=0; i<pucks.size(); i++){
    if( pucks[i].Configure() ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to initialize puck #" << i 
			<< std::endl;
      exit(-1);
    }
  }
}

bool devWAM::PackCurrents( devCANFrame& canframe, 
			   devGroupID gid, 
			   const double I[4]){

  // we can only pack torques for these groups
  if(gid == devGroup::UPPERARM || gid == devGroup::FOREARM){

    // get the motor currents in the right group index
    devPropertyValue currents[4] = {0, 0, 0, 0};
    for(devPuckID i=groups[gid].FirstMember()-1; 
	          i<=groups[gid].LastMember()-1; 
	          i++){

      // adjust -1 because puck ID indices start at 1
      int idx =  pucks[i].Index()-1;
      if( idx < 0 ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			  << ": Illegal index." 
			  << std::endl;
	return true;
      }
      currents[ idx ] = (devPropertyValue)I[i];
    }

    // pack the currents in a 8 bytes message
    unsigned char msg[8];
    msg[0]= devPropertyMotor::TRQ | 0x80;
    msg[1]=(unsigned char)(( currents[0]>>6)&0x00FF);
    msg[2]=(unsigned char)(((currents[0]<<2)&0x00FC)|((currents[1]>>12)&0x0003));
    msg[3]=(unsigned char)(( currents[1]>>4)&0x00FF);
    msg[4]=(unsigned char)(((currents[1]<<4)&0x00F0)|((currents[2]>>10)&0x000F));
    msg[5]=(unsigned char)(( currents[2]>>2)&0x00FF);
    msg[6]=(unsigned char)(((currents[2]<<6)&0x00C0)|((currents[3]>>8) &0x003F));
    msg[7]=(unsigned char)(  currents[3]    &0x00FF);
    
    canframe = devCANFrame( devGroup::CANId( gid ), msg, 8 );
    return false;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Invalid group ID." 
		      << std::endl;
    return true;
  }
}

bool devWAM::SendPositions( const vctDynamicVector<double>& q ){
  
  // let the safety module ignore a few errors
  if( safetymodule.SetProperty( devPropertySM::IGNOREFAULT, 8 ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to set the safety module>" 
		      <<std::endl;
    return true;
  }

  for(size_t i=0; i<pucks.size(); i++){
    devPropertyValue position;
 
    // convert the motor positions to encoder ticks
    position = (devPropertyValue)floor( (q[i]*pucks[i].CountsPerRevolution()) /
					(2.0*M_PI) );
    // set the motor position
    if(pucks[i].SetProperty( devPropertyMotor::POS, position, false) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to set pos of puck#: " << i+1
			<<std::endl;
      return true;
    }
   usleep(1000);
  }

  // reset the safety module
  if( safetymodule.SetProperty( devPropertySM::IGNOREFAULT, 1 ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to set the safety module>" 
		      << std::endl;
    return true;
  }

  return false;
}

bool devWAM::SendCurrents( const vctDynamicVector<double>& I ){

  if(!groups[devGroup::UPPERARM].Empty()){

    // copy the first 4 currents in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<4; i++)
      currents[i] = I[i];

    // pack the currents in a can frames
    devCANFrame canframe;
    if( PackCurrents( canframe, devGroup::UPPERARM, currents ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to pack the currents."
			<< std::endl;
      return true;
    }

    // send the canframe
    if( canbus->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to send upper arm currents." 
			<< std::endl;
      return true;
    }

  }

  if(!groups[devGroup::FOREARM].Empty()){

    // if using the forearm, make sure that 7 currents are available
    if( I.size() != 7 ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Expected 7 currents. Got " << I.size()
			<< std::endl;
      return true;
    }
    
    // copy the last 3 currents in a 4 array
    double currents[4] = {0.0, 0.0, 0.0, 0.0};
    for( size_t i=0; i<3; i++)
      currents[i] = I[i+4];

    // pack the currents in a CAN frame
    devCANFrame canframe;
    if( PackCurrents( canframe, devGroup::FOREARM, currents ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to pack the currents."
			<< std::endl;
      return true;
    }

    // Send the CAN frame
    if( canbus->Send( canframe, true ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to send forearm currents." 
			<< std::endl;
      return true;
    }
  }
  return false;
}

bool devWAM::RecvPositions( vctDynamicVector<double>& q ){

  // Query all the motor (broadcast group)
  if( groups[ devGroup::BROADCAST ].GetProperty( devPropertyMotor::POS ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
		      << ": Failed to query the motors."
		      << std::endl;
    return true;
  }

  // wait for the pucks to respond
  for(size_t i=0; i<pucks.size(); i++){

    // receive a response from a puck
    devCANFrame canframe;
    if( canbus->Recv( canframe, true ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to receive a CAN frame."
			<< std::endl;
    }

    else{
      // determine the origin of the canframe (-1 is for zero indexing)
      devPuckID pid = devPuck::Origin( canframe ) - 1;

      devPropertyID propid;
      devPropertyValue position;
      // Unpack the can frame
      if( pucks[pid].UnpackCANFrame( canframe, propid, position ) ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to unpack a CAN frame."
			<< std::endl;
      }

      // Ensure that the can frame provides a motor position
      if( propid != devPropertyMotor::POS){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			  << ": Oops! Didn't receive a position."
			  << std::endl;
      }

      q[ pid ] = ( ((double)position) * 2.0 * M_PI  /
		   ((double)pucks[pid].CountsPerRevolution() ) );
    }
  }

  return false;
}
