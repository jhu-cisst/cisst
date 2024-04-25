#include <cisstRobot/robJoint.h>
#include <cisstCommon/cmnLogger.h>

#include <algorithm>  // transform
#include <iomanip>    // setw
#include <cctype>    // toupper

robJoint::robJoint() :
  type( cmnJointType::CMN_JOINT_UNDEFINED ),
  mode( robJoint::ACTIVE ),
  qoffset( 0.0 ),
  qmin( 0.0 ),
  qmax( 0.0 ),
  ftmax( 0.0 ) {}

robJoint::robJoint( cmnJointType type,
		    robJoint::Mode mode,
		    double offset,
		    double min,
		    double max,
		    double ftmax ) :
  name( "" ),
  type( type ),
  mode( mode ),
  qoffset( offset ),
  qmin( min ),
  qmax( max ),
  ftmax( ftmax ) {}


cmnJointType robJoint::GetType() const { return type; }

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

void robJoint::SetPositionOffset(const double offset) { qoffset = offset; }

const double & robJoint::PositionMin(void) const {
  return qmin;
}

double & robJoint::PositionMin(void) {
  return qmin;
}

const double & robJoint::PositionMax(void) const {
  return qmax;
}

double & robJoint::PositionMax(void) {
  return qmax;
}

const double & robJoint::ForceTorqueMax(void) const {
  return ftmax;
}

double & robJoint::ForceTorqueMax(void) {
  return ftmax;
}

robJoint::Errno robJoint::Read( std::istream& is ){

  std::string type, mode;

  is >> type        // read the stuff from the stream
     >> mode
     >> this->qoffset
     >> this->qmin
     >> this->qmax
     >> this->ftmax;

  // convert the strings to upper cases
  std::transform( type.begin(), type.end(), type.begin(), ::toupper );
  std::transform( mode.begin(), mode.end(), mode.begin(), ::toupper );

  // match to string to a joint type
  if( (type.compare("REVOLUTE") == 0) || (type.compare("HINGE") == 0) )
    this->type = cmnJointType::CMN_JOINT_REVOLUTE;

  else if( (type.compare("PRISMATIC") == 0) || (type.compare("SLIDER") == 0) )
    this->type = cmnJointType::CMN_JOINT_PRISMATIC;

  else if( type.compare("UNIVERSAL") == 0 )
    this->type = cmnJointType::CMN_JOINT_UNIVERSAL;

  else if( type.compare("BALLSOCKET") == 0 )
    this->type = cmnJointType::CMN_JOINT_BALL_SOCKET;

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

#if CISST_HAS_JSON
robJoint::Errno robJoint::Read(const Json::Value &config)
{
    std::string type, mode;

    // read values from JSON config file
    this->name = config.get("name", "").asString();
    type = config.get("type", "revolute").asString();
    mode = config.get("mode", "active").asString();
    this->qoffset = config.get("offset", 0.0).asDouble();
    this->qmin = config.get("qmin", 0.0).asDouble();
    this->qmax = config.get("qmax", 0.0).asDouble();
    this->ftmax = config.get("ftmax", 0.0).asDouble();

    // convert the strings to upper cases
    std::transform( type.begin(), type.end(), type.begin(), ::toupper );
    std::transform( mode.begin(), mode.end(), mode.begin(), ::toupper );

    // match to string to a joint type
    if( (type.compare("REVOLUTE") == 0) || (type.compare("HINGE") == 0) )
      this->type = cmnJointType::CMN_JOINT_REVOLUTE;

    else if( (type.compare("PRISMATIC") == 0) || (type.compare("SLIDER") == 0) )
      this->type = cmnJointType::CMN_JOINT_PRISMATIC;

    else if( type.compare("UNIVERSAL") == 0 )
      this->type = cmnJointType::CMN_JOINT_UNIVERSAL;

    else if( type.compare("BALLSOCKET") == 0 )
      this->type = cmnJointType::CMN_JOINT_BALL_SOCKET;

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
#endif

robJoint::Errno robJoint::Write( std::ostream& os ) const {

  switch( GetType() ){
  case cmnJointType::CMN_JOINT_REVOLUTE:
    os <<  std::setw(15) << "hinge ";
    break;
  case cmnJointType::CMN_JOINT_PRISMATIC:
    os <<  std::setw(15) << "slider ";
    break;
  case cmnJointType::CMN_JOINT_UNIVERSAL:
    os << std::setw(15) << "universal ";
    break;
  case cmnJointType::CMN_JOINT_BALL_SOCKET:
    os << std::setw(15) << "ballsocket ";
    break;
  default:
    os << std::setw(15) << "undefined ";
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
