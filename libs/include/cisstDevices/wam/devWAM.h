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

#ifndef _devWAM_h
#define _devWAM_h

#include <cisstDevices/can/devCAN.h>
#include <cisstDevices/wam/devPuck.h>
#include <cisstDevices/wam/devGroup.h>

#include <cisstVector/vctDynamicMatrix.h>

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsVector.h>
#include <cisstDevices/devExport.h>

//! A clas for a WAM device
/**
   Whole Arm Manipulator (WAM) is a 4-7DOF robot manufactured by Barrett Tech. 
   From a device perspective the WAM can be interfaced from either an ethernet
   device (wired or wireless) or a CAN device. On the WAM, the ethernet device
   is connected to a PC104. Thus, when using ethernet you can communicate with
   the onboard computer through a shell or client/server. The downside of this 
   is that the PC104 does all the (CAN) communication with the motors and 
   much of the computation as well. The PC104 can be bypassed and the motor can 
   be controlled by an external PC (see the documentation on how to configure 
   the switches). For this, the external PC must have a CAN device and the 
   communication with the hardware must be established and maintained. The 
   devWAM class implements a WAM that can be controlled from an external PC.
   Thus it manages the CAN bus, the pucks present on the CAN bus and the WAM's
   safety module. Operations are pretty basic: set motors torques, set 
   motor positions and get motor positions.   
*/
class CISST_EXPORT devWAM : public mtsTaskPeriodic {

  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION , CMN_LOG_LOD_RUN_DEBUG );

private:
  int firstruns;

  //
  // MTS specific members
  //

  //! MTS vector of joint positions
  mtsVector<double> jointspositions;

  //! MTS function used to querry joints torques
  mtsFunctionRead ReadJointsTorques;

  //
  // WAM specific members
  //

  //! A pointer to a CAN device
  devCAN* candev;
  
  //! CAN ID of the safety module
  /**
     The puck ID for the safety module
  */
  static const devPuckID SAFETYMODULE_PID = 10;
  
  //! A vector of all the pucks
  std::vector<devPuck>   pucks;
 
 //! A vector of all the groups
  std::vector<devGroup> groups;

  //! The safety module
  devSafetyModule safetymodule;

  //! Pack motor currents into a CAN frame.
  /**
     Motor currents are sent by group ID. One group represents the 4 pucks of 
     the upper arm. A second group represents the 3 pucks of the forearm.
     \param canframe[out] The canframe containing the motor currents
     \param gid The group ID. This must be either devGroup::UPPERARM or 
     devGroup::FOREARM
     \param t A 4 array with the motor currents. The currents must be
              ordered by puck id. If gid is set to devGroup::FOREARM, 
	      then only the first 3 currents will be used.
     \return false if no error occurred. true otherwise.
  */
  bool PackCurrents( devCANFrame& canframe, devGroupID gid, const double I[4] );

  //! Matrix used to convert motors positions to joints positions
  /**
     When given a vector of motors positions, this matrix us used to transform
     the motors positions to joints positions. For the WAM, this is essentially
     a block diagonal matrix.
     \sa MotorsPos2JointsPos
  */
  vctDynamicMatrix<double> mpos2jpos;

  //! Matrix used to convert joints positions to motors positions
  /**
     When given a vector of joints positions, this matrix us used to transform
     the joints positions to motors positions. For the WAM, this is essentially 
     a block diagonal matrix.
     \sa JointsPos2MotorsPos
  */
  vctDynamicMatrix<double> jpos2mpos;

  //! Matrix used to convert joints torques to motors torques
  /**
     When given a vector of joints torques, this matrix us used to transform
     the joints torques to motors torques. For the WAM, this is essentially  
     a block diagonal matrix.
     \sa JointsTrq2MotorsTrq
  */
  vctDynamicMatrix<double> jtrq2mtrq;

  //! Convert motor positions to joints positions
  /**
     Converts the motor angles received from the pucks to joint angles.
     This only perform a matrix/vector multiplication
     \param q A vector of motor angles
     \return A vector of joint angles
     \sa mpos2jpos
  */
  vctDynamicVector<double> 
  MotorsPos2JointsPos( const vctDynamicVector<double>& q );

  //! Convert joints positions to motors positions
  /**
     Converts joints angles to motors angles that can be sent to the pucks.
     This only perform a matrix/vector multiplication
     \param q A vector of joints angles
     \return A vector of motors angles
     \sa jpos2mpos
  */
  vctDynamicVector<double> 
  JointsPos2MotorsPos( const vctDynamicVector<double>& q );

  //! Convert joints torques to motors torques
  /**
     Converts joints torques to motors torques that can be sent to the pucks.
     This only perform a matrix/vector multiplication
     \param q A vector of motor angles
     \return A vector of joint angles
     \sa jtrq2mtrq
  */
  vctDynamicVector<double> 
  JointsTrq2MotorsTrq( const vctDynamicVector<double>& t );

  //! Receive joints positions
  /**
     This broadcast a position query to the pucks and process all their replies.
     First, the replies (in encoder ticks) are converted to motors positions and
     then to joint angles.
     \param q[out] The resulting motor positions in radians
     \return false if no error occurred. true otherwise.
  */
  bool RecvPositions( vctDynamicVector<double>& q );

  //! Send joints positions
  /**
     The WAM has relative encoders, which means that you must "zero" the 
     encoders each time the WAM is powered. For this you manually move the WAM
     to a know configuration and then call SendPosition. First, the safety 
     module is turned off to avoid triggering a velocity fault. Then, the joints
     positions are converted to encoder counts and each position is overwritten 
     by "setting" the position on each puck. Finally, the safety module is 
     turned back on. Note that this does not "move" the robot, it only tells 
     each puck the absolute position of its encoder.
     \param q[in] The motor positions in radians
     \return false if no error occurred. true otherwise.
   */
  void SendPositions( const mtsVector<double>& q );

  //! Send joints torques
  /**
     Set the torques of each joint. First this converts the joints torques to 
     motors torques. Then it packs the torques in two CAN frames. The first 
     frame is addressed the upper arm group and the second frame is addressed 
     to the forearm group (if present).
     \param i[in] The motor torques
     \return false if no error occurred. true otherwise
  */
  void SendTorques( const mtsVector<double>& t );
  
public:

  //! Default constructor
  /**
     Initiallize the WAM. This configures the pucks, groups and the safety 
     module. You can use devWAM with any CAN device, has long as it is derived
     from the devCAN base class.
     \param taskname The task name
     \param period The task period
     \param candev The CAN device used by the WAM. This device must be derived
                   from the devCAN classe.
     \param N The number of pucks on the bus (not counting the safety module). 
              Default value is 7.
     \sa devCAN
  */
  devWAM( const std::string& taskname, 
	  double period, 
	  devCAN* candev, 
	  size_t N=7 );

  ~devWAM(){}

  void Configure( const std::string& filename = "" );
  void Startup();
  void Cleanup();
  void Run();

  static const std::string PositionInterfaceName;
  static const std::string ReadPositionsCommandName;
  static const std::string WritePositionsCommandName;

  static const std::string TorqueInterfaceName;
  static const std::string ReadTorquesCommandName;

};

CMN_DECLARE_SERVICES_INSTANTIATION( devWAM );

#endif

























