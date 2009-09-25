#ifndef _robJoint_h
#define _robJoint_h

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robMutex.h>

#include <values.h>

#include <iostream>

namespace cisstRobot{

  //! Implement a joint
  /**
     This class defines the behavior of a joint on the manipulator.
  */
  class robJoint{

  private:
    robMutex mutex;

    Real     q;    // joint position
    Real    qd;    // joint velocity
    Real   qdd;    // joint acceleration

    Real  qmin;    // minimum position
    Real  qmax;    // maximum position

    Real    ft;    // joint force(N)/torque (Nm)
    Real ftmax;    // max force/torque

    void Velocity(Real newpos, Real dt);      // don't use those directly
    void Acceleration(Real newvel, Real dt);  // 
    
  public:

    //! Default constructor
    robJoint();

    //! Overloaded constructor
    /**
       \param qs The joint angle
    */
    robJoint(Real q);

    //! Return the minimum joint angle
    Real MinPosition() const { return qmin; }

    //! Return the maximum joint angle
    Real MaxPosition() const { return qmax; }

    //! Return the maximum joint torque
    Real MaxFT() const { return ftmax; }

    //! Return the joint torque
    Real FT();

    //! Return the joint position
    Real Position();

    //! Return the joint velocity
    Real Velocity();

    //! Return the joint acceleration
    Real Acceleration();

    //! Set the joint position
    /**
       Set the joint position and, subsequently, the joint velocity and
       acceleration. The new joint position is clipped at the minimum/maximum
       values
       \param q The new joint position
       \param dt The time increment used to compute the velocity and acceleration
     */
    void Position( Real q, Real dt=MAXFLOAT );

    //! Set the joint torque
    /**
       Set the joint torque. The new value is clipped at the minimum/maximum
       torque limit.
       \param t The new torque
     */
    void FT(Real t);
        
    //! Read from an input stream
    /**
       \param s The input stream
       \param j The joint
    */
    friend std::istream& operator >> ( std::istream& is, robJoint& j ){
      is >> j.qmin >> j.qmax >> j.ftmax;
      return is;
    }
    
    //! Write to an output stream
    /**
       \param s The output stream
       \param j The joint
    */
    friend std::ostream& operator << ( std::ostream& os, const robJoint& j ){
      os << std::setw(10) << j.q
	 << std::setw(10) << j.qd
	 << std::setw(10) << j.qdd << std::endl
	 << std::setw(10) << j.qmin 
	 << std::setw(10) << j.qmax 
	 << std::setw(10) << j.ftmax << std::endl;
      return os;
    }
  };
}

#endif

