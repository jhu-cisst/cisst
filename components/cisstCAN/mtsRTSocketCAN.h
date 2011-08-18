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


#ifndef _mtsRTSocketCAN_h
#define _mtsRTSocketCAN_h

#include <cisstCAN/mtsCAN.h>
#include <cisstCAN/osaRTSocketCAN.h>
#include <cisstCAN/cisstCANExport.h>

//! A RTSocketCAN device wrapped with MTS interfaces
/**
   This class combines the MTS CAN interfaces to osaRTSocketCAN. It simply 
   derives from both osaRTSocketCAN and mtsCAN
*/
class CISST_EXPORT mtsRTSocketCAN : 
  public mtsCAN,                 // This adds the MTS CAN interface
  public osaRTSocketCAN{         // This adds the osaRTSocketCAN code

 public:

  //! Main constructor
  /**
     \param cmpname MTS component name
     \param canname The name of the RT Socket CAN device (rtcan0, rtcan1, ... )
     \param rate    The CAN rate of the device
     \param loopback Specify if the device is operating in loopback mode
   */
  mtsRTSocketCAN( const std::string& cmpname,
		  const std::string& canname,
		  cisstCAN::Rate rate,
		  cisstCAN::Loopback loopback = cisstCAN::LOOPBACK_OFF ) : 
    mtsCAN( cmpname, this ),
    osaRTSocketCAN( canname, rate, loopback ){}
  
};

#endif
