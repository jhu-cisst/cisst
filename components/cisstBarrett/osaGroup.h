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

#ifndef _osaGroup_h
#define _osaGroup_h

#include "osaPuck.h"
#include <vector>

//! A logical group of pucks
/**
   Groups are used to communicate with pucks simultaneously. This has the 
   benifit of saving bandwidth. For example, all the pucks belong to the 
   BROADCAST group. Thus, when a CAN frame is destined to the BROADCAST group
   all the pucks will process the frame.
*/
class osaGroup {
public:

  //! The ID used to identify each group
  enum ID{

    //! The broadcast group
    /**
       The broadcast group contains all the pucks in a WAM (with the exception 
       of the safety module)
    */
    BROADCAST = 0x00,

    //! The upper arm group
    /**
       The upper arm group represents the 4 pucks of the upper arm 
       (shoulder+elbow)
    */
    UPPERARM = 0x01,

    //! The forearm group
    /**
       The forearm group represents the 3 pucks of the upper arm (wrist)
    */
    FOREARM = 0x02,

    //! The motor position group
    /**
       All pucks belong to the position group. Send a message to this group to 
       query all the motor positions. Each puck will reply to a message to this
       group with its motor position
    */
    POSITION = 0x03,

    //! Upper arm property group
    UPPERARM_POSITION = 0x04,

    //! Forearm property group
    FOREARM_POSITION = 0x05,

    //! Feedback property group
    PROPERTY = 0x06,

    HAND = 0x07,

    HAND_POSITION = 0x08,
    
    LASTGROUP = 0x09

  };

  //! Define the status of a group
  /**
     Barrett defines the following mode in which a puck can be
  */
  enum Status{ RESET=0, READY=2 };

  //! Error codes used by osaGroup
  enum Errno{ ESUCCESS, EFAILURE };
  
private:

  std::string LogPrefix();

  //! The ID of the pucks in the group
  std::vector< osaPuck > pucks;

  //! The CAN bus that is connected to the group
  cisstCAN* canbus; 
  
  //! The ID of the group
  osaGroup::ID id;

  //! Is the data contain a set property command
  /**
     Pucks have properties that can be read/write. To read/write a property, 
     you must send a CAN frame with the proper data format. To write a property,
     the CAN data must have a "write" bit set while a read command must have the
     "write" bit cleared. This method returns true if the "write" bit is set.
     \param canframe A CAN frame with a read/write command
     \return true if the command is a write. false if the command is a read
  */
  static bool IsSetFrame( const cisstCANFrame& canframe );
  
  //! pack a CAN frame
  /**
     Build a CAN frame destined to the group of pucks with the data formated.
     \param canframe[out] The resulting CAN frame
     \param propid The property ID to set or get
     \param propval The property value if the property must be set
     \param set True of the property must be set. False for a query
     \return false if no error occurred. true otherwise
  */
  osaGroup::Errno PackProperty( cisstCANFrame& canframe,
				  Barrett::Command command,
				  Barrett::ID propid,
				  Barrett::Value propval = 0 );
  
  //! The bit of a CAN ID that identicates a group
  static const cisstCANFrame::ID GROUP_CODE = 0x0400;


  osaGroup::Errno PackCurrents( cisstCANFrame& frame, 
				const vctFixedSizeVector<double,4>& I );

  //! Querry the group. This is only valid for querying
  //  positions on group 3
  osaGroup::Errno GetProperty( Barrett::ID id, 
			       std::vector<Barrett::Value>& values );
  
  //! Set the property of a group
  osaGroup::Errno SetProperty( Barrett::ID id, 
			       Barrett::Value value,
			       bool verify );
  
public:

  //! Create a group with an ID and a CAN device
  /**
     Initialize the group to the given ID and give the CAN device connected to
     the pucks.
     \param groupid The ID of the puck
     \param can The CAN device used to communicate with the pucks
  */
  osaGroup( osaGroup::ID id, cisstCAN* canbus );
  
  //! Convert a group ID to a CAN id
  /**
     Convert the ID of a group to a CAN ID used in a CAN frame. This assumes 
     that the origin of the CAN ID will be the host (00000)
  */
  static cisstCANFrame::ID CANID( osaGroup::ID groupid );
  
  //! Return the group ID
  osaGroup::ID GetID() const;

  //! Return the origin ID of the CAN id
  static osaGroup::ID OriginID( cisstCANFrame::ID canid );
  
  //! Return the origin ID of the CAN frame
  static osaGroup::ID OriginID( const cisstCANFrame& canframe );
  
  //! Return the destination ID of the CAN id
  static osaGroup::ID DestinationID( cisstCANFrame::ID canid );
  
  //! Return the destination ID of the CAN id
  static osaGroup::ID DestinationID( const cisstCANFrame& canframe );
  
  //! Return true if the CAN frame id's destination is a group (any group)
  static bool IsDestinationAGroup( const cisstCANFrame canframe );
  
  //! Add the puck ID to the group
  void AddPuckToGroup( osaPuck::ID pid );

  bool Clear() const { return pucks.empty(); }

  //! Return the puck ID of the first member
  osaPuck First() const { return pucks.front(); }
  osaPuck Last()  const { return pucks.back(); }

  bool IsEmpty() const { return pucks.empty(); }


  osaGroup::Errno Initialize();

  osaGroup::Errno Reset();

  osaGroup::Errno Ready();

  osaGroup::Errno GetStatus( std::vector<Barrett::Value>& status );


  osaGroup::Errno GetPositions( vctDynamicVector<double>& q );
  osaGroup::Errno SetTorques( const vctFixedSizeVector<double,4>& tau );

  osaGroup::Errno SetMode( Barrett::Value mode );

  

};

// Increment operator for pucks id
osaGroup::ID operator++( osaGroup::ID& gid, int i );

#endif
