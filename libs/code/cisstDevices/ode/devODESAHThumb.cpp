#include <cisstDevices/ode/devODESAHThumb.h>
#include <cisstRobot/SAH/robSAHThumb.h>

devODESAHThumb::devODESAHThumb( const std::string& devname,
				double period,
				devODEWorld& world,
				const vctFrame4x4<double>& Rtb0,
				const std::string& thumbgeom,
				const std::vector<std::string>& fingergeoms,
				dBodyID palmbodyid ) :

  devODEManipulator( devname, period, vctDynamicVector<double>(5, 0.0) ) {

  vctFixedSizeVector<double,4> q( 0.0, 0.0, 0.0, 0.0 );

  // Create a temporary thumb
  robSAHThumb thumb( Rtb0 );

  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world.SpaceID() );

  // Initialize the links/joints
  devODEBody *metacarpus, *mcp, *proximal, *intermediate, *distal;
  devODEJoint *mtc, *mcp1, *mcp2, *pip, *dip;

  // position and orientation of the ith link 
  vctFrame4x4<double> Rtwi;
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  /////////////////////
  // METACARPUS LINK
  /////////////////////

  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = 0.0001;   I[1][1] = 0.0001;   I[1][1] = 0.0001;
 
  // create and initialize the first link (metacarpus)
  metacarpus = new devODEBody( world.WorldID(),                        // world
			       spaceid,                                // space
			       thumb.ForwardKinematics
			       ( q, robSAHThumb::METACARPUS ),
			       0.1,                                    // mass
			       vctFixedSizeVector<double,3>(0.0, 0.02, 0.02),
			       I,                                      // I
			       thumbgeom );
  mtc =  new devODEJoint( world.WorldID(),                // the world ID
			  palmbodyid,                     // the first body
			  metacarpus->BodyID(),           // the second body
			  dJointTypeHinge,                // the joint type
			  Rtwi.Translation(),             // the XYZ position
			  Rtwi.Rotation() * z,            // the Z axis 
			  -cmnPI_2,                       // the lower limit
			  0.0873 );                       // the upper limit

  world.Insert( metacarpus );
  world.Insert( mtc );
  joints.push_back( mtc );

  /////////////////////
  // MCP LINK
  /////////////////////

  // create and initialize the first link (MCP)
  mcp = new devODEBody( world.WorldID(),                         // world
			spaceid,                                 // space
			thumb.ForwardKinematics
			( q, robSAHThumb::MCP ),
			thumb.links[0].Mass(),                  // m   
			thumb.links[0].CenterOfMass(),          // com
			thumb.links[0].MomentOfInertiaAtCOM(),  // I  
			fingergeoms[0] );

  Rtwi = thumb.ForwardKinematics( q, robSAHThumb::METACARPUS );
  mcp1 =  new devODEJoint( world.WorldID(),                 // the world ID
			   metacarpus->BodyID(),            // the first body
			   mcp->BodyID(),                   // the second body
			   dJointTypeHinge,                 // the joint type
			   Rtwi.Translation(),              // the XYZ position
			   Rtwi.Rotation() * z,             // the Z axis 
			   thumb.links[0].PositionMin(),   // the lower limit
			   thumb.links[0].PositionMax() ); // the upper limit
  world.Insert( mcp );
  world.Insert( mcp1 );
  joints.push_back( mcp1 );
  
  /////////////////////
  // PROXIMAL LINK
  /////////////////////

  // create and initialize the second link (proximal)
  proximal = new devODEBody( world.WorldID(),                        // world
			     spaceid,                                // space
			     thumb.ForwardKinematics                 // pos/ori
			     (q, robSAHThumb::PROXIMAL),
			     thumb.links[1].Mass(),                  // m   
			     thumb.links[1].CenterOfMass(),          // com
			     thumb.links[1].MomentOfInertiaAtCOM(),  // I  
			     fingergeoms[1] );

  Rtwi = thumb.ForwardKinematics( q, robSAHThumb::MCP );
  mcp2 =  new devODEJoint( world.WorldID(),                 // the world ID
			   mcp->BodyID(),                   // the first body
			   proximal->BodyID(),              // the second body
			   dJointTypeHinge,                 // the joint type
			   Rtwi.Translation(),              // the XYZ position
			   Rtwi.Rotation() * z,             // the Z axis 
			   0,//thumb.links[1].PositionMin(),   // the lower limit
			   thumb.links[1].PositionMax() ); // the upper limit
  world.Insert( proximal );
  world.Insert( mcp2 );
  joints.push_back( mcp2 );

  /////////////////////
  // INTERMEDIATE LINK
  /////////////////////

  // create and initialize the third link (intermediate)
  intermediate = new devODEBody( world.WorldID(),                       // world
				 spaceid,                               // space
				 thumb.ForwardKinematics
				 (q, robSAHThumb::INTERMEDIATE),
				 thumb.links[2].Mass(),                // m   
				 thumb.links[2].CenterOfMass(),        // com
				 thumb.links[2].MomentOfInertiaAtCOM(),// I  
				 fingergeoms[2] );

  Rtwi = thumb.ForwardKinematics( q, robSAHThumb::PROXIMAL );
  pip =  new devODEJoint( world.WorldID(),                 // the world ID
			  proximal->BodyID(),              // the first body
			  intermediate->BodyID(),          // the second body
			  dJointTypeHinge,                 // the joint type
			  Rtwi.Translation(),              // the XYZ position
			  Rtwi.Rotation() * z,             // the Z axis 
			  -0.0873,
			  thumb.links[2].PositionMax() ); // the upper limit
  world.Insert( intermediate );
  world.Insert( pip );
  joints.push_back( pip );


  /////////////////////
  // DISTAL LINK
  /////////////////////


  // create and initialize the third link (intermediate)
  distal = new devODEBody( world.WorldID(),                       // world
			   spaceid,                               // space
			   thumb.ForwardKinematics(q,robSAHThumb::DISTAL),
			   thumb.links[3].Mass(),                // m   
			   thumb.links[3].CenterOfMass(),        // com
			   thumb.links[3].MomentOfInertiaAtCOM(),// I  
			   fingergeoms[3] );

  Rtwi = thumb.ForwardKinematics( q, robSAHThumb::INTERMEDIATE );
  dip =  new devODEJoint( world.WorldID(),                 // the world ID
			  intermediate->BodyID(),          // the first body
			  distal->BodyID(),                // the second body
			  dJointTypeHinge,                 // the joint type
			  Rtwi.Translation(),              // the XYZ position
			  Rtwi.Rotation() * z,             // the Z axis 
			  -0.0873,
			  thumb.links[3].PositionMax() ); // the upper limit
  world.Insert( distal );
  world.Insert( dip );
  joints.push_back( dip );

  
}
