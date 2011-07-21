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

#include <cisstDevices/can/devCAN.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devProperties.h>

#ifndef _devPuck_h
#define _devPuck_h

//! Implements a Barrett puck
/**
   Pucks are the small gizmos that are connected to each motor. They essentially
   have all the hardware to control motors (micro-controller, amplifier,
   encoder and CAN device). Pucks are used to query encoders positions and set
   motors currents. A puck also has several "properties" (see the Barrett 
   documentation) to determine their behavior. Each puck is accesses on the CAN 
   bus by its node ID but several pucks can be addressed by group 
   (\sa devGroup).
*/
class devPuck {

public:

  //! Define the ID of a puck
  /**
     The ID of a puck is at most 5 bits. Hence we use 8 bits of which only the 5
     LSB are used.
  */
  enum ID { PUCK_ID1=1, 
	    PUCK_ID2=2, 
	    PUCK_ID3=3,
	    PUCK_ID4=4, 
	    PUCK_ID5=5, 
	    PUCK_ID6=6, 
	    PUCK_ID7=7,
	    SAFETY_MODULE_ID=10,
	    PUCK_IDF1=11,
	    PUCK_IDF2=12,
	    PUCK_IDF3=13,
	    PUCK_IDF4=14  };

  //! Define the modes of a puck
  /**
     Barrett defines the following mode in which a puck can be
  */
  static const devProperty::Value STATUS_RESET = 0;
  static const devProperty::Value STATUS_READY = 2;  

  //! Define the status of a puck
  /**
     Barrett defines the following status for a puck
  */
  static const devProperty::Value MODE_IDLE = 0;
  static const devProperty::Value MODE_TORQUE = 2;
  static const devProperty::Value MODE_POSITION = 3;
  static const devProperty::Value MODE_PID = 3;
  static const devProperty::Value MODE_VELOCITY = 4;
  static const devProperty::Value MODE_TRAPEZOIDAL = 5;

  //! Error values
  enum Errno{ ESUCCESS, EFAILURE };

private:
  
  //! The CAN device connected to the puck.
  /**
     This is a bit counter-intuitive. This is not the CAN device "on" the puck
     (each puck has a CAN device). Rather, this is the can device used to 
     communicate "with" the puck.
  */
  devCAN*    canbus;

  //! The ID of the puck
  devPuck::ID  id;

  //! Number of cunts per revolution of the encoder
  devProperty::Value cntprev;

  //! Amps per Newton meter constant of the motor
  devProperty::Value ipnm;

  //! The index of the puck within its group
  devProperty::Value grpidx;

  //! The maximum current of the motor
  devProperty::Value imax;
  
  //! The encoder position
  devProperty::Value encpos;

  //! The motor current
  devProperty::Value mcurrent;
  
  //! Convert a puck ID to a CAN id (assume origin from host 00000)
  /**
     Convert the ID of a puck to a CAN ID used in a CAN frame
  */
  static devCAN::Frame::ID CANID( devPuck::ID id );

  //! Does the data contain a set property command
  /**
     Pucks have properties that can be read/write. To read/write a property, 
     you must send a CAN frame with the proper data format. To write a property,
     the CAN data must have a "write" bit set while a read command must have the
     "write" bit cleared. This method returns true if the "write" bit is set.
     \param canframe A CAN frame with a read/write command
     \return true if the command is a write. false if the command is a read
  */
  static bool IsSetFrame( const devCAN::Frame& canframe );
  
  //! pack a CAN frame
  /**
     Build a CAN frame destined to the puck with the data formated.
     \param canframe[out] The resulting CAN frame
     \param propid The property ID to set or get
     \param propval The property value if the property must be set
     \param set True of the property must be set. False for a query
     \return false if no error occurred. true otherwise
  */
  devPuck::Errno PackProperty( devCAN::Frame& canframe,
			       devProperty::Command command,
			       devProperty::ID propid,
			       devProperty::Value propval = 0 );

public:
  
  //! Default constructor
  devPuck();

  //! Create a puck with an ID and a CAN device
  /**
     Initialize the puck to the given ID and give the CAN device connected to 
     the puck.
     \param puckid The ID of the puck
     \param can The CAN device used to communicate with the puck
  */
  devPuck( devPuck::ID id, devCAN* can );

  //! Return the puck ID
  devPuck::ID GetID() const;

  //! Return the origin ID of the CAN id
  /**
     Each CAN ID consists of 11 bits. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the origin puck ID in a CAN ID.
     \param canid The CAN id
     \return The origin puck ID of the CAN ID
     \sa Origin( devCAN::Frame )
  */
  static devPuck::ID OriginID( devCAN::Frame::ID id );

  //! Return the origin ID of the CAN frame
  /**
     Each CAN frame contains an 11 bits ID. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the origin puck ID in a CAN frame.
     \param canid The CAN frame
     \return The origin puck ID of the CAN frame
     \sa Origin( devCANID )
  */
  static devPuck::ID OriginID( const devCAN::Frame& canframe );

  //! Return the destination ID of the CAN id
  /**
     Each CAN ID consists of 11 bits. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the destination puck ID in a CAN ID.
     \param canid The CAN id
     \return The origin puck ID of the CAN ID
     \sa Origin( devCAN::Frame )
  */
  static devPuck::ID DestinationID( devCAN::Frame::ID id );

  //! Return the destination ID of the CAN frame
  /**
     Each CAN frame contains an 11 bits ID. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the destination puck ID in a CAN frame.
     \param canid The CAN frame
     \return The origin puck ID of the CAN frame
     \sa Destination( devCANID )
  */
  static devPuck::ID DestinationID( const devCAN::Frame& canframe );

  //! Return the index of the puck within its group
  /**
     Each puck on the WAM belongs to a group. The upper arm group contains 
     represents 4 pucks and the forearm group represents 3 pucks. Since these
     groups are ordered, the index (0, 1, 2, 3) of a puck within its group 
     matters. Call this method if you want to find the (zero) index of the pucks
     within its group.
     \return The zero index of the puck within its group
  */
  devProperty::Value GroupIndex() const ;

  //! Return the motor Amp/Nm constant
  devProperty::Value IpNm() const ;

  //! Return the encoder counts/revolution constant
  devProperty::Value CountsPerRevolution() const ;

  //! Perform the initial configuration
  /**
     This sets the puck in the "ready" status and in "idle" mode. It also 
     queries the puck for its group index, I/Nm constant and counts/rev 
     constant.
     \param false if no error occurred. true otherwise
   */
  devPuck::Errno Configure();

  //! Query the puck for a property ID 
  /**
     This method queries the puck for the value of a property.
     \param propid The ID of the property to query
     \return The value of the property
  */
  devPuck::Errno GetProperty( devProperty::ID id,
			      devProperty::Value& value );

  //! Set the puck property ID to a value
  /**
     This method sets the value of the puck's property.
     \param propid The ID of the property to set
     \param propval The value of the property
     \param verify Double check by querying the property of the puck
     \return false is no error occurred. true otherwise
  */
  devPuck::Errno SetProperty( devProperty::ID propid, 
			      devProperty::Value propval, 
			      bool verify );


  //! unpack a CAN frame
  /**
     Extract the property ID and a property value of a CAN frame.
     \param canframe[in] The CAN frame to process
     \param propid[out] The ID of the property in the data
     \param propval[out] The value of the property in the data
   */
  devPuck::Errno UnpackCANFrame( const devCAN::Frame& canframe, 
				 devProperty::ID& id, 
				 devProperty::Value& value );
  
}; 

// Increment operator for pucks id
devPuck::ID operator++( devPuck::ID& pid, int i );


//! Define the safety module
/**
   The safety module is the hardware that monitors the activity on the CAN bus.
   It contains a puck that processes the CAN frames and is able to shut down the
   WAM if a fault occurs.
*/
typedef devPuck devSafetyModule;

#endif
