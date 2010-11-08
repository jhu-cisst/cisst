#include <cisstRobot/robJoint.h>
#include <cisstCommon/cmnLogger.h>

#include <algorithm>  // transform
#include <iomanip>    // setw
#include <ctype.h>    // toupper

robJoint::robJoint() : 
  type( robJoint::HINGE ),
  mode( robJoint::ACTIVE ),
  qoffset( 0.0 ),
  qmin( 0.0 ),
  qmax( 0.0 ),
  ftmax( 0.0 ) {}

robJoint::robJoint( robJoint::Type type,
		    robJoint::Mode mode,
		    double offset,
		    double min,
		    double max,
		    double ftmax ) :
  type( type ),
  mode( mode ),
  qoffset( offset ),
  qmin( min ),
  qmax( max ),
  ftmax( ftmax ) {}


robJoint::Type robJoint::GetType() const { return type; }

robJoint::Mode robJoint::GetMode() const { return mode; }

double robJoint::GetPosition() const {
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;
  return 0.0;
}

double robJoint::GetVelocity() const {
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

  return 0.0;
}

double robJoint::GetForceTorque() const {
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;
  return 0.0;
}

void robJoint::SetPosition(double){
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;
}

void robJoint::SetVelocity(double){
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;
}

void robJoint::SetForceTorque( double ){
  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;
}

double robJoint::PositionOffset() const { return qoffset; }

double robJoint::PositionMin()    const { return qmin; }

double robJoint::PositionMax()    const { return qmax; }

double robJoint::ForceTorqueMax() const { return ftmax; }

robJoint::Errno robJoint::ReadJoint( std::istream& is ){ 

  std::string type, mode;
  
  is >> type        // read the stuff from the stream
     >> mode
     >> this->qoffset
     >> this->qmin 
     >> this->qmax 
     >> this->ftmax;
  
  // convert the strings to upper cases
  std::transform( type.begin(), type.end(), type.begin(), toupper );
  std::transform( mode.begin(), mode.end(), mode.begin(), toupper );
  
  // match to string to a joint type
  if( (type.compare("REVOLUTE") == 0) || (type.compare("HINGE") == 0) )
    this->type = robJoint::HINGE;
      
  else if( (type.compare("PRISMATIC") == 0) || (type.compare("SLIDER") == 0) )
    this->type = robJoint::SLIDER;

  else if( type.compare("UNIVERSAL") == 0 )
    this->type = robJoint::UNIVERSAL;

  else if( type.compare("BALLSOCKET") == 0 )
    this->type = robJoint::BALLSOCKET;

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected a joint type. Got " << type << "."
		      << std::endl;
    return robJoint::EFAILURE;
  }

  // match the mode string to a joint mode
  if( mode.compare( "PASSIVE" ) == 0 )
    this->mode = robJoint::PASSIVE;
  
  else if( mode.compare( "ACTIVE" ) == 0 )
    this->mode = robJoint::ACTIVE;
  
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected a joint mode. Got " << mode << "."
		      << std::endl;
    return robJoint::EFAILURE;
  }

  return robJoint::ESUCCESS;
}

robJoint::Errno robJoint::WriteJoint( std::ostream& os ) const {

  switch( GetType() ){
  case robJoint::HINGE:
    os <<  std::setw(10) << "hinge ";
    break;
  case robJoint::SLIDER:
    os <<  std::setw(10) << "slider ";
    break;
  case robJoint::UNIVERSAL:
    os << std::setw(10) << "universal ";
    break;
  case robJoint::BALLSOCKET:
    os << std::setw(10) << "ballsocket ";
    break;
  }

  if( GetMode() == robJoint::ACTIVE )
    os << "active ";
  else
    os << "passive ";

  os << std::setw(13) << PositionOffset() 
     << std::setw(13) << PositionMin()
     << std::setw(13) << PositionMax()
     << std::setw(13) << ForceTorqueMax();

  return robJoint::ESUCCESS;

}

