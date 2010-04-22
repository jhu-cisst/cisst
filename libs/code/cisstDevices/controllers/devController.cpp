#include <cisstDevices/controllers/devController.h>
#include <cisstDevices/manipulators/devManipulator.h>

const std::string devController::ControlInterface   = "ControllerIOCTL";
const std::string devController::Enable             = "Enable";

const std::string devController::InputInterface        ="ControllerInput";
const std::string devController::InputJointPosition    ="InputJointPosition";
const std::string devController::InputJointVelocity    ="InputJointVelocity";
const std::string devController::InputJointAcceleration="InputJointAcceleration";

const std::string devController::InputPosition          ="InputPosition";
const std::string devController::InputLinearVelocity    ="InputLinearVelocity";
const std::string devController::InputLinearAcceleration="InputLinearAcceleration";

const std::string devController::InputOrientation        ="InputOrientation";
const std::string devController::InputAngularVelocity    ="InputAngularVelocity";
const std::string devController::InputAngularAcceleration="InputAngularAcceleration";

const std::string devController::FeedbackInterface  = "ControllerFeedback";
const std::string devController::Feedback           = "Feedback";

const std::string devController::OutputInterface    = "ControllerOutput";
const std::string devController::Output             = "Output";

devController::devController( const std::string& taskname, 
			      double period,
			      const std::string& robfile,
			      const vctFrame4x4<double>& Rtw0,
			      bool enabled ) : 
  // initialize the task
  mtsTaskPeriodic( taskname, period, true ),

  // initialize the manipulator
  robManipulator( robfile, Rtw0 ),
  told(-1.0) ,
  name( taskname ),
  mtsEnabled( enabled ),
  risingedge( false ){

  // Create the control interface
  {
    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( devController::ControlInterface );
    if( interface ){
      StateTable.AddData( mtsEnabled, "Enabled" );
      interface->AddCommandWriteState
	( StateTable, mtsEnabled, devController::Enable );
    }
  }  

  // Create the output interface
  {
    mtsRequiredInterface* interface;
    interface = AddRequiredInterface( devController::OutputInterface );
    if( interface )
      { interface->AddFunction( devManipulator::Input, mtsFnSetOutput ); }
  }

  // Create the feedback interface
  {
    mtsRequiredInterface* interface;
    interface = AddRequiredInterface( devController::FeedbackInterface );
    if( interface )
      { interface->AddFunction( devManipulator::Output, mtsFnGetFeedback ); }
  }

}

void devController::Startup(){}

void devController::Run(){

  ProcessQueuedCommands();

  if( mtsEnabled ){
  
    mtsVector<double> q;
    mtsFnGetFeedback( q );

    double t = q.Timestamp();
    if( 0.0<told && 0<( t-told ) ){
      double dt = t-told;
      vctDynamicVector<double> qd = (q - qold)/dt;
      vctDynamicVector<double> tau;
      tau =  Control( vctDynamicVector<double>(q), qd, dt );
      mtsFnSetOutput( mtsVector<double>( tau ) );
    }

    told = t;
    qold = q;

    if( !risingedge ){
      CMN_LOG_RUN_ERROR << name << " enabled." << std::endl;
      risingedge = true;
    }

  }
  else{
    told = -1.0;
    if( risingedge ){
      CMN_LOG_RUN_ERROR << name << " disabled." << std::endl;
      risingedge = false;
    }

  }

}
