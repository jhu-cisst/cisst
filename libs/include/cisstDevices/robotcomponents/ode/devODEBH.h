

#ifndef _devODEBH_h
#define _devODEBH_h

#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>


//! ODE Barrett Hand
/**
   This class implements a Barrett hand device for ODE simulation. The class
   is derived from devODEManipulator yet it reimplements most of the virtual 
   methods due to the parallel and underactuated architecture.
   The hand creates 3 fingers, themselves ODE manipulators devices and dispatches
   the I/O to each finger.
*/
class CISST_EXPORT devODEBH : public devODEManipulator {

 private:

  devODEManipulator* f1; // Finger 1 
  devODEManipulator* f2; // Finger 2
  devODEManipulator* f3; // Finger 3

 protected:

  //! Read the state of the hand
  virtual void Read();
  //! Write the state of the hand
  virtual void Write();

 public:

  //! Barrett Hand constructor
  devODEBH( const std::string& devname,
	    double period,
	    devManipulator::State state,
	    osaCPUMask cpumask,
	    devODEWorld* world,
	    devManipulator::Mode inputmode,
	    const std::string& f1f2filename,
	    const std::string& f3filename,
	    const vctFrame4x4<double>& Rtw0,
	    const std::string& palmmodel,
	    const std::string& metacarpalmodel,
	    const std::string& proximalmodel,
	    const std::string& intermediatemodel );

  //! Barrett Hand constructor
  devODEBH( const std::string& devname,
            double period,
	    devManipulator::State state,
	    osaCPUMask cpumask,
            devODEWorld* world,
	    devManipulator::Mode inputmode,
	    const std::string& f1f2filename,
	    const std::string& f3filename,
            const vctFrm3& Rtw0,
            const std::string& palmmodel,
            const std::string& metacarpalmodel,
            const std::string& proximalmodel,
            const std::string& intermediatemodel );

  ~devODEBH();

  //! Return the joints positions
  /**
     Query each ODE joint and return the joint positions
     \return A vector of joints positions
  */
  //virtual vctDynamicVector<double> GetJointsPositions() const ;

  //! Return the joints velocities
  /**
     Query each ODE joint and return the joint velocities
     \return A vector of joints velocities
  */
  //virtual vctDynamicVector<double> GetJointsVelocities() const ;

  //! Set the joint position
  /**
     This sets the position command of ODE (internal) servo motors. This does not
     instantly changes the position. The position values are used to set the 
     velocity of the ODE servo motors.
     \param qs A vector of joint positions
  */
  virtual 
    devODEBH::Errno 
    SetPositions( const vctDynamicVector<double>& qs );
  
  //! Set the joint velocity
  /**
     This sets the velocity command of ODE (internal) servo motors. This does not
     instantly changes the velocity. The velocity values are used to set the 
     velocity of the ODE servo motors.
     \param qsd A vector of joint velocities
  */
  virtual
    devODEBH::Errno 
    SetVelocities( const vctDynamicVector<double>& qsd );

  //! Set the joint forces or torques
  /**
     This sets the force/torque value of each joint. This method does NOT apply 
     the FT right away. The FT will be applied at the next iteration of the 
     world.
     \param ft A vector of joint forces/torques
  */
  virtual 
    devODEBH::Errno 
    SetForcesTorques( const vctDynamicVector<double>& ft);

  //! Return the state of the robot
  /**
     For an ODE manipulator, the state of a manipulator is defined by the 
     position/orientation and the velocity of each link. It is not defined by the
     position/velocity of each joint.
     This method queries each link of the robot for its state 
     (position/orientation + angular/linear velocities) and return them in a 
     vector.
     \return A vector containing the state of each link.
  */
  //virtual devODEManipulator::State GetState( ) const;

  //! Set the state of the robot
  /**
     For an ODE manipulator, the state of a manipulator is defined by the 
     position/orientation and the velocity of each link. It is not defined by the
     position/velocity of each joint.
     This method set the state of each link of the robot: 
     (position/orientation + angular/linear velocities) and return them in a 
     vector.
  */
  //virtual void SetState( const devODEManipulator::State& state );

};

#endif
