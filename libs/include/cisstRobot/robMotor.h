#ifndef _robMotor_h
#define _robMotor_h

#include <cisstRobot/robActuator.h>

namespace cisstRobot{

  //! Implements a motor
  /**
     The Motor class defines a motor that can be used to model a manipulator.
     Each motor contain a set of variables that are used to constraint motion
     and convert between torques and positions
  */
  class robMotor : public robActuator {
  private:
    
    Real I;         // motor current
    Real ipnm;      // amps per Newton meter

  public:
    
    //! Default constructor
    robMotor() : robActuator(){ I = ipnm = 0.0; }

    //! Default destructor
    ~robMotor(){}

    //! Set the torque
    /**
       Set the motor torque. This torque will be clipped between its maximum
       and minimum values
       \param t The new motor torque
    */
    void FT(Real t){
      robActuator::FT( t );
      mutex.Lock();
      this->I = this->ft * ipnm;
      mutex.Unlock();
    }

    //! Get motor current
    /**
       \return The current in the motor
    */
    Real Current(){
      mutex.Lock(); 
      Real i = I;
      mutex.Unlock(); 
      return i;
    }

    //! Set the I/Nm constant of the motor
    /**
       Use this to change the I/Nm constant of the motor. This is used because
       for a WAM, this value is given by the pucks
       \param IpNm The I/Nm constant
     */
    void IpNm(Real ipnm){
      mutex.Lock();  
      this->ipnm = ipnm;
      mutex.Unlock();  
    }

    //! Read from input stream
    /**
       Read the motor to the intput stream (i.e. cin >> v >> endl;)
        \param is The input stream
        \param motor The motor to read
        \return the modified input stream
    */
    friend std::istream& operator >> (std::istream& is, robMotor& m){
      is >> m.ftmax >> m.ipnm;
      return is;
    }
    
    //! Write to output stream
    /**
       Write the motor to the output stream (i.e. cout << v << endl;)
        \param os The output stream
        \param motor The motor to write
        \return the modified output stream
    */
    friend std::ostream& operator << (std::ostream& os, const robMotor& m){
      os << std::setw(10) << m.q
	 << std::setw(10) << m.qd
	 << std::setw(10) << m.qdd 
	 << std::setw(10) << m.ftmax << std::endl;
      return os;
    }

  }; 
}
#endif

