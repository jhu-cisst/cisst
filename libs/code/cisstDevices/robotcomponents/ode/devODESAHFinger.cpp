#include <cisstDevices/robotcomponents/ode/devODESAHFinger.h>
#include <cisstRobot/SAH/robSAHFinger.h>

devODESAHFinger::devODESAHFinger( const std::string& devname,
				  double period,
				  devODEWorld& world,
				  const vctFrame4x4<double>& Rtb0,
				  const std::vector<std::string>& fingergeoms,
				  dBodyID palmbodyid ) :

  devODEManipulator( devname, period, vctDynamicVector<double>(4, 0.0) ){

  // Use these angles to evaluate the forwar kinecatics
  vctFixedSizeVector<double,3> q( 0.0, 0.0, 0.0 );

  // Create a temporary finger
  robSAHFinger finger( Rtb0 );

  //! Create a new space for the finger
  dSpaceID spaceid = dSimpleSpaceCreate( world.SpaceID() );

  // Initialize the links/joints
  devODEBody *mcp, *proximal, *intermediate, *distal;
  devODEJoint *mcp1, *mcp2, *pip, *dip;

  // position and orientation of the ith link 
  vctFrame4x4<double> Rtwi;
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  /////////////////
  //     MCP LINK
  /////////////////


  // create and initialize the first link (MCP)
  mcp = new devODEBody( world.WorldID(),                         // world
			spaceid,                                 // space
			finger.ForwardKinematics
			( q, robSAHFinger::MCP ),
			finger.links[0].Mass(),                  // m   
			finger.links[0].CenterOfMass(),          // com
			finger.links[0].MomentOfInertiaAtCOM(),  // I  
			fingergeoms[0] );
  world.Insert( mcp );

  Rtwi = finger.ForwardKinematics( q, robSAHFinger::BASE );
  mcp1 =  new devODEJoint( world.WorldID(),                 // the world ID
			   palmbodyid,                      // the first body
			   mcp->BodyID(),                   // the second body
			   dJointTypeHinge,                 // the joint type
			   Rtwi.Translation(),              // the XYZ position
			   Rtwi.Rotation() * z,             // the Z axis 
			   finger.links[0].PositionMin(),   // the lower limit
			   finger.links[0].PositionMax() ); // the upper limit
  world.Insert( mcp1 );
  joints.push_back( mcp1 );


  /////////////////
  // PROXIMAL LINK
  /////////////////


  // create and initialize the second link (proximal)
  proximal = new devODEBody( world.WorldID(),                         // world
			     spaceid,                                 // space
			     finger.ForwardKinematics
			     (q,robSAHFinger::PROXIMAL),
			     finger.links[1].Mass(),                  // m   
			     finger.links[1].CenterOfMass(),          // com
			     finger.links[1].MomentOfInertiaAtCOM(),  // I  
			     fingergeoms[1] );
  world.Insert( proximal );

  Rtwi = finger.ForwardKinematics( q, robSAHFinger::MCP );
  mcp2 =  new devODEJoint( world.WorldID(),                 // the world ID
			   mcp->BodyID(),                   // the first body
			   proximal->BodyID(),              // the second body
			   dJointTypeHinge,                 // the joint type
			   Rtwi.Translation(),              // the XYZ position
			   Rtwi.Rotation() * z,             // the Z axis 
			   finger.links[1].PositionMin(),   // the lower limit
			   finger.links[1].PositionMax() ); // the upper limit
  world.Insert( mcp2 );
  joints.push_back( mcp2 );


  /////////////////
  // INTERMEDIATE LINK
  /////////////////

  // create and initialize the third link (intermediate)
  intermediate = new devODEBody( world.WorldID(),                       // world
				 spaceid,                               // space
				 finger.ForwardKinematics
				 (q, robSAHFinger::INTERMEDIATE),
				 finger.links[2].Mass(),                // m   
				 finger.links[2].CenterOfMass(),        // com
				 finger.links[2].MomentOfInertiaAtCOM(),// I  
				 fingergeoms[2] );

  Rtwi = finger.ForwardKinematics( q, robSAHFinger::PROXIMAL );
  pip =  new devODEJoint( world.WorldID(),                 // the world ID
			  proximal->BodyID(),              // the first body
			  intermediate->BodyID(),          // the second body
			  dJointTypeHinge,                 // the joint type
			  Rtwi.Translation(),              // the XYZ position
			  Rtwi.Rotation() * z,             // the Z axis 
			  //finger.links[2].PositionMin(),   // the lower limit
			  0.0,
			  finger.links[2].PositionMax() ); // the upper limit
  world.Insert( intermediate );
  world.Insert( pip );
  joints.push_back( pip );


  /////////////////
  //  DISTAL LINK
  /////////////////



  // create and initialize the third link (intermediate)
  distal = new devODEBody( world.WorldID(),                       // world
			   spaceid,                               // space
			   finger.ForwardKinematics
			   (q,robSAHFinger::DISTAL),
			   finger.links[3].Mass(),                // m   
			   finger.links[3].CenterOfMass(),        // com
			   finger.links[3].MomentOfInertiaAtCOM(),// I  
			   fingergeoms[3] );

  Rtwi = finger.ForwardKinematics( q, robSAHFinger::INTERMEDIATE );
  dip =  new devODEJoint( world.WorldID(),                 // the world ID
			  intermediate->BodyID(),          // the first body
			  distal->BodyID(),                // the second body
			  dJointTypeHinge,                 // the joint type
			  Rtwi.Translation(),              // the XYZ position
			  Rtwi.Rotation() * z,             // the Z axis 
			  //finger.links[3].PositionMin(),   // the lower limit
			  0.0,
			  finger.links[3].PositionMax() ); // the upper limit
  world.Insert( distal );
  world.Insert( dip );
  joints.push_back( dip );

}
