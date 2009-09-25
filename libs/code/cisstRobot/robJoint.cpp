#include <cisstRobot/robJoint.h>

using namespace cisstRobot;

cisstRobot::robJoint::robJoint(){ 
  q = qd = qdd = ft = 0;
  qmin = -M_PI;
  qmax =  M_PI;
  ftmax = 0; 
}

Real cisstRobot::robJoint::Position(){  
  Real q;  
  mutex.Lock();  
  q = this->q;   
  mutex.Unlock();  
  return q;  
}

Real cisstRobot::robJoint::Velocity(){  
  Real qd;  
  mutex.Lock();  
  qd = this->qd;   
  mutex.Unlock();  
  return qd;  
}

Real cisstRobot::robJoint::Acceleration(){  
  Real qdd;  
  mutex.Lock();  
  qdd = this->qdd;   
  mutex.Unlock();  
  return qdd;  
}
    
void cisstRobot::robJoint::Position( Real newq, Real dt ) {  

  Velocity(newq, dt);      // first compute the velocity

  mutex.Lock();
  this->q = newq;
  mutex.Unlock();
}
    
void cisstRobot::robJoint::Velocity( Real newq, Real dt ){

  Real newqd=(newq-q)/dt;  // compute the new velocity

  Acceleration(newqd, dt); // first compute the acceleration

  mutex.Lock();
  this->qd = newqd;
  mutex.Unlock();
}

void cisstRobot::robJoint::Acceleration( Real newqd, Real dt ){

  Real newqdd=(newqd-qd)/dt;

  mutex.Lock();
  qdd = newqdd;
  mutex.Unlock();
}

Real cisstRobot::robJoint::FT(){  
  Real ft;  
  mutex.Lock();  
  ft = this->ft;   
  mutex.Unlock();  
  return ft;
}

void cisstRobot::robJoint::FT(Real ft) {  
  mutex.Lock();  
  this->ft = cisstRobot::Saturate( ft, -ftmax, ftmax );  
  mutex.Unlock();
}

