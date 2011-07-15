#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>

const std::string devTrajectory::Input     = "Input";
const std::string devTrajectory::Output    = "Output";

const std::string devTrajectory::SetMode   = "SetMode";

devTrajectory::devTrajectory( const std::string& name, 
			      double period,
			      devTrajectory::State state,
			      osaCPUMask cpumask,
			      devTrajectory::Mode mode ) :
  devRobotComponent( name, period, state, cpumask ),  // set the task
  mtsMode( mode ),                                    // start in queue mode
  tstart( -1.0 ){

  // Create the control interface
  mtsInterfaceProvided* cinterface;
  cinterface = GetInterfaceProvided( devRobotComponent::Control );
  if( cinterface ){
    StateTable.AddData( mtsMode, "Mode" );
    cinterface->AddCommandWriteState
      ( StateTable, mtsMode, devTrajectory::SetMode );
  }

}

devTrajectory::Mode devTrajectory::GetMode(){
  int tmpMode = mtsMode;
  switch( tmpMode ){
  case devTrajectory::QUEUE:  return devTrajectory::QUEUE;
  case devTrajectory::TRACK:  return devTrajectory::TRACK;
  default:                    return devTrajectory::IDLE;
  }
}

// Get the start time at startup
void devTrajectory::Startup() { tstart = osaGetTime(); }

void devTrajectory::RunComponent(){

  if( tstart < 0.0 ){ Startup(); }

  // Evaluate the current time
  double t = osaGetTime() - tstart;

  // Deal with the input of the function
  ProcessInput( t );
    
  // Deal with the output of the function
  ProcessOutput( t );
    
}

robFunction* devTrajectory::GetCurrentFunction( double t ){
  
  // For each function in the list (starting with the last one)
  for( int i=(int)functions.size()-1; 0<=i; i-- ){
    //for( size_t i=0; i<functions.size(); i++ ){
    if( functions[i] != NULL ){
      // If the time falls between the initial and final time
      if( functions[i]->StartTime() <= t && t <= functions[i]->StopTime() ){
	// return the function
	return functions[i];
      }
    }
  }

  // We are here because none of the function were current
  // If the list of function isn't empty we settled for the last function
  if( !functions.empty() )
    { return functions.back(); }
  // If the list is empty we return NULL
  else
    { return NULL; }


}

void devTrajectory::ProcessInput( double t ){

  // Is there a new input?
  if( IsInputNew() ){

    // Test the mode
    switch( GetMode() ){
      
    // Should the new input be queued?
    case devTrajectory::QUEUE: 

      {
	
	robFunction* lastfunction = NULL;
	
	// Find the last function in the queue (if any)
	if( !functions.empty() ){
	  lastfunction = functions.back();
	  
	  if( lastfunction != NULL ){
	    // Find the stop time of the last function
	    double tstop = lastfunction->StopTime(); 
	    // Use the stop time only if the stop time has not passed yet
	    if( t < tstop ) 
	      { t = tstop; }
	  }
	  
	}
	
	// Create a new function that will be queued after the last function
	robFunction* newfunction = NULL;
	newfunction = Queue( t, lastfunction );
	
	if( newfunction != NULL )
	  { functions.push_back( newfunction ); }
	    
      }
      return;

      // Should the input be tracked?
    case devTrajectory::TRACK:

      {

	robFunction* currentfunction = NULL;
	
	// Find the last function in the queue (if any)
	if( !functions.empty() )
	  { currentfunction = GetCurrentFunction( t ); }
	
	// Create a new function that will be queued after the last function
	robFunction* newfunction = NULL;
	newfunction = Track( t, currentfunction );
	
	if( newfunction != NULL )
	  { functions.push_back( newfunction ); }
	    
      }
      return;
      
    case devTrajectory::IDLE:
      return;

    }
 
  }

}

void devTrajectory::ProcessOutput( double t )
{ Evaluate( t, GetCurrentFunction( t ) ); }


