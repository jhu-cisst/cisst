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


#ifndef _mtsCAN_h
#define _mtsCAN_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstCAN/cisstCAN.h>
#include <cisstCAN/mtsCANFrame.h>
#include <cisstCAN/cisstCANExport.h>

//! MTS interfaces for a cisstCAN device
/**
   This class implements 2 MTS interfaces that can be attached to any cisstCAN
   device. The first interface is for IO (read/write) and the second interface
   (CTL) is for controlling the device. The IO interface provides two methods
   for reading/writing to the CAN device. The CTL interface provides Open/Close
   methods. Both methods use the mtsCANFrame type. This class is not intended to
   be used by itself but to be "attached" to a cisstCAN device (i.e. multiple 
   inheritance).
*/
class CISST_EXPORT mtsCAN : public mtsComponent{
  
  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR );

 private:

  //! The cisstCAN device
  /**
     This pointer must be set to a valid cisstCAN object. Once configured, the
     device can be used through MTS.
  */
  cisstCAN* can;

  //! Open the CAN device
  void mtsOpen();

  //! Close the CAN device
  void mtsClose();

  //! Read a CAN frame from the device
  void mtsRead( mtsCANFrame &frame ) const;

  //! Write a CAN frame to the device
  void mtsWrite( const mtsCANFrame &frame );

 protected:

  //! The input/output interface
  mtsInterfaceProvided* io;

  //! The control interface
  mtsInterfaceProvided* ctl;

 public:
  
  // Main constructor
  /**
     Creates the MTS interfaces for the CAN device and add read/write commands.
     \param componentname The MTS name of the component
     \param can The CAN device to attach to the MTS interfaces
  */
  mtsCAN( const std::string& componentname, cisstCAN* can );

};

CMN_DECLARE_SERVICES_INSTANTIATION( mtsCAN )

#endif
