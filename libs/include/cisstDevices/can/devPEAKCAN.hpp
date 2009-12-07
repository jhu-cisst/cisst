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

#ifdef CISST_HAS_PEAKCAN

#ifndef _devPEAKCAN_HPP
#define _devPEAKCAN_HPP

#include <cisstDevices/can/devCAN.hpp>
#include <libpcan.h>

//! A PEAK System CAN device
/**
   This implements a CAN device manufactured by PEAK System. PEAK System 
   CAN hardware are compatible with several drivers and API including SocketCAN
   RTSocketCAN, CAN Festival. This class uses the API from PEAK System. Since
   PEAK System claims that their API works with all their harware, this class
   should work with all PEAK System devices.
*/

class devPEAKCAN : public devCAN {
    
private:
  
  //! The CAN handle
  HANDLE handle;    // nt-handle
  
  //! not so important stuff
  static const long TXQUEUESIZE = 32;  // nr of entries in message queue*
  static const long RXQUEUESIZE = 32;  // nr of entries in message queue
  static const long TXTIMEOUT   = 50;  // tx-timeout in miliseconds 
  static const long RXTIMEOUT   = 50;  // rx-timeout in miliseconds 
  
public:

  //! Constructor
  /**
     Initialize the device and the rate of the CAN device
     \param rate The CAN rate (devCAN_150, devCAN_300, devCAN_1000)
  */
  devPEAKCAN( devCANRate rate );
  
  //! Open and configure the CAN socket
  /**
     This method creates and configure a PEAK CAN device.
  */
  bool Open();

  //! Close the CAN device
  bool Close();
  
  //! Send a CAN frame on the bus
  /**
     Call this method to send a CAN frame on the bus. The method can be blocking
     \param frame[in] The CAN frame to send on the bus
     \param block Block the device until the operation is completed
  */
  bool Send( const devCANFrame& canframe, bool block=false );

  //! Send a CAN frame on the bus
  /**
     Call this method to send a CAN frame on the bus. The method can be blocking
     \param frame[in] The CAN frame to send on the bus
     \param block Block the device until the operation is completed
  */
  bool Recv( devCANFrame& canframe, bool block=false );
  
};

#endif // _devPEAKCAN_h

#endif // CISST_HAS_PEAKCAN
