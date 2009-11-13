/*

  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robActuator_h
#define _robActuator_h

#include <cisstRobot/robDefines.h>

#include <iostream>
#include <iomanip>
#include <limits>
using namespace std;

class robActuator{
private:

  double  qmin;     // minimum position
  double  qmax;     // maximum position
  double ftmax;     // (absolute) maximum force/torque 
  
public:
  
  robActuator(){ 
    qmin = std::numeric_limits<double>::min();
    qmax = std::numeric_limits<double>::max();
    ftmax = std::numeric_limits<double>::max();
  }
  virtual ~robActuator(){}

  //! Return the minimum position angle
  virtual double PositionMin()    const { return qmin; }
  
  //! Return the maximum position angle
  virtual double PositionMax()    const { return qmax; }
  
  //! Return the maximum force/torque
  virtual double ForceTorqueMax() const { return ftmax; }
  
  virtual void Read( std::istream& is ) {
    is >> qmin >> qmax >> ftmax;
  }

  virtual void Write( std::ostream& os ) const {
    os << std::setw(13) << PositionMin()
       << std::setw(13) << PositionMax()
       << std::setw(13) << ForceTorqueMax();
  }

  //
  friend std::ostream& operator<<( std::ostream& os, 
				   const robActuator& actuator ){
    actuator.Write( os );
    return os;
  }

  //
  friend std::istream& operator>>( std::istream& is, 
				   robActuator& actuator ){
    actuator.Read( is );
    return is;
  }

};

#endif
