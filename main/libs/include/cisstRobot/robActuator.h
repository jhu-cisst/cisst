#ifndef _robActuator_h
#define _robActuator_h

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robMutex.h>

#include <iostream>

namespace cisstRobot{

  class robActuator{
  protected:
    
    robMutex mutex;
    
    Real     q;     // motor position
    Real    qd;     // motor velocity
    Real   qdd;     // motor acceleration

    Real    ft;     // force torque;
    Real ftmax;     // force/torque uppper limits    

  public:

    robActuator(){ q = qd = qdd = ft = ftmax = 0.0; }
    virtual ~robActuator(){}

    //! Get position
    virtual Real Position() { 
      mutex.Lock(); 
      Real q = this->q; 
      mutex.Unlock(); 
      return q;
    }

    //! Set position
    virtual void Position( Real q ) { 
      mutex.Lock(); 
      this->q = q; 
      mutex.Unlock(); 
    }

    //! Get Force/Torque
    virtual Real FT() { 
      mutex.Lock(); 
      Real ft = this->ft; 
      mutex.Unlock(); 
      return ft; 
    }

    //! Set Force/Torque
    virtual void FT( Real ft ){
      mutex.Lock(); 
      this->ft = cisstRobot::Saturate( ft, -FTmax(), FTmax() ); 
      mutex.Unlock();
    }

    //! Get maximum Force/Torque
    Real FTmax() const { return ftmax; }

    //
    virtual void Print() const {
      std::cout << std::setw(10) << q 
		<< std::setw(10) << qd
		<< std::setw(10) << qdd << std::endl
		<< std::setw(10) << ft 
		<< std::setw(10) << ftmax << std::endl;
    }
  };
}
#endif
