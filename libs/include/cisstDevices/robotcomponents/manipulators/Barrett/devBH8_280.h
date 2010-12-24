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

#ifndef _devBH8_280_h
#define _devBH8_280_h

#include <cisstDevices/can/devCAN.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devPuck.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devGroup.h>

#include <cisstDevices/robotcomponents/manipulators/devManipulator.h>
#include <cisstDevices/devExport.h>

//! A clas for a BH8_280 device
/**
   The BH8-280 is a 4DOF robot hand manufactured by Barrett Tech. 
   From a device perspective the BH8-280 can be interfaced from either a serial
   device or a CAN device. 
*/
class CISST_EXPORT devBH8_280 : public devManipulator {

private:

  enum Errno{ ESUCCESS, EFAILURE };

  // BH8_280 specific members
  //

  RnIO* input;
  RnIO* output;

  //! A pointer to a CAN device
  devCAN* candev;

  vctDynamicVector<double> qinit;
  
  //! A vector of all the pucks
  std::vector<devPuck>   pucks;
 
  //! A vector of all the groups
  std::vector<devGroup> groups;
  
  //! Matrix used to convert motors positions to joints positions
  /**
     When given a vector of motors positions, this matrix us used to transform
     the motors positions to joints positions. For the BH8_280, this is essentially
     a block diagonal matrix.
     \sa MotorsPos2JointsPos
  */
  vctDynamicMatrix<double> mpos2jpos;

  //! Matrix used to convert joints positions to motors positions
  /**
     When given a vector of joints positions, this matrix us used to transform
     the joints positions to motors positions. For the BH8_280, this is essentially 
     a block diagonal matrix.
     \sa JointsPos2MotorsPos
  */
  vctDynamicMatrix<double> jpos2mpos;

  //! Matrix used to convert joints torques to motors torques
  /**
     When given a vector of joints torques, this matrix us used to transform
     the joints torques to motors torques. For the BH8_280, this is essentially  
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
  devBH8_280::Errno PackCurrents( devCAN::Frame& canframe, 
				  devGroup::ID gid, 
				  const double I[4] );

  //! Receive joints positions
  /**
     This broadcast a position query to the pucks and process all their replies.
     First, the replies (in encoder ticks) are converted to motors positions and
     then to joint angles.
     \param q[out] The resulting motor positions in radians
     \return false if no error occurred. true otherwise.
  */
  devBH8_280::Errno RecvPositions( vctDynamicVector<double>& q );

  void  Read();
  
  //! Send joints positions
  /**
     The BH8-280 has relative encoders, which means that you must "zero" the 
     encoders each time the BH8-280 is powered. For this you manually move the 
     BH8-280 to a know configuration and then call SendPosition. First, the 
     safety module is turned off to avoid triggering a velocity fault. Then, the
     joints positions are converted to encoder counts and each position is 
     overwritten by "setting" the position on each puck. Finally, the safety 
     module is turned back on. Note that this does not "move" the robot, it only
     tells each puck the absolute position of its encoder.
     \param q[in] The motor positions in radians
     \return false if no error occurred. true otherwise.
  */
  devBH8_280::Errno SendPositions( const vctDynamicVector<double>& q );

  //! Send joints torques
  /**
     Set the torques of each joint. First this converts the joints torques to 
     motors torques. Then it packs the torques in two CAN frames. The first 
     frame is addressed the upper arm group and the second frame is addressed 
     to the forearm group (if present).
     \param i[in] The motor torques
     \return false if no error occurred. true otherwise
  */
  devBH8_280::Errno SendTorques( const vctDynamicVector<double>& t );

  void Write( );

  //! Set velocity warning
  /**
     The safety module intercepts joints positions and compute joints velocities
     from them. After "zeroing" the joints (setting their initial values), the
     safety module compute the Cartesian velocity of the elbow and the Cartesian
     velocity of the 4DOF end-effector. If any of these velocities is greater 
     than the velocity warning limit, the safety module will display a warning 
     on the 7-segment of the display pendant ("E" for elbow and "A" for arm) and
     the "Warning Velocity" LED will be on. The warning won't affect anything 
     other than warn you to slow down before a velocity fault shuts down the 
     arm.
  */
  devBH8_280::Errno SetVelocityWarning( devProperty::Value velocitywarning );

  //! Set velocity fault
  /**
     The safety module intercepts joints positions and compute joints velocities
     from them. After "zeroing" the joints (setting their initial values), the
     safety module compute the Cartesian velocity of the elbos and the Cartesian
     velocity of the 4DOF end-effector. If any of these velocities is greater 
     than the velocity fault threshold, the safety module will shutdown the arm
     and display the fault on the 7-segment of the display pendant ("E" for 
     elbow and "A" for arm) and the "Velocity Fault" LED will be on. 
  */
  devBH8_280::Errno SetVelocityFault( devProperty::Value velocityfault );

  //! Set torque warning
  /**
     The safety module intercepts joints currents that are sent to the pucks.
     If any of these torques is greater than the torque warning limit, the 
     safety module will display a warning on the 7-segment of the display 
     pendant (the number of the joint with too much torque) and the "Warning 
     Torque" LED will be on. The warning won't affect anything other than warn 
     you to ease down the torque before a torque fault shuts down the arm. Keep
     in mind that this torque limit is "one size fits all". That is the torque
     warning limit of motor 1 will apply to motor 7 and vice versa.
  */
  devBH8_280::Errno SetTorqueWarning( devProperty::Value torquewarning );

  //! Set torque fault
  /**
     The safety module intercepts joints currents that are sent to the pucks.
     If any of these torques is greater than the torque fault limit, the 
     safety module will shutdown the arm and display a warning on the 7-segment 
     of the display pendant (the number of the joint with too much torque) and 
     the "Fault Torque" LED will be on. Keep in mind that this torque limit is
     "one size fits all". That is the torque warning limit of motor 1 will apply
     to motor 7 and vice versa.
  */
  devBH8_280::Errno SetTorqueFault( devProperty::Value torquefault );
  
  devBH8_280::Errno SetPucksStatus( devProperty::Value puckstatus, bool ver=false );

  //! Enable the motors
  /**
     This enables the motors of the BH8_280. This is the equivalent of "activating"
     the BH8_280 with the display pendant. So be careful. Things can move after.
  */

public:

  //! Default constructor
  /**
     Initiallize the BH8-280. This configures the pucks and groups. You can use 
     devBH8_280 with any CAN device, has long as it is derived from the devCAN 
     base class.
     \param taskname The task name
     \param period The task period
     \param candev The CAN device used by the BH8-280. This device must be 
                   derived from the devCAN classe.
     \sa devCAN
  */
  devBH8_280( const std::string& taskname, 
	      double period, 
	      osaCPUMask mask,
	      devCAN* candev,
	      const vctDynamicVector<double>& qinit );
  ~devBH8_280(){}

  void Configure( const std::string& filename = "" );
  void Cleanup();

};

#endif

























