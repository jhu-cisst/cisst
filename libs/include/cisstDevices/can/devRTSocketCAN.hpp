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

#ifndef _devRTSocketCAN_hpp
#define _devRTSocketCAN_hpp

#include <cisstDevices/can/devCAN.hpp>
#include <rtdm/rtcan.h> // Defines for the RT CAN socket

#include <string>       // for device name (can0, can1,...)

//! A Real Time Socket CAN device
/**
   Most harware in Linux use the /dev interface. Typically, your device (usb,
   serial port, ...) will be represented by a file in /dev. CAN hardware is no
   exception except for the socket CAN module. SocketCAN implements a CAN stack
   and extend the BSD sockets just like you have a ethernet stack and sockets. 
   RT SocketCAN is the "real time" version for the Xenomai framework.
   To use devRTSocketCAN, you must have a Xenomai patched Linux kernel with 
   the RT CAN module.
*/

class devRTSocketCAN : public devCAN {

private:

  //! The name of the CAN device (can0, can1, ...)
  std::string devicename;

  //! The file descriptor of the socket
  int canfd;

  //! The socket address for the CAN address family
  struct sockaddr_can addr;

  //! CAN filter. These are way too "WAMish" 
  struct can_filter filters[3];
  
public:

  //! Constructor
  /**
     Initialize the device name and the rate of the CAN device
     \param devicename The name of the device (can0, can1, ...)
     \param rate The CAN rate (devCAN_150, devCAN_300, devCAN_1000)
  */
  devRTSocketCAN( const std::string& devicename, devCANRate rate );

  //! Open and configure the CAN socket
  /**
     This method creates and configure a RT CAN socket.
  */
  bool Open();

  //! Close the CAN socket
  bool Close();

  //! Send a CAN frame on the bus
  /**
     Call this method to send a CAN frame on the bus.
     \param frame[in] The CAN frame to send on the bus
     \param block Block the device until the operation is completed. This
                  parameter is irrelevant for devRTSocketCAN.
  */
  bool Send( const devCANFrame& frame, bool block=false );

  //! Receive a CAN frame
  /**
     Call this method to receive a CAN frame.
     \param frame[out] The CAN frame received from the bus
     \param block Block the device until a CAN frame is received. This
                  parameter is irrelevant for devRTSocketCAN.
  */
  bool Recv( devCANFrame& frame, bool block=false );
  
};

#endif // _devRTSocketCAN_hpp
#endif // XENOMAI
