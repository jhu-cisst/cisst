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

#ifndef _mtsPuck_h
#define _mtsPuck_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstBarrett/osaPuck.h>
#include <cisstBarrett/cisstBarrettExport.h>

class CISST_EXPORT mtsPuck : 
  public mtsComponent,
  public osaPuck{

  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR );

 private:

  //! Reset the firmware
  void mtsReset();

  //! Set the status to ready
  void mtsReady();

  void mtsInitialize();

  //! Read a CAN frame from the device
  void mtsReadPosition( mtsLongLong& position ) const;

  //! Write a CAN frame to the device
  void mtsWriteTorque( const mtsLongLong& torque );

 protected:

  //! The input/output interface
  mtsInterfaceProvided* io;

  //! The control interface
  mtsInterfaceProvided* ctl;

 public:

  mtsPuck( const std::string& componentname, 
	   osaPuck::ID id,
	   cisstCAN* can );

};

CMN_DECLARE_SERVICES_INSTANTIATION( mtsPuck )

#endif
