
#include <cisstDevices/sah/devSAH.h>
#include <cisstCommon/cmnLogger.h>

CMN_IMPLEMENT_SERVICES(devSAH);

const std::string devSAH::ThumbInterfaceName  = "ThumbInterface";
const std::string devSAH::FirstInterfaceName  = "FirstInterface";
const std::string devSAH::MiddleInterfaceName = "MiddleInterface";
const std::string devSAH::RingInterfaceName   = "RingInterface";

const std::string devSAH::SetThumbPositionCmd  = "SetThumbPosition";
const std::string devSAH::SetFirstPositionCmd  = "SetFirstPosition";
const std::string devSAH::SetMiddlePositionCmd = "SetMiddlePosition";
const std::string devSAH::SetRingPositionCmd   = "SetRingPosition";

devSAH::devSAH( const std::string& taskname,
                double period,
		devSAH::Port port, 
		devSAH::Controller controller ) :

  // configure the task
  mtsTaskPeriodic( taskname, period, true ) {

  // Configure the interface
  // The SAH requires the following interface for each finger position
  //AddRequiredInterface( devSAH::ThumbInterfaceName );
  AddRequiredInterface( devSAH::FirstInterfaceName );
  AddRequiredInterface( devSAH::MiddleInterfaceName );
  AddRequiredInterface( devSAH::RingInterfaceName );

  // Now deal with the hardware
  // copy the port
  this->port = port;

  // init the hand
  short result;
  result = sah.SAHandInit();
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to initialize. Return code was " 
		       << result
		       << std::endl;
  }

  // enable the fingers
  EnableFinger( devSAH::FTHUMB );
  EnableFinger( devSAH::FFIRST );
  EnableFinger( devSAH::FMIDDLE );
  EnableFinger( devSAH::FRING );

  // set the controller
  result = sah.SetController( port, controller );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to set the controller. Return code was " 
		       << result
		       << std::endl;
  }

  // release the Estop
  result = sah.SetEmergencyStop( port, FALSE );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to clear the ESTOP. Return code was "
		       << result
		       << std::endl;
  }

}

devSAH::~devSAH(){
  int result;

  // put the brakes
  result = sah.SetEmergencyStop( port, FALSE );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to set the ESTOP. Return code was "
		       << result
		       << std::endl;
  }

  // set the controller to reset
  result = sah.SetController( port, devSAH::CRESET );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to set the controller. Return code was " 
		       << result
		       << std::endl;
  }

  // enable the fingers
  DisableFinger( devSAH::FTHUMB );
  DisableFinger( devSAH::FFIRST );
  DisableFinger( devSAH::FMIDDLE );
  DisableFinger( devSAH::FRING );

}

void devSAH::Startup( ){
  /*
  mtsDeviceInterface* thumbinterface;
  thumbinterface = GetProvidedInterfaceFor( devSAH::ThumbInterfaceName );  
  if( thumbinterface )
    {SetThumbPosition.Bind( thumbinterface, devSAH::SetThumbPositionCmd );}
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the interface for " 
		       << devSAH::ThumbInterfaceName
		       << std::endl;
  }
  */

  mtsDeviceInterface* firstinterface;
  firstinterface = GetProvidedInterfaceFor( devSAH::FirstInterfaceName );  
  if( firstinterface )
    {SetFirstPosition.Bind( firstinterface, devSAH::SetFirstPositionCmd );}
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the interface for " 
		       << devSAH::FirstInterfaceName
		       << std::endl;
  }

  mtsDeviceInterface* middleinterface;
  middleinterface = GetProvidedInterfaceFor( devSAH::MiddleInterfaceName );  
  if( middleinterface )
    {SetMiddlePosition.Bind( middleinterface, devSAH::SetMiddlePositionCmd );}
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the interface for " 
		       << devSAH::MiddleInterfaceName
		       << std::endl;
  }

  mtsDeviceInterface* ringinterface;
  ringinterface = GetProvidedInterfaceFor( devSAH::RingInterfaceName );  
  if( ringinterface )
    {SetRingPosition.Bind( ringinterface, devSAH::SetRingPositionCmd );}
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the interface for " 
		       << devSAH::RingInterfaceName
		       << std::endl;
  }

}

void devSAH::Cleanup(){}
void devSAH::Configure( const std::string& ){}
void devSAH::Run(){

  ProcessQueuedCommands();

  mtsVector<double> fpos, mpos, rpos;
  
  {
    SetFirstPosition( fpos );
    float q[3];
    q[0] = fpos[0]; q[1] = fpos[1]; q[2] = fpos[2];
    float qd[3] = {0.01, 0.01, 0.01};
    sah.MoveFinger( port, FIRST_FINGER, q, qd );
  }
  {
    SetMiddlePosition( mpos );
    float q[3];
    q[0] = mpos[0]; q[1] = mpos[1]; q[2] = mpos[2];
    float qd[3] = {0.01, 0.01, 0.01};
    sah.MoveFinger( port, MIDDLE_FINGER, q, qd );
  }
  {
    SetRingPosition( rpos );
    float q[3];
    q[0] = rpos[0]; q[1] = rpos[1]; q[2] = rpos[2];
    float qd[3] = {0.01, 0.01, 0.01};
    sah.MoveFinger( port, RING_FINGER, q, qd );
  }

}

/*
void devSAH::move( double q1 ){

  float q[3];
  q[0] = 5; q[1] = q1; q[2] = 0;
  float qd[3] = {0.01, 0.01, 0.01};
  short result = sah.MoveFinger( port, FIRST_FINGER, q, qd );
  std::cout << result << std::endl;

}
*/

std::string devSAH::FingerName( devSAH::Finger finger ) const {
  switch( finger ){

  case devSAH::FTHUMB:
    return std::string("thumb");

  case devSAH::FFIRST:
    return std::string("first");

  case devSAH::FMIDDLE:
    return std::string("middle");

  case devSAH::FRING:
    return std::string("ring");

  default:
    return std::string("");
  }
}


devSAH::Errno devSAH::EnableFinger( devSAH::Finger finger ){
  short result = sah.SetFingerEnable( port, finger, TRUE );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to enable the " << FingerName( finger )
		       << ". Returned code was " << result
		       << std::endl;
    return devSAH::EFAILURE;
  }
  return devSAH::ESUCCESS;
}

devSAH::Errno devSAH::DisableFinger( devSAH::Finger finger ){
  short result = sah.SetFingerEnable( port, finger, FALSE );
  if( result < 0 ){
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to disable the " << FingerName( finger )
		       << ". Returned code was " << result
		       << std::endl;
    return devSAH::EFAILURE;
  }
  return devSAH::ESUCCESS;
}


/*
devSAH::Errno devSAH::CreateFingerInterface( devSAH::Finger finger ){
  mtsProvidedInterface* positioninterface = 
    AddProvidedInterface( devSAH::PositionInterfaceName );

  // Configure the position interface
  if( positioninterface ){

    // copy the initial joints positions to the state table
    jointspositions.SetSize( qinit.size() );
    jointspositions = qinit;
    StateTable.AddData( jointspositions, "JointsPositions" );

    // the position interface has a "ReadPositions" command
    positioninterface->AddCommandReadState( StateTable, 
					    jointspositions,
					    devWAM::ReadPositionsCommandName );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to create the provided interface for "
		       << devWAM::ReadPositionsCommandName
		       << std::endl;
    exit(-1);
  }
}
*/
