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

#include <cisstDevices/can/devCAN.hpp>
#include <cisstDevices/wam/devProperties.hpp>

#ifndef _devPuck_hpp
#define _devPuck_hpp

//! Define the ID of a puck
/**
   The ID of a puck is at most 5 bits. Hence we use 8 bits of which only the 5
   LSB are used.
*/
typedef unsigned char devPuckID;

//! Define the modes of a puck
/**
   Barrett defines the following mode in which a puck can be
*/
struct devPuckMode{enum{ IDLE=0, TORQUE=2, PID=3, VELOCITY=4, TRAPEZOIDAL=5};};

//! Define the status of a puck
/**
   Barrett defines the following status for a puck
*/
struct devPuckStatus{enum{ RESET=0, READY=2 };};

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
private:
  
  //! The CAN device connected to the puck.
  /**
     This is a bit counter-intuitive. This is not the CAN device "on" the puck
     (each puck has a CAN device). Rather, this is the can device used to 
     communicate "with" the puck.
  */
  devCAN*    canbus;

  //! The ID of the puck
  devPuckID  puckid;

  //! Number of cunts per revolution of the encoder
  devPropertyValue cntprev;
  //! Amps per Newton meter constant of the motor
  devPropertyValue    ipnm;
  //! The index of the puck within its group
  devPropertyValue  grpidx;
  //! The maximum current of the motor
  devPropertyValue  imax;
  
  //! The encoder position
  devPropertyValue   encpos;
  //! The motor current
  devPropertyValue mcurrent;
  
  //! The number of properties available
  static const size_t NUM_PROPERTIES = 128;

  //! Convert a puck ID to a CAN id (assume origin from host 00000)
  /**
     Convert the ID of a puck to a CAN ID used in a CAN frame
  */
  static devCANID CANId( devPuckID puckid );

  //! Is the data contain a set property command
  /**
     Pucks have properties that can be read/write. To read/write a property, 
     you must send a CAN frame with the proper data format. To write a property,
     the CAN data must have a "write" bit set while a read command must have the
     "write" bit cleared. This method returns true if the "write" bit is set.
     \param canframe A CAN frame with a read/write command
     \return true if the command is a write. false if the command is a read
  */
  static bool IsSetCommand( const devCANFrame& canframe );
  
  //! Return true if the property is valid
  /**
     Call this method to determine if the property ID is valid
     \param propid The property ID
     \return true if the property ID is valid. false otherwise
  */
  bool IsValid( devPropertyID propid ) const;
  
  //! not implemented yet
  bool IsValid( devPropertyID propid, devPropertyValue propval ) const;
  
public:
  
  //! Default constructor
  devPuck(){}

  //! Create a puck with an ID and a CAN device
  /**
     Initialize the puck to the given ID and give the CAN device connected to 
     the puck.
     \param puckid The ID of the puck
     \param can The CAN device used to communicate with the puck
  */
  devPuck( devPuckID puckid, devCAN* can );

  //! Return the origin ID of the CAN id
  /**
     Each CAN ID consists of 11 bits. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the origin puck ID in a CAN ID.
     \param canid The CAN id
     \return The origin puck ID of the CAN ID
     \sa Origin( devCANFrame )
  */
  static devPuckID Origin( devCANID canid );

  //! Return the destination ID of the CAN id
  /**
     Each CAN ID consists of 11 bits. For communicating with pucks, this
     ID is composed of 5 bits representing the ID of the puck at the origin 
     of the CAN frame and 5 bits representing the ID of the destination puck.
     Call this method to obtain the destination puck ID in a CAN ID.
     \param canid The CAN id
     \return The origin puck ID of the CAN ID
     \sa Origin( devCANFrame )
  */
  static devPuckID Destination( devCANID canid );

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
  static devPuckID Origin( const devCANFrame& canframe );

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
  static devPuckID Destination( const devCANFrame& canframe );

  //! Return the index of the puck within its group
  /**
     Each puck on the WAM belongs to a group. The upper arm group contains 
     represents 4 pucks and the forearm group represents 3 pucks. Since these
     groups are ordered, the index (0, 1, 2, 3) of a puck within its group 
     matters. Call this method if you want to find the (zero) index of the pucks
     within its group.
     \return The zero index of the puck within its group
  */
  devPropertyValue Index() const ;

  //! Return the motor Amp/Nm constant
  devPropertyValue IpNm() const ;

  //! Return the encoder counts/revolution constant
  devPropertyValue CountsPerRevolution() const ;

  //! Perform the initial configuration
  /**
     This sets the puck in the "ready" status and in "idle" mode. It also 
     queries the puck for its group index, I/Nm constant and counts/rev 
     constant.
     \param false if no error occurred. true otherwise
   */
  bool Configure();

  //! Query the puck for a property ID 
  /**
     This method queries the puck for the value of a property.
     \param propid The ID of the property to query
     \return The value of the property
  */
  devPropertyValue GetProperty( devPropertyID propid );

  //! Set the puck property ID to a value
  /**
     This method sets the value of the puck's property.
     \param propid The ID of the property to set
     \param propval The value of the property
     \param verify Double check by querying the property of the puck
     \return false is no error occurred. true otherwise
  */
  bool SetProperty( devPropertyID propid, 
		    devPropertyValue propval, 
		    bool verify=true);

  //! pack a CAN frame
  /**
     Build a CAN frame destined to the puck with the data formated.
     \param canframe[out] The resulting CAN frame
     \param propid The property ID to set or get
     \param propval The property value if the property must be set
     \param set True of the property must be set. False for a query
     \return false if no error occurred. true otherwise
  */
  bool PackProperty( devCANFrame& canframe,
		     devPropertyID propid,
		     devPropertyValue propval=0, 
		     bool set=false );

  //! unpack a CAN frame
  /**
     Extract the property ID and a property value of a CAN frame.
     \param canframe[in] The CAN frame to process
     \param propid[out] The ID of the property in the data
     \param propval[out] The value of the property in the data
   */
  bool UnpackCANFrame( const devCANFrame& canframe, 
		       devPropertyID& propid, 
		       devPropertyValue& propval );
  
}; 

//! Define the safety module
/**
   The safety module is the hardware that monitors the activity on the CAN bus.
   It contains a puck that processes the CAN frames and is able to shut down the
   WAM if a fault occurs.
*/
typedef devPuck devSafetyModule;

#endif
