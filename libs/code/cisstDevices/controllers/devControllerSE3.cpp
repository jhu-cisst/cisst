#include <cisstDevices/controllers/devControllerSE3.h>

devControllerSE3::devControllerSE3( const std::string& taskname, 
				    double period,
				    const std::string& robfile,
				    const vctFrame4x4<double>& Rtw0,
				    bool enabled ) :

  devController( taskname, period, robfile, Rtw0, enabled ){

  // Create the input interface
  mtsProvidedInterface* interface;
  interface = AddProvidedInterface( devController::InputInterface );

  // Configure the input interface
  if( interface ){

    vctDynamicVector<double> q( links.size(), 0.0 );
    vctFrame4x4<double> Rt = ForwardKinematics( q );

    // POSITION

    mtstws.SetSize( 3 );     // Create the position            
    mtsvs.SetSize( 3 );      // Create the linear velocity
    mtsvsd.SetSize( 3 );     // Create the linear acceleration

    mtstws[0] = Rt[0][3];
    mtstws[1] = Rt[1][3]; 
    mtstws[2] = Rt[2][3];    // Set the initial position
    mtsvs.SetAll( 0.0 );     // Clear the linear velocity
    mtsvsd.SetAll( 0.0 );    // Clear the linear acceleration

    // Add the variables to the state table
    StateTable.AddData( mtstws, "Position" );
    StateTable.AddData( mtsvs,  "LinearVelocity" );
    StateTable.AddData( mtsvsd, "LinearAcceleration" );

    // Let another task write to these vectors
    interface->AddCommandWriteState
      ( StateTable, mtstws, devController::InputPosition );

    interface->AddCommandWriteState
      ( StateTable, mtsvs,  devController::InputLinearVelocity );

    interface->AddCommandWriteState
      ( StateTable, mtsvsd, devController::InputLinearAcceleration );

    // ORIENTATION

    mtsRws.SetSize( 3, 3 );        // Create the orientation
    mtsws.SetSize( 3 );            // Create the angular velocity
    mtswsd.SetSize( 3 );           // Create the angular acceleration

    for( size_t r=0; r<3; r++ ){
      for( size_t c=0; c<3; c++ ){
	mtsRws[r][c] = Rt[r][c];
      }
    }
    mtsws.SetAll( 0.0 );           // Clear the angular velocity
    mtswsd.SetAll( 0.0 );          // Clear the angular acceleration

    // Add the variables to the state table
    StateTable.AddData( mtsRws, "Orientation" );
    StateTable.AddData( mtsws,  "AngularVelocity" );
    StateTable.AddData( mtswsd, "AngularAcceleration" );

    // Let another task write to these vectors
    interface->AddCommandWriteState
      ( StateTable, mtsRws, devController::InputOrientation );

    interface->AddCommandWriteState
      ( StateTable, mtsws, devController::InputAngularVelocity );

    interface->AddCommandWriteState
      ( StateTable, mtswsd, devController::InputAngularAcceleration );
  }

}


