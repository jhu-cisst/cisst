#include <cisstDevices/robotcomponents/ode/devODEBHF3.h>

#include <cisstRobot/BH/robBHF3.h>

const double devODEBHF3::TRQMAX = 1.0;
const double devODEBHF3::VELMAX = 1.0;

devODEBHF3::devODEBHF3( const std::string& devname,
			double period,
			devManipulator::State state,
			osaCPUMask mask,
			devODEWorld& world,
			dSpaceID spaceid,
			const std::string& proximalgeom,
			const std::string& intermediategeom,
			devODEBody* palm,
			double qmax ) : 

  // Initialize an empty ODE manipulator
  devODEManipulator( devname, period, state, mask ),
  sm1( NULL ),
  sm2( NULL ),
  period( period ),
  qmax( fabs( qmax ) ) {

  // The ODE bodies of the finger
  devODEBody *proximal, *intermediate;
  devODEJoint *mcp2, *pip;

  qinit = vctDynamicVector<double>( 2, 0.0, cmnPI/4.0 );

  dBodyID palmbodyID = NULL;
  if( palm != NULL ) 
    { palmbodyID = palm->BodyID(); }


  // position and orientation of the ith link 
  vctFrame4x4<double> Rtwi;
  vctFixedSizeVector<double,3> z( 0.0, 0.0, 1.0 ); // the local Z axis
  vctDynamicVector<double> q( 1, 0.0 );

  // Initialize the finger
  robBHF3 f3;  
  if( palm != NULL ){
    f3.Rtw0 = ( vctFrame4x4<double>(palm->GetOrientation(), palm->GetPosition())
		* f3.Rtw0 );
  }


  //
  // First link and first joint
  //


  // create and initialize the second link (proximal)
  proximal = new devODEBody( world.WorldID(),                       // world
			     spaceid,                               // space
			     f3.ForwardKinematics( q, robBHF3::PROXIMAL ),
			     f3.links[0].Mass(),                    // m   
			     f3.links[0].CenterOfMass(),            // com
			     f3.links[0].MomentOfInertiaAtCOM(),    // I  
			     proximalgeom );
  this->Insert( proximal );
  world.Insert( proximal );

  // Create and initialize the second joint (MCP)
  Rtwi = f3.ForwardKinematics( q, robBHF3::METACARP );
  mcp2 = new devODEJoint( world.WorldID(),            // the world ID
			  palmbodyID,
			  proximal->BodyID(),         // the second body
			  dJointTypeHinge,            // the joint type
			  Rtwi.Translation(),         // the XYZ position
			  Rtwi.Rotation() * z,        // the Z axis 
			  0.0,                        // the lower limit
			  f3.links[0].PositionMax() -
			  f3.links[0].PositionMin() );// the upper limit
  this->Insert( mcp2 );
  world.Insert( mcp2 );

  sm1 = new devODEServoMotor( world.WorldID(), 
			      palmbodyID,                 // the first body
			      proximal->BodyID(),         // the second body
			      Rtwi.Rotation() *-z,        // the Z axis 
			      VELMAX,
			      TRQMAX );                        //




  //
  // Second link and second joint
  //




  // create and initialize the third link (intermediate)
  intermediate = new devODEBody( world.WorldID(),                   // world
				 spaceid,                           // space
				 f3.ForwardKinematics(q, robBHF3::INTERMEDIATE),
				 f3.links[1].Mass(),                // m   
				 f3.links[1].CenterOfMass(),        // com
				 f3.links[1].MomentOfInertiaAtCOM(),// I  
				 intermediategeom );
  this->Insert( intermediate );
  world.Insert( intermediate );

  // Create and initialize the second joint (PIP)
  Rtwi = f3.ForwardKinematics( q, robBHF3::PROXIMAL );
  pip = new devODEJoint( world.WorldID(),             // the world ID
			 proximal->BodyID(),          // the first body
			 intermediate->BodyID(),      // the second body
			 dJointTypeHinge,             // the joint type
			 Rtwi.Translation(),          // the XYZ position
			 Rtwi.Rotation() * z,         // the Z axis 
			 0.0,                         // the lower limit
			 f3.links[1].PositionMax() -
			 f3.links[1].PositionMin() ); // the upper limit
  this->Insert( pip );
  world.Insert( pip );

  sm2 = new devODEServoMotor( world.WorldID(), 
			      proximal->BodyID(),         // the first body
			      intermediate->BodyID(),     // the second body
			      Rtwi.Rotation() *-z,        // the Z axis 
			      VELMAX,
			      TRQMAX );                      //
  
}

void devODEBHF3::Write( const vctDynamicVector<double>& qs ){

  double qs1 = qinit[0] + qs[0];
  double qs2 = qinit[1] + qs[0]*(45.0/140.0);

  vctDynamicVector<double> q = GetJointsPositions();
  double q1 = q[0];
  double q2 = q[1];

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



