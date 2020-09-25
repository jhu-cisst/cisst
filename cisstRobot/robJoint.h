/*
  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robJoint_h
#define _robJoint_h

#include <iostream>
#include <cisstConfig.h>

#if CISST_HAS_JSON
#include <json/json.h>
#endif

#include <cisstRobot/robExport.h>

//! Joint types
/**
   These are used to identify the type of joints.
*/

class CISST_EXPORT robJoint {

 public:

  enum Type{
    UNDEFINED,   // no clue
    HINGE,       // revolute joint
    SLIDER,      // prismatic joint
    UNIVERSAL,   // universal joint
    BALLSOCKET   // ball and socket joint
  };

  //! Joint modes
  /**
     Each joint can be active (powered by a motor) or passive
  */
  enum Mode{
    ACTIVE,
    PASSIVE
  };

  enum Errno{
    ESUCCESS,
    EFAILURE
  };

private:

  std::string name;
  
  //! The type of the joint
  /**
     Determine if the joint is a hinge, slider, universal, ball and socket, etc.
  */
  robJoint::Type type;

  //! The mode of the joint
  /**
     Determine if the joint is active (actuated) or passive
  */
  robJoint::Mode mode;

  //! The position offset (added to each joint value)
  double qoffset;

  //! The minimum joint value
  double qmin;

  //! The maximum joint value
  double qmax;

  //! The absolute maximum force or torque
  double ftmax;

public:

  //! Default constructor
  robJoint();
  robJoint( robJoint::Type type,
	    robJoint::Mode mode,
	    double offset,
	    double min,
	    double max,
	    double ftmax );

  // class with virtual methods should have a virtual destructor
  inline virtual ~robJoint() {};

  inline const std::string & Name(void) const {
    return name;
  }
  inline std::string & Name(void) {
    return name;
  }
  
  //! Return the type of the joint
  /**
     \return The type of the joint (hinge, slider, universal, ball and socket)
  */
  robJoint::Type GetType() const;

  //! Return the mode of the joint
  /**
     \return The mode of the joint (active or passive)
  */
  robJoint::Mode GetMode() const;

  //! Return the joint position
  /**
     If supported, this returns the joint position.
     \return The joint angular or linear position (no unit)
  */
  double GetPosition() const;

  //! Return the joint velocity
  /**
     If supported, this returns the joint velocity.
     \return The joint angular or linear velocity (no unit)
  */
  double GetVelocity() const;

  //! Return the joint force or torque
  /**
     If supported, this returns the joint force or torque.
     \return The joint torque or force (no unit)
  */
  double GetForceTorque() const;

  //! Set the joint position
  /**
     If supported, this sets the joint position. The position is NOT clipped to
     the position limits.
     \param q The new joint angular or linear position
  */
  void SetPosition( double q );

  //! Set the joint velocity
  /**
     If supported, this sets the joint velocity. The velocity is NOT clipped to
     the velocity limit.
     \param qd The new joint angular or linear velocity.
  */
  void SetVelocity( double qd );

  //! Set the force/torque
  /**
     If supported, this sets the force/torque. The new value is NOT clipped to
     the force/torque limit.
     \param ft The new force/torque
  */
  void SetForceTorque( double ft );

  //! Return the offset position
  /**
     \return The offset position of the joint. This value has no unit.
  */
  double PositionOffset() const;
  void SetPositionOffset(const double offset);

  //! Return the minimum position
  /**
     \return The minimum position of the joint. This value has no unit.
  */
  const double & PositionMin(void) const;
  double & PositionMin(void);

  //! Return the maximum position
  /**
     \return The maximum position of the joint. This value has no unit.
  */
  const double & PositionMax(void) const;
  double & PositionMax(void);

  //! Return the maximum force/torque
  /**
     \return The absolute value for the maximum force or torque that can be
             applied by the joint.
  */
  const double & ForceTorqueMax(void) const;
  double & ForceTorqueMax(void);

  //! Read from an input stream
  /**
     Use this method to configure the parameters of the joints from an input
     stream. The parameters are in the following order: type, mode, position
     offset, min position, max position, max force/torque.
     \param is[in] The input stream
  */
  virtual robJoint::Errno Read( std::istream& is );
#if CISST_HAS_JSON
  virtual robJoint::Errno Read( const Json::Value &config );
#endif

  //! Read from an input stream
  /**
     Use this method to write the parameters of the joints to an output
     stream. This method can be overloaded for more specific joints.
     \param os[in] The output stream
  */
  virtual robJoint::Errno Write( std::ostream& os ) const;

};

#endif
