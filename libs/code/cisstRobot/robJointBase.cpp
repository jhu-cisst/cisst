#include <cisstRobot/robJointBase.h>
#include <cisstCommon/cmnLogger.h>

#include <algorithm>  // transform
#include <iomanip>    // setw
#include <ctype.h>    // toupper

robJointBase::robJointBase(){
  this->type = robJointHinge;
  this->mode = robJointActive;
  this->qoffset = 0.0;
  this->qmin = 0.0;
  this->qmax = 0.0;
  this->ftmax = 0.0;
}

robJointBase::~robJointBase(){}

robJointType robJointBase::JointType() const { return type; }

robJointMode robJointBase::JointMode() const { return mode; }

double 
robJointBase::GetJointPosition() const {

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

  return 0.0;
}

double 
robJointBase::GetJointVelocity() const {

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

  return 0.0;
}

double 
robJointBase::GetJointForceTorque() const {

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

  return 0.0;

}

void 
robJointBase::SetJointPosition(double){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

}

void 
robJointBase::SetJointVelocity(double){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

}

void 
robJointBase::SetJointForceTorque(double){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
		      << "Not implemented"
		      << std::endl;

}

double robJointBase::PositionOffset() const { return qoffset; }

double robJointBase::PositionMin()    const { return qmin; }

double robJointBase::PositionMax()    const { return qmax; }

double robJointBase::ForceTorqueMax() const { return ftmax; }

robError 
robJointBase::Read( std::istream& is ){ 

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
    this->type = robJointHinge;
      
  else if( (type.compare("PRISMATIC") == 0) || (type.compare("SLIDER") == 0) )
    this->type = robJointSlider;

  else if( type.compare("UNIVERSAL") == 0 )
    this->type = robJointUniversal;

  else if( type.compare("BALLSOCKET") == 0 )
    this->type = robJointBallSocket;

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected a joint type. Got " << type << "."
		      << std::endl;
    return ERROR;
  }

  // match the mode string to a joint mode
  if( mode.compare( "PASSIVE" ) == 0 )
    this->mode = robJointPassive;
  
  else if( mode.compare( "ACTIVE" ) == 0 )
    this->mode = robJointActive;
  
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected a joint mode. Got " << mode << "."
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}

robError 
robJointBase::Write( std::ostream& os ) const {

  switch( JointType() ){
  case robJointHinge:
    os <<  std::setw(10) << "hinge ";
    break;
  case robJointSlider:
    os <<  std::setw(10) << "slider ";
    break;
  case robJointUniversal:
    os << std::setw(10) << "universal ";
    break;
  case robJointBallSocket:
    os << std::setw(10) << "ballsocket ";
    break;
  }

  if( JointMode() == robJointActive )
    os << "active ";
  else
    os << "passive ";

  os << std::setw(13) << PositionOffset() 
     << std::setw(13) << PositionMin()
     << std::setw(13) << PositionMax()
     << std::setw(13) << ForceTorqueMax();

  return SUCCESS;

}

