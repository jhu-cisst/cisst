/*
  Copyright 2006 Simon Leonard

  This file is part of openwam.

  openman is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 3 of the License, or (at your
  option) any later version.

  openman is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _devWAM_hpp
#define _devWAM_hpp

#include <cisstDevices/can/devCAN.hpp>
#include <cisstDevices/wam/devPuck.hpp>
#include <cisstDevices/wam/devGroup.hpp>
#include <cisstVector/vctDynamicVector.h>

class devWAM {
    
private:

  //! A pointer to a CAN bus device
  devCAN* canbus;
  
  //! CAN ID of the safety module
  /**
     The puck ID for the safety module
  */
  static const devPuckID SAFETY_NID = 10;
  
  //! A vector of all the pucks
  std::vector<devPuck>        pucks;
  //! A vector of all the groups
  std::vector<devGroup>      groups;
  //! The safety module
  devSafetyModule      safetymodule;
  
  //! Pack motor currents into a CAN frame.
  /**
     Motor currents are sent by group ID. One group represents the 4 pucks of 
     the upper arm. A second group represents the 3 pucks of the forearm.
     \param canframe[out] The canframe containing the motor currents
     \param gid The group ID. This must be either devGroup::UPPERARM or 
                devGroup::FOREARM
     \param currents A 4 array with the motor currents
     \return false if no error occurred. true otherwise.
   */
  bool PackCurrents( devCANFrame& canframe,
		     devGroupID gid,
		     const double currents[4] );

public:
  //! Default constructor
  /**
     Initiallize the WAM. This configures the pucks, groups and the safety 
     module.
     \param canbus The canbus device used for the WAM
     \param N The number of pucks on the bus
  */
  devWAM( devCAN* canbus, size_t N=7 );
  
  //! Receive motor positions
  /**
     This sends a broadcast position query to the pucks and process the replies.
     \param q[out] The resulting motor positions in radians
     \return false if no error occurred. true otherwise.
  */
  bool RecvPositions( vctDynamicVector<double>& q );

  //! Send motor positions
  /**
     This sets the motor positions. This is mostly used to "zero" the WAM to its
     initial position. For this, the safety module must be turned off and then
     a position command is sent to each puck. Finally, the safety module is 
     turned back on. Note that this does not "move" the robot, it only tells 
     each encoder its absolute position.
     \param q[in] The motor positions in radians
     \return false if no error occurred. true otherwise.
   */
  bool SendPositions( const vctDynamicVector<double>& q );

  //! Send motor currents
  /**
     Sends currents to the motor. This packs the currents in two CAN message.
     The first message is for the upper arm group and the second message is for
     the forearm group (if any).
     \param i[in] The motor currents
     \return false if no error occurred. true otherwise
  */
  bool SendCurrents( const vctDynamicVector<double>& i );
  
};

#endif

























