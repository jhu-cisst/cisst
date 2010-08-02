#include<cisstDevices/robotcomponents/controllers/devControllerJoints.h>

devControllerJoints::devControllerJoints( const std::string& taskname,
					  double period,
					  const std::string& robfile,
					  const vctFrame4x4<double>& Rtw0,
					  bool enabled ) :
  devController( taskname, period, robfile, Rtw0, enabled ){

  mtsProvidedInterface* interface;
  // Create the input interface
  interface = AddProvidedInterface( devController::InputInterface );
  
  // Configure the input interface
  if( interface ){
    
    mtsqs.SetSize( links.size() );
    mtsqsd.SetSize( links.size() );
    mtsqsdd.SetSize( links.size() );
    
    mtsqs.SetAll( 0.0 );
    mtsqsd.SetAll( 0.0 );
    mtsqsdd.SetAll( 0.0 );
    
    StateTable.AddData( mtsqs, "JointPosition" );
    StateTable.AddData( mtsqsd, "JointVelocity" );
    StateTable.AddData( mtsqsdd, "JointAcceleration" );

    // Let another task write to these vectors
    interface->AddCommandWriteState
      ( StateTable, mtsqs, devController::InputJointPosition );

    interface->AddCommandWriteState
      ( StateTable, mtsqsd, devController::InputJointVelocity );

    interface->AddCommandWriteState
      ( StateTable, mtsqsdd, devController::InputJointAcceleration );
  }

}

