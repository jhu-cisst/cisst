#include <cisstDevices/controllers/devControllerSE3.h>
#include <cisstVector/vctQuaternionRotation3.h>

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

    vctFrame4x4<double> Rt;
    Rt = ForwardKinematics( vctDynamicVector<double>( links.size(), 0.0 ) );

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

    vctMatrixRotation3<double> Rws( Rt[0][0], Rt[0][1], Rt[0][2], 
				    Rt[1][0], Rt[1][1], Rt[1][2], 
				    Rt[2][0], Rt[2][1], Rt[2][2],
				    VCT_NORMALIZE );
    mtsqws = vctQuaternionRotation3<double>( Rws );

    mtsws.SetSize( 3 );            // Create the angular velocity
    mtsws.SetAll( 0.0 );           // Clear the angular velocity

    mtswsd.SetSize( 3 );           // Create the angular acceleration
    mtswsd.SetAll( 0.0 );          // Clear the angular acceleration

    // Add the variables to the state table
    StateTable.AddData( mtsqws, "Orientation" );
    StateTable.AddData( mtsws,  "AngularVelocity" );
    StateTable.AddData( mtswsd, "AngularAcceleration" );

    // Let another task write to these vectors
    interface->AddCommandWriteState
      ( StateTable, mtsqws, devController::InputOrientation );

    interface->AddCommandWriteState
      ( StateTable, mtsws, devController::InputAngularVelocity );

    interface->AddCommandWriteState
      ( StateTable, mtswsd, devController::InputAngularAcceleration );
  }

}


