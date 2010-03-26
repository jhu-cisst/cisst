#include <cisstDevices/controlloops/devControlLoop.h>

const std::string devControlLoop::ControlInterface  = "ControlInterface";
const std::string devControlLoop::ReadEnableCommand = "ReadCommand";

const std::string devControlLoop::ReferenceInterface               = "ReferenceInterface";
const std::string devControlLoop::ReadReferencePositionCommand     = "ReadReferencePosition";
const std::string devControlLoop::ReadReferenceVelocityCommand     = "ReadReferenceVelocity";
const std::string devControlLoop::ReadReferenceAccelerationCommand = "ReadReferenceAcceleration";

const std::string devControlLoop::FeedbackInterface   = "FeedbackInterface";
const std::string devControlLoop::ReadFeedbackCommand = "ReadFeedback";

const std::string devControlLoop::OutputInterface    = "OutputInterface";
const std::string devControlLoop::WriteOutputCommand = "WriteOutput";

devControlLoop::devControlLoop( const std::string& taskname, 
				double period,
				const std::string& robfile,
				const vctFrame4x4<double>& Rt,
				const std::vector<devGeometry*> geoms ) :
  // initialize the task
  mtsTaskPeriodic( taskname, period, true ),
  // initialize the manipulator
  robManipulator( robfile, Rt ) {

  // copy the geometries
  if( !geoms.empty() )
    { this->geoms.insert( this->geoms.begin(), geoms.begin(), geoms.end() ); }

  mtsProvidedInterface* interface;

  // Create the control interface
  interface = AddProvidedInterface( devControlLoop::ControlInterface );

  // Configure the control interface
  if( interface ){
    enabled = true;
    //enabled = false;
    StateTable.AddData( enabled, "Enabled" );
    interface->AddCommandWriteState( StateTable, 
				     enabled,
				     devControlLoop::ReadEnableCommand );
  }
  
  // Create the output interface
  interface = AddProvidedInterface( devControlLoop::OutputInterface );
  
  // Configure the output interface
  if( interface ){
    output.SetSize( links.size() );
    output.SetAll( 0.0 );
    StateTable.AddData( output, "OutputVector" );      
    interface->AddCommandReadState( StateTable, 
				    output,
				    devControlLoop::WriteOutputCommand );
  }

  // the control loop must obtain its input
  AddRequiredInterface( devControlLoop::ReferenceInterface );
  AddRequiredInterface( devControlLoop::FeedbackInterface );

}

void devControlLoop::Startup(){

  mtsDeviceInterface* interface;

  // get the interface connected to the controller's feedback interface
  interface = GetProvidedInterfaceFor( devControlLoop::FeedbackInterface );  
  // Bind the command to read feedback
  if( interface )
    { ReadFeedback.Bind( interface, devControlLoop::ReadFeedbackCommand ); }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to get the provided interface for "
		       << devControlLoop::FeedbackInterface
		       << std::endl;
  }

  // get the interface connected to the controller's feedback interface
  interface = GetProvidedInterfaceFor( devControlLoop::ReferenceInterface );  
  // Bind the command to read feedback
  if( interface ){ 
    ReadReferencePosition.Bind( interface, devControlLoop::ReadReferencePositionCommand ); 
    ReadReferenceVelocity.Bind( interface, devControlLoop::ReadReferenceVelocityCommand ); 
    ReadReferenceAcceleration.Bind( interface, devControlLoop::ReadReferenceAccelerationCommand ); 
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << ": Failed to get the provided interface for "
		       << devControlLoop::ReferenceInterface
		       << std::endl;
  }
}

void devControlLoop::Run(){
  
  ProcessQueuedCommands();
  
  // Query the device to obtain joint angles
  mtsVector<double> q;
  ReadFeedback( q );

  if( enabled && q.size() == links.size() ){
    output = Control( q );
    // any models to render?
    if( !geoms.empty() ){ 
      for( size_t i=0; i<geoms.size(); i++ )
	{ geoms[i]->SetPositionOrientation( ForwardKinematics( q, i+1 ) ); }
    }
  }
}
