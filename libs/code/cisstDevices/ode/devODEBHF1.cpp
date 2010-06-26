#include <cisstDevices/ode/devODEBHF1.h>

#include <cisstRobot/BH/robBHF1.h>

devODEBHF1::devODEBHF1( const std::string& devname,
			double period,
			devODEWorld& world,
			dSpaceID spaceid,
			const std::string& metacarpgeom,
			const std::string& proximalgeom,
			const std::string& intermediategeom,
			devODEBody* palm,
			double qmax ) : 

  // Initialize an empty ODE manipulator
  devODEManipulator( devname, 
		     period, 
		     vctDynamicVector<double>( 3, 0.0, 0.0, cmnPI/4.0 ) ),
  sm0( NULL ),
  sm1( NULL ),
  sm2( NULL ),
  period( period ),
  qmax( fabs( qmax ) ) {

  // The ODE bodies of the finger
  devODEBody *metacarp, *proximal, *intermediate;
  devODEJoint *mcp1, *mcp2, *pip;
  
  dBodyID palmbodyID = NULL;
  if( palm != NULL )
    { palmbodyID = palm->BodyID(); }

  // position and orientation of the ith link 
  vctFrame4x4<double> Rtwi;
  vctFixedSizeVector<double,3> z( 0.0, 0.0, 1.0 ); // the local Z axis
  vctDynamicVector<double> q( 2, 0.0 );

  // Initialize the finger
  robBHF1 f1;  
  if( palm != NULL ) {
    f1.Rtw0 = ( vctFrame4x4<double>(palm->GetOrientation(), palm->GetPosition())
		* f1.Rtw0 );
  }

  //
  // First link and first joint
  //



  // Create and initialize the first link (metacarp)
  metacarp = new devODEBody( world.WorldID(),                       // world
			     spaceid,                               // space
			     f1.ForwardKinematics( q, robBHF1::METACARP ),
			     f1.links[0].Mass(),                    // m   
			     f1.links[0].CenterOfMass(),            // com
			     f1.links[0].MomentOfInertiaAtCOM(),    // I  
			     metacarpgeom );                        // geometry
  this->Insert( metacarp );
  world.Insert( metacarp );

  // Create and initialize the first joint (MCP)
  Rtwi = f1.ForwardKinematics( q, robBHF1::BASE );
  mcp1 = new devODEJoint( world.WorldID(),            // the world ID
			  palmbodyID,                 // the first body
			  metacarp->BodyID(),         // the second body
			  dJointTypeHinge,            // the joint type
			  Rtwi.Translation(),         // the XYZ position
			  Rtwi.Rotation() * z,        // the Z axis 
			  0.0,                        // the lower limit
			  f1.links[0].PositionMax() -
			  f1.links[0].PositionMin() );// the upper limit
  this->Insert( mcp1 );
  world.Insert( mcp1 );

  sm0 = new devODEServoMotor( world.WorldID(), 
			      palmbodyID,                 // the first body
			      metacarp->BodyID(),         // the second body
			      Rtwi.Rotation() *-z,        // the Z axis 
			      1 );                      //
			      


  //
  // Second link and second joint
  //



  // create and initialize the second link (proximal)
  proximal = new devODEBody( world.WorldID(),                       // world
			     spaceid,                               // space
			     f1.ForwardKinematics( q, robBHF1::PROXIMAL ),
			     f1.links[1].Mass(),                    // m   
			     f1.links[1].CenterOfMass(),            // com
			     f1.links[1].MomentOfInertiaAtCOM(),    // I  
			     proximalgeom );
  this->Insert( proximal );
  world.Insert( proximal );

  // Create and initialize the second joint (MCP)
  Rtwi = f1.ForwardKinematics( q, robBHF1::METACARP );
  mcp2 = new devODEJoint( world.WorldID(),            // the world ID
			  metacarp->BodyID(),         // the first body
			  proximal->BodyID(),         // the second body
			  dJointTypeHinge,            // the joint type
			  Rtwi.Translation(),         // the XYZ position
			  Rtwi.Rotation() * z,        // the Z axis 
			  0.0,                        // the lower limit
			  f1.links[1].PositionMax() -
			  f1.links[1].PositionMin() );// the upper limit
  this->Insert( mcp2 );
  world.Insert( mcp2 );

  sm1 = new devODEServoMotor( world.WorldID(), 
			      metacarp->BodyID(),         // the first body
			      proximal->BodyID(),         // the second body
			      Rtwi.Rotation() *-z,        // the Z axis 
			      1 );                      //



  //
  // Third link and third joint
  //



  // create and initialize the third link (intermediate)
  intermediate = new devODEBody( world.WorldID(),                   // world
				 spaceid,                           // space
				 f1.ForwardKinematics(q, robBHF1::INTERMEDIATE),
				 f1.links[2].Mass(),                // m   
				 f1.links[2].CenterOfMass(),        // com
				 f1.links[2].MomentOfInertiaAtCOM(),// I  
				 intermediategeom );
  this->Insert( intermediate );
  world.Insert( intermediate );

  // Create and initialize the second joint (PIP)
  Rtwi = f1.ForwardKinematics( q, robBHF1::PROXIMAL );
  pip = new devODEJoint( world.WorldID(),             // the world ID
			 proximal->BodyID(),          // the first body
			 intermediate->BodyID(),      // the second body
			 dJointTypeHinge,             // the joint type
			 Rtwi.Translation(),          // the XYZ position
			 Rtwi.Rotation() * z,         // the Z axis 
			 0.0,                         // the lower limit
			 f1.links[2].PositionMax() -
			 f1.links[2].PositionMin() ); // the upper limit
  this->Insert( pip );
  world.Insert( pip );

  sm2 = new devODEServoMotor( world.WorldID(), 
			      proximal->BodyID(),         // the first body
			      intermediate->BodyID(),     // the second body
			      Rtwi.Rotation() *-z,        // the Z axis 
			      1 );                      //

}

void devODEBHF1::Write( const vctDynamicVector<double>& qs ){

  double qs0 = qinit[0] + qs[0];
  double qs1 = qinit[1] + qs[1];
  double qs2 = qinit[2] + qs[1]*(45.0/140.0);

  vctDynamicVector<double> q = Read();
  double q0 = q[0];
  double q1 = q[1];
  double q2 = q[2];

  if( sm0 != NULL ){

    if( period*qmax < fabs( qs0 - q0 ) ){
      if( q0 < qs0 ) { sm0->SetVelocity(  qmax ); }
      else           { sm0->SetVelocity( -qmax ); }
    }
    else             { sm0->SetVelocity( 0.0 ); }

  }

  if( sm1 != NULL ){
    
    if( period*qmax < fabs( qs1 - q1 ) ){
      if( q1 < qs1 ) { sm1->SetVelocity(  qmax ); }
      else           { sm1->SetVelocity( -qmax ); }
    }
    else             { sm1->SetVelocity( 0.0 ); }
    
  }

  if( sm2 != NULL ){
    
    if( period*qmax*45.0/140.0 < fabs( qs2 - q2 ) ){
      if( q2 < qs2 ) { sm2->SetVelocity(  qmax*45.0/140.0 ); }
      else           { sm2->SetVelocity( -qmax*45.0/140.0 ); }
    }
    else             { sm2->SetVelocity( 0.0 ); }
    
  }

}



