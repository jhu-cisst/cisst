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

#ifndef _devGroup_hpp
#define _devGroup_hpp

#include <cisstDevices/wam/devPuck.hpp>
#include <vector>

//! Define the ID of a group (5 bits)
/**
   The ID of a group is at most 5 bits. Hence we use 8 bits of which only the 5
   LSB are used.
*/
typedef unsigned char devGroupID;

//! Define the status of a group
/**
   Barrett defines the following mode in which a puck can be
*/
struct devGroupStatus{enum{ RESET=0, READY=2 };};

//! A logical group of pucks
/**
   Groups are used to communicate with pucks simultaneously. This has the 
   benifit of saving bandwidth. For example, all the pucks belong to the 
   BROADCAST group. Thus, when a CAN frame is destined to the BROADCAST group
   all the pucks will process the frame.
*/
class devGroup {
  
private:

  //! The ID of the pucks in the group
  std::vector<devPuckID> pucksid;

  //! The CAN bus that is connected to the group
  devCAN* canbus; 
  
  //! The ID of the group
  devGroupID groupid;

  // The number of properties
  static const size_t NUM_PROPERTIES = 128;

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
  
  //! pack a CAN frame
  /**
     Build a CAN frame destined to the group of pucks with the data formated.
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
  
  //! The bit of a CAN ID that identicates a group
  static const devCANID GROUPTAG = 0x0400;
  
public:

  //! The broadcast group
  /**
     The broadcast group contains all the pucks in a WAM (with the exception of
     the safety module)
  */
  static const devGroupID BROADCAST    = 0x00; 
  //! The upper arm group
  /**
     The upper arm group represents the 4 pucks of the upper arm 
     (shoulder+elbow)
  */
  static const devGroupID UPPERARM     = 0x01;
  //! The forearm group
  /**
     The forearm group represents the 3 pucks of the upper arm (wrist)
  */
  static const devGroupID FOREARM      = 0x02;
  //! The motor position group
  /**
     All pucks belong to the position group. Send a message to this group to 
     query all the motor positions. Each puck will reply to a message to this
     group with its motor position
  */
  static const devGroupID POSITION     = 0x03;
  //! Upper arm property group
  static const devGroupID UPPERARMPROP = 0x04;
  //! Forearm property group
  static const devGroupID FOREARMPROP  = 0x05;
  //! Feedback property group
  static const devGroupID PROPFEEDBACK = 0x06;
  
  //! Create a group with an ID and a CAN device
  /**
     Initialize the group to the given ID and give the CAN device connected to
     the pucks.
     \param groupid The ID of the puck
     \param can The CAN device used to communicate with the pucks
  */
  devGroup( devGroupID groupid, devCAN* canbus );
  
  //! Convert a group ID to a CAN id
  /**
     Convert the ID of a group to a CAN ID used in a CAN frame. This assumes 
     that the origin of the CAN ID will be the host (00000)
  */
  static devCANID CANId( devGroupID puckid );
  
  //! Return true if the CAN id's destination is a group
  static bool IsForGroup( const devCANFrame canframe );
  
  //! Return the origin ID of the CAN id
  static devGroupID Origin( devCANID canid );
  
  //! Return the destination ID of the CAN id
  static devGroupID Destination( devCANID canid );
  
  //! Return the origin ID of the CAN frame
  static devGroupID Origin( const devCANFrame& canframe );
  
  //! Return the destination ID of the CAN id
  static devGroupID Destination( const devCANFrame& canframe );
  
  //! Add the puck ID to the group
  void AddPuckIDToGroup( devPuckID pid ) { pucksid.push_back(pid); }

  //! Return the puck ID of the first member
  devPuckID FirstMember() const { return pucksid.front(); }
  devPuckID LastMember() const { return pucksid.back(); }

  bool Empty() const { return pucksid.empty(); }

  //! Querry the group. This is only valid for querying
  //  positions on group 3
  bool GetProperty( devPropertyID propid );
  
  //! Set the property of a group
  bool SetProperty( devPropertyID propid, devPropertyValue propval);
  
};

#endif
