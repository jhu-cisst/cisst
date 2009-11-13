/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robWAM.h>
#include <cisstDevices/devCAN/devPuckProperties.h>

#include <fstream>
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robWAM::robWAM( const std::string& manfile,
		const std::string& toolfile,
		const math::vctFrame4x4<double,VCT_ROW_MAJOR>& Rt,
		const math::vctDynamicVector<double>& q0,
		const std::string& wamfile ) : 
  robManipulator( manfile, toolfile, Rt ){

  safetymodule = robPuck( 10, canbus );

  pucks.clear();         // clear the pucks
  // create'n initialize the pucks with their ID and the canbus
  for(size_t i=1; i<=motors.size(); i++)
    pucks.push_back( Puck( (PuckID)(i), canbus ) );

  groups.clear();
  // create'n initialize the groups with their ID and the canbus
  for(GroupID i=Group::BROADCAST; i<Group::PROPFEEDBACK; i++)
    groups.push_back( Group( i, canbus ) );

  // Add the pucks ID to each group
  for(PuckID i=1; i<=4; i++){
    groups[Group::BROADCAST].push_back( i );
    groups[Group::UPPERARM].push_back( i );
    groups[Group::UPPERARMPROP].push_back( i );
  }

  if( motors.size() == 7 ){
    for(PuckID i=5; i<=7; i++){
      groups[Group::BROADCAST].push_back( i );
      groups[Group::FOREARM].push_back( i );
      groups[Group::FOREARMPROP].push_back( i );
    }
  }

  // wake up everyone
  if(groups[Group::BROADCAST].set(CommonProperty::STATUS, 
					   Status::READY) 
     == FAILURE){
    std::cerr << "robWAM::robWAM: Failed to wake up the pucks." << std::endl;
    exit(FAILURE);
  }
  usleep(300000);

  // make sure that the safety module is there
  if( safetymodule.get( CommonProperty::STATUS ) != 
      Status::READY ){
    std::cerr << "robWAM::robWAM: safety module is offline." << std::endl;
    exit(FAILURE);
  }
  std::cout << "Safety module online...[ OK ]" << std::endl;

  // configure the safety module?
  safetymodule.set( SMProperty::VELWARNING, 1000 );
  safetymodule.set( SMProperty::VELFAULT, 2000 );
  safetymodule.set( SMProperty::TRQWARNING,1000 );
  safetymodule.set( SMProperty::TRQFAULT, 2000 );

  // count number of pucks detected
  size_t numpucks = 0;
  for(size_t i=0; i<pucks.size(); i++){
    if(pucks[i].get(CommonProperty::STATUS)  == Status::READY)
      numpucks++;
  }

  // make sure that all the pucks are there
  if( numpucks != pucks.size() ){
    std::cerr << "robWAM::robWAM: Found " << numpucks << ". "
	      << "Expected " << pucks.size() << std::endl;
    exit(FAILURE);
  }
  std::cout << "Number of pucks online...[ OK ]" << std::endl;

  // configure the pucks
  for(size_t i=0; i<pucks.size(); i++){
    if( pucks[i].init() == FAILURE){
      std::cerr << "robWAM::robWAM: Failed to initialize puck #" << i << std::endl;
      exit(FAILURE);
    }
  }

  // Here we need to give the IpNm to each motor. This is because the
  // pucks IpNm are read from the firmware and the motors must be aware of this
  for(size_t i=0; i<pucks.size(); i++)  motors[i].ipnm( pucks[i].ipnm() );


  // Super lame. There's no reason why theses matrices cannot be static
  // The freakin' numbers arent going to change so why the hell do they need to
  // loaded from a file?
  Mpos2Jpos = NULL;
  Jpos2Mpos = NULL;
  Jtrq2Mtrq = NULL;

  Mpos2Jpos = rmatrix( 0, motors.size(), 0, motors.size() );
  Jpos2Mpos = rmatrix( 0, motors.size(), 0, motors.size() );
  Jtrq2Mtrq = rmatrix( 0, motors.size(), 0, motors.size() );

  if( Mpos2Jpos == NULL || Jpos2Mpos == NULL || Jtrq2Mtrq == NULL ){
    std::cerr << "robWAM::init: Failed to allocate the matrices." << std::endl;
    exit(FAILURE);
  }

  std::ifstream ifs;
  ifs.open( wamfile.data() );
  if( !ifs ){
    std::cerr << "robWAM::init: Failed to open " << wamfile << std::endl;
    exit(FAILURE);
  }

  // load all three matrices
  for(size_t i=0; i<motors.size(); i++) 
    for(size_t j=0; j<motors.size(); j++)
      ifs >> Mpos2Jpos[i][j];

  for(size_t i=0; i<motors.size(); i++) 
    for(size_t j=0; j<motors.size(); j++)
      ifs >> Jpos2Mpos[i][j];

  for(size_t i=0; i<motors.size(); i++) 
    for(size_t j=0; j<motors.size(); j++)
      ifs >> Jtrq2Mtrq[i][j];

  jpos(q0);

}

// convert motors positions to joints positions
void robWAM::mpos2jpos(math::real dt){
  std::vector<math::real> q(motors.size(), 0);

  for(size_t r=0; r<motors.size(); r++)
    for(size_t c=0; c<motors.size(); c++)
      q[r] += Mpos2Jpos[r][c] * motors[c].pos();

  for(size_t i=0; i<joints.size(); i++){
    joints[i].pos( q[i], __DBL_MAX__ );
    links[i].pos( joints[i].pos() );
  }

}

// convert joints positions to motors positions
void robWAM::jpos2mpos(){
  std::vector<math::real> q(joints.size(), 0);

  for(size_t r=0; r<joints.size(); r++)
    for(size_t c=0; c<joints.size(); c++)
	q[r] += Jpos2Mpos[r][c] * joints[c].pos();

  for(size_t i=0; i<motors.size(); i++)
    motors[i].pos( q[i] );
}

// convert links torques to motor torques
void robWAM::jtrq2mtrq(){
  std::vector<math::real> t(motors.size(), 0);

  for(size_t r=0; r<joints.size(); r++)
    for(size_t c=0; c<joints.size(); c++)
      t[r] += Jtrq2Mtrq[r][c] * joints[c].trq();

  for(size_t i=0; i<motors.size(); i++)
    motors[i].trq( t[i] );

}

CANFrame robWAM::packtrq(GroupID gid){

  // we can only pack torques for these groups
  if(gid == Group::UPPERARM || gid == Group::FOREARM){

    // this needs a bit of work but whatever
    PropertyValue currents[4] = {0, 0, 0, 0};

    // get the motor currents in the right group index
    // adjust -1 because puck ID and the group indexes start at 1
    for(PuckID i=groups[gid].front()-1; i<=groups[gid].back()-1; i++)
      currents[pucks[i].index()-1]=(PropertyValue)motors[i].current();

    // this bit is taken from Brian's code
    // this should really be packed as a PropertyValue and set by groups
    // but I'm not sure in which order the bytes needs to be ordered in a 64 long
    uint8_t msg[8];
    msg[0] = MotorProperty::TRQ | 0x80;
    msg[1] = (uint8_t)(( currents[0]>>6)&0x00FF);
    msg[2] = (uint8_t)(((currents[0]<<2)&0x00FC)|((currents[1]>>12)&0x0003));
    msg[3] = (uint8_t)(( currents[1]>>4)&0x00FF);
    msg[4] = (uint8_t)(((currents[1]<<4)&0x00F0)|((currents[2]>>10)&0x000F));
    msg[5] = (uint8_t)(( currents[2]>>2)&0x00FF);
    msg[6] = (uint8_t)(((currents[2]<<6)&0x00C0)|((currents[3]>>8) &0x003F));
    msg[7] = (uint8_t)(  currents[3]    &0x00FF);
    
    return CANFrame( Group::canid( gid ), msg, 8 );
  }
  else{
    std::cerr << "robWAM::packtrq: Invalid group." << std::endl;
    return CANFrame();
  }
}

Retval robWAM::recv_mtrq(){return SUCCESS;}
Retval robWAM::send_mpos(){
  
  // let the safety module ignore a few errors
  if( safetymodule.set( SMProperty::IGNOREFAULT, 8 ) ){
    std::cerr << "robWAM::sent_mpos(): Failed to set the safety module>" <<std::endl;
    return FAILURE;
  }

  for(size_t i=0; i<pucks.size(); i++){
    PropertyValue position;
 
    // convert the motor positions to encoder ticks
    position = (PropertyValue)floor( (motors[i].pos()*pucks[i].cpr())
					      /
					      (2.0*M_PI) );
    // set the motor position
    if(pucks[i].set(MotorProperty::POS,position,0)==FAILURE){
      std::cerr << "robWAM::send_mpos: Failed to set pos of puck#: "<<i+1<<std::endl;
      return FAILURE;
    }
   usleep(1000);
  }

  // reset the safety module
  if( safetymodule.set( SMProperty::IGNOREFAULT, 1 ) ){
    std::cerr << "robWAM::sent_mpos(): Failed to set the safety module>" <<std::endl;
    return FAILURE;
  }

  return SUCCESS;
}

Retval robWAM::send_mtrq(){
  //cout << "**********" << endl;
  if(!groups[Group::UPPERARM].empty()){

    CANFrame canframe = packtrq( Group::UPPERARM );
    //cout << canframe << endl;
    if(canbus->send((void*)&canframe, sizeof(CANID), true) == FAILURE){
      std::cerr << "robWAM::send_mtrq: Failed to send torques." << std::endl;
      return FAILURE;
    }

  }

  if(!groups[Group::FOREARM].empty()){

    CANFrame canframe = packtrq( Group::FOREARM );
    //cout << canframe << endl;
    if(canbus->send((void*)&canframe, sizeof(CANID), true) == FAILURE){
      std::cerr << "robWAM::send_mtrq: Failed to send torques." << std::endl;
      return FAILURE;
    }

  }
  return SUCCESS;
}

Retval robWAM::recv_mpos(){

  if( groups[ Group::BROADCAST ].get( MotorProperty::POS )
      == FAILURE){
    std::cerr << "robWAM::recv_mpos: Failed to query the positions." << std::endl;
    return FAILURE;
  }

  for(size_t i=0; i<pucks.size(); i++){

    // receive'n unpack
    CANFrame canframe;
    size_t size;
    if(canbus->recv((void*)&canframe, size, true) == FAILURE){
      std::cerr << "robWAM::recv_mpos: Failed to receive a position." << std::endl;
      return FAILURE;
    }

    PuckID pid = Puck::origin( canframe )-1;
    //cout << canframe << endl << endl;
    PropertyID propid;
    PropertyValue position;
    if( pucks[pid].unpack( canframe, propid, position ) == FAILURE ){
      std::cerr << "robWAM::recv_mpos: Failed to unpack the CAN frame." <<std::endl;
      return FAILURE;
    }

    if(propid != MotorProperty::POS){
      std::cerr << "robWAM::recv_mpos: Oops! Didn't receive a position."<<std::endl;
      return FAILURE;
    }

    motors[pid].pos( 2.0*M_PI*(math::real)position
		     / 
		     ((math::real)pucks[pid].cpr()) );
  }

  return SUCCESS;
}
