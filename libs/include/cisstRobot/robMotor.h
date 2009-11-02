#ifndef _robMotor_h
#define _robMotor_h

#include <cisstRobot/robActuator.h>

//! Implements a motor
/**
   The Motor class defines a motor that can be used to model a manipulator.
   Each motor contain a set of variables that are used to constraint motion
   and convert between torques and positions
*/
class robMotor : public robActuator {

private:

  double I;         // motor current
  double AmpPerNm;  // amps per Newton meter
  
public:
  
  //! Default constructor
  robMotor() : robActuator(){ I = AmpPerNm = 0.0; }
  
  //! Default destructor
  ~robMotor(){}

  //! Get motor current
  /**
     \return The current in the motor
  */
  double Current() const {  return I;  }
  
  //! Set the I/Nm constant of the motor
  /**
     Use this to change the I/Nm constant of the motor. This is used because
     for a WAM, this value is given by the pucks
     \param IpNm The I/Nm constant
  */
  void IpNm( double AmpPerNm )
  { this->AmpPerNm = AmpPerNm; }
  
  //! Get the I/Nm constant of the motor
  double IpNm() const { return AmpPerNm; }

  //! Set the torque
  /**
     Set the motor torque. This torque will be clipped between its maximum
     and minimum values
     \param t The new motor torque
  */
  void ForceTorque(double newft){
    robActuator::ForceTorque( newft );
    this->I = robActuator::ForceTorque() * IpNm();
  }

  //! Read from an input stream
  /**
     Read the motor to the intput stream (i.e. cin >> v >> endl;)
     \param is The input stream
     \param motor The motor to read
     \return the modified input stream
  */
  virtual void Read( std::istream& is ){ 
    robActuator::Read( is );
    is >> AmpPerNm; 
  }
    
  //! Write to an output stream
  /**
     Write the motor to the output stream (i.e. cout << v << endl;)
     \param os The output stream
     \param motor The motor to write
     \return the modified output stream
  */
  virtual void Write( std::ostream& os ) const { 
    robActuator::Write( os );
    os << AmpPerNm;
  }
  
}; 

#endif

