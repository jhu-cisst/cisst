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

#if (CISST_OS == CISST_LINUX_XENOMAI)

#include <cisstDevices/can/devRTSocketCAN.hpp>
#include <cisstCommon/cmnLogger.h>

#include <stdio.h>

devRTSocketCAN::devRTSocketCAN( const std::string& devname, 
				devCANRate rate ) : devCAN( rate ){
  this->devicename = devname;
}

bool devRTSocketCAN::Open(){

  struct ifreq ifr;

  // create a socket
  canfd = rt_dev_socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if( canfd < 0 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror("Couldn't create a CAN socket: ");
    return true;
  }

  // Get CAN interface index by name
  strncpy(ifr.ifr_name, devicename.data(), IFNAMSIZ);
  if( rt_dev_ioctl(canfd, SIOCGIFINDEX, &ifr) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror( "Couldn't get the CAN interface index by name:  " );
    return true;
  }

  // Set CAN filters
  // These are WAM specific filters
  filters[0].can_mask = 0x0000041F;  // mask broadcast to a group
  filters[0].can_id   = 0x00000403;  // allow group 3

  filters[1].can_mask = 0x0000041F;  // mask broadcast to a group
  filters[1].can_id   = 0x00000406;  // allow group 6

  filters[2].can_mask = 0x0000041F;  // mask broadcast to a group
  filters[2].can_id   = 0x00000000;  // allow direct messages to the host

  // Add the filter to the socket
  if( rt_dev_setsockopt(canfd, 
			SOL_CAN_RAW, 
			CAN_RAW_FILTER, 
			filters, 
			3*sizeof(struct can_filter)) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror( "Couldn't set the socket filters: " );
    return true;
  }

  // Bind the socket to the local address
  memset(&addr, 0, sizeof(addr));     // clear the address
  addr.can_ifindex = ifr.ifr_ifindex; // ifr_ifindex was set from SIOCGIFINDEX
  addr.can_family = AF_CAN;           // Address Family CAN

  if( rt_dev_bind(canfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_can))){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror( "Couldn't bind the socket: " );
    return true;
  }

  // set the baud rate
  can_baudrate_t* can_baudrate = (can_baudrate_t *)&ifr.ifr_ifru;
  *can_baudrate = rate;
  if( rt_dev_ioctl( canfd, SIOCSCANBAUDRATE, &ifr ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror( "Couldn't set the rate: " );
    return true;
  }

  /*
  can_ctrlmode_t* can_ctrlmode = (can_ctrlmode_t *)&ifr.ifr_ifru;
  *can_ctrlmode = CAN_CTRLMODE_LISTENONLY; // is this correct?
  if( rt_dev_ioctl(canfd, SIOCSCANCTRLMODE, &ifr) ){
    perror("devRTSocketCAN::open: Couldn't set the control mode: ");
    return FAILURE;
  }
  */

  // Set the mode 
  CAN_MODE* mode = (CAN_MODE*)&ifr.ifr_ifru;
  *mode = CAN_MODE_START;
  if( rt_dev_ioctl(canfd, SIOCSCANMODE, &ifr) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror( "Couldn't set the operation mode: " );
    return true;
  }

  /*
  nanosecs_rel_t timeout = 50000000;
  if (rt_dev_ioctl(canfd, RTCAN_RTIOC_SND_TIMEOUT, &timeout) ){
    perror("devRTSocketCAN::open: Couldn't set the send timeout: ");
    return FAILURE;
  }

  if( rt_dev_ioctl(canfd, RTCAN_RTIOC_RCV_TIMEOUT, &timeout) ){
    perror("devRTSocketCAN::open: Couldn't set the recv timeout: ");
    return FAILURE;
  }
  */

  return false;
}

bool devRTSocketCAN::Close(){
  // close the socket
  if( rt_dev_close( canfd ) ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror("devRTSocketCAN::close: Couldn't close the socket: ");
    return true;
  }
  return false;
}

// Send a can frame
// Note that block is useless for Socket CAN
bool devRTSocketCAN::Send( const devCANFrame& canframe, bool block ){

  // copy the data in to a RT Socket CAN frame
  struct can_frame frame;
  frame.can_id = canframe.Id();
  frame.can_dlc = canframe.Length();
  
  const unsigned char* framedata = canframe.Data();
  for(size_t i=0; i<8; i++)
    frame.data[i] = framedata[i];

  // send the frame
  int error = rt_dev_sendto( canfd, 
			     (void*)&frame, 
			     sizeof(can_frame_t), 
			     0,
			     (struct sockaddr *)&addr, 
			     sizeof(addr) );

  if( error < 0 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror("Failed to send frame: ");
    return true;
  }
  
  return false;
}

// Receive a CAN frame
bool devRTSocketCAN::Recv( devCANFrame& canframe, bool block ){

  struct can_frame frame;            // the RT Socket CAN frame
  struct sockaddr_can addr;          // the source address
  socklen_t addrlen = sizeof(addr);  // the size of the source address

  memset(&frame, 0, sizeof(frame));  // clear the frame
  int error =  rt_dev_recvfrom( canfd, 
				(void*)&frame, 
				sizeof(can_frame_t), 
				0,
				(struct sockaddr*)&addr, 
				&addrlen );

  if( error < 0 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << std::endl;
    perror("Failed to receive the frame: ");
    return true;
  }

  // create a devCANFrame
  canframe = devCANFrame( frame.can_id, frame.data, frame.can_dlc );

  return false;
}

#endif
