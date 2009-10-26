#ifndef _robActuator_h
#define _robActuator_h

#include <cisstRobot/robDefines.h>

#include <iostream>
#include <limits>

class robActuator{
private:
    
  double     q;     // actuator position
  double    qd;     // actuator velocity
  double   qdd;     // actuator acceleration
  double    ft;     // force/torque;
  
  double  qmin;     // minimum position
  double  qmax;     // maximum position
  double ftmax;     // (absolute) maximum force/torque 
  
  //! Set the velocity
  /**
     Compute and set the actuator's velocity using a 1st order finite difference
     \param newq The new joint position
     \param dt The time increment
  */
  void Velocity(double newq, double dt=std::numeric_limits<double>::max()){
    double newqd=(newq-q)/dt;  // compute the new velocity
    Acceleration( newqd, dt ); // first compute the acceleration
    qd = newqd;                // overwrite the velocity
  }
  
  //! Set the acceleration
  /**
     Compute and set the actuator's acceleration eusing a 1st order finite 
     difference
     \param newq The new joint velocity
     \param dt The time increment
  */
  void Acceleration(double newqd, double dt=std::numeric_limits<double>::max())
  { qdd = (newqd-qd)/dt; }     // overwrite the acceleration
  
public:
  
  robActuator(){ 
    q = qd = qdd = ft = 0.0;
    qmin = std::numeric_limits<double>::min();
    qmax = std::numeric_limits<double>::max();
    ftmax = std::numeric_limits<double>::max();
  }
  virtual ~robActuator(){}

  //! Return the position
  virtual double Position()       const { return q; }
  
  //! Return the velocity
  virtual double Velocity()       const { return qd; }
  
  //! Return the acceleration
  virtual double Acceleration()   const { return qdd; }

  //! Return the torque
  virtual double ForceTorque()    const { return ft; }
  
  //! Return the minimum position angle
  virtual double PositionMin()    const { return qmin; }
  
  //! Return the maximum position angle
  virtual double PositionMax()    const { return qmax; }
  
  //! Return the maximum force/torque
  virtual double ForceTorqueMax() const { return ftmax; }
  
  //! Set the joint position
  /**
     Set the position and, subsequently, the velocity and acceleration. The new 
     position is clipped at the minimum/maximum values
     \param q The new position
     \param dt The time increment used to compute the velocity and acceleration
  */
  virtual void Position( double newq, 
			 double dt=std::numeric_limits<double>::max() ){
    newq = Saturate( newq, PositionMin(), PositionMax() );// clip the position
    Velocity(newq, dt);                                   // compute the velocity
    q = newq;                                             // overwrite the pos
  }
  
  //! Set the force/torque
  /**
     Set the force/torque. The new value is clipped at the minimum/maximum 
     force/torque limit.
     \param t The new torque
  */
  virtual void ForceTorque(double newft)
  { ft = Saturate( newft, -ForceTorqueMax(), ForceTorqueMax() ); }
  
  virtual void ISRead( std::istream& is ) {}
  virtual void OSWrite( std::ostream& os ) const {}

  //
  friend std::ostream& operator << ( std::ostream& os, 
				     const robActuator& actuator ){
    os << std::setw(13) << actuator.Position() 
       << std::setw(13) << actuator.Velocity()
       << std::setw(13) << actuator.Acceleration()
       << std::setw(13) << actuator.ForceTorque()
       << std::setw(13) << actuator.PositionMin()
       << std::setw(13) << actuator.PositionMax()
       << std::setw(13) << actuator.ForceTorqueMax();
    actuator.OSWrite( os );
    return os;
  }

  //
  friend std::istream& operator >> ( std::istream& is, 
				     robActuator& actuator ){
    is >> actuator.qmin >> actuator.qmax >> actuator.ftmax;
    actuator.ISRead( is );
    return is;
  }

};

#endif
