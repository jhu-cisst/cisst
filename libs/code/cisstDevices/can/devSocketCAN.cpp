#include <iomanip>
#include "socketcan.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <linux/can.h>
#include <linux/if.h>
#include <sys/ioctl.h>

using namespace std;

openman::SocketCAN::SocketCAN( const std::string& devname, 
				   uint32_t baudrate ) : CANbus( baudrate ){
  this->devname = devname;
}

openman::Retval openman::SocketCAN::open(){
  struct  ifreq ifr;

  // create a socket
  canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if( canfd < 0 ){
    perror("SocketCAN::open: Couldn't open the socket: ");
    return openman::FAILURE;
  }

  // Get CAN interface index by name
  strncpy(ifr.ifr_name, devname.data(), IFNAMSIZ);
  if( ioctl(canfd, SIOCGIFINDEX, &ifr) ){
    perror("SocketCAN::open: Couldn't get the interface index: ");
    return openman::FAILURE;
  }

  // set the baud rate
  can_baudrate_t* can_baudrate = (can_baudrate_t *)&ifr.ifr_ifru;
  *can_baudrate = baudrate;
  if( ioctl(canfd, SIOCSCANBAUDRATE, &ifr) ){
    perror("SocketCAN::open: Couldn't set the baudrate: ");
    return openman::FAILURE;
  }

  /*
  can_ctrlmode_t* can_ctrlmode = (can_ctrlmode_t *)&ifr.ifr_ifru;
  *can_ctrlmode = CAN_CTRLMODE_LISTENONLY; // is this correct?
  if( rt_dev_ioctl(canfd, SIOCSCANCTRLMODE, &ifr) ){
    perror("SocketCAN::open: Couldn't set the control mode: ");
    return openman::FAILURE;
  }
  */

  can_mode_t* mode = (can_mode_t *)&ifr.ifr_ifru;
  *mode = CAN_MODE_STA;
  if( ioctl(canfd, SIOCSCANMODE, &ifr) ){
    perror("SocketCAN::open: Couldn't set the mode: ");
    return openman::FAILURE;
  }

  memset(&to_addr, 0, sizeof(to_addr));  // clear the address
  to_addr.can_ifindex = ifr.ifr_ifindex; // ifr_ifindex was set from SIOCGIFINDEX
  to_addr.can_family = AF_CAN;           // CAN address family
  /*
  nanosecs_rel_t timeout = 50000000;
  if (rt_dev_ioctl(canfd, CAN_IOC_SND_TIMEOUT, &timeout) ){
    perror("SocketCAN::open: Couldn't set the send timeout: ");
    return openman::FAILURE;
  }

  if( rt_dev_ioctl(canfd, CAN_IOC_RCV_TIMEOUT, &timeout) ){
    perror("SocketCAN::open: Couldn't set the recv timeout: ");
    return openman::FAILURE;
  }
  */
  return openman::SUCCESS;
}

openman::Retval openman::SocketCAN::close(){
  if( close(canfd) ){
    perror("SocketCAN::close: Couldn't close the socket: ");
    return openman::FAILURE;
  }
  return openman::SUCCESS;
}

openman::Retval openman::SocketCAN::send(void* data,size_t nbytes, bool block){
  openman::CANFrame* canframe = (openman::CANFrame*)data;

  struct can_frame frame;
  frame.can_id = canframe->id();
  frame.can_dlc = canframe->length();
  
  const uint8_t* framedata = canframe->data();
  for(size_t i=0; i<8; i++){
    //cout << "0x" << hex << setw(2) << setfill('0') << (int)framedata[i] << " ";
    frame.data[i] = framedata[i];
  }
  //cout << endl;

  mutex.lock();
  int error = sendto(canfd, (void*)&frame, sizeof(can_frame_t), 0,
		     (struct sockaddr *)&to_addr, sizeof(to_addr));
  if( error < 0 ){
    perror("SocketCAN::send: Failed to send frame: ");
    cout << error << endl;
    mutex.unlock();
    return openman::FAILURE;
  }
  
  mutex.unlock();
  return openman::SUCCESS;
}

openman::Retval openman::SocketCAN::recv(void* data, size_t& nbytes, bool block){
  struct can_frame frame;            // the can frame
  struct sockaddr_can addr;          // the source address
  socklen_t addrlen = sizeof(addr);  // the size of the source address

  mutex.lock();
  int error =  recvfrom(canfd, (void*)&frame, sizeof(can_frame_t), 0,
			(struct sockaddr *)&addr, &addrlen);
  if( error < 0 ){
    perror("SocketCAN::recv: failed to receive the frame: ");
    //cout << error << endl;
    mutex.unlock();
    nbytes = 0;
    return openman::FAILURE;
  }
  mutex.unlock();

  openman::CANFrame* canframe = (openman::CANFrame*)data;
  *canframe = openman::CANFrame( frame.can_id, frame.data, frame.can_dlc );
  nbytes = sizeof(openman::CANFrame);

  return openman::SUCCESS;
}

