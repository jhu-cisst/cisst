/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstDevices/ode/devODEManipulator.h>

//CMN_IMPLEMENT_SERVICES( devODEManipulator );

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      const vctDynamicVector<double>& qinit ):

  devManipulator( devname, period, qinit.size() ),
  robManipulator( "", vctFrame4x4<double>() ),
  qinit( qinit ){}

devODEManipulator::devODEManipulator(const std::string& devname,
				     double period,
				     devODEWorld& world,
				     const std::string& robotfilename,
				     const vctFrame4x4<double>& Rtw0,
				     const vctDynamicVector<double> qinit,
				     const std::vector<std::string>& geomfiles):
  devManipulator( devname, period, qinit.size() ),
  robManipulator( robotfilename, Rtw0 ),
  qinit( qinit ){

  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world.SpaceID() );

  // Initialize the links

  // a temporary vector to hold pointers to bodies
  std::vector<devODEBody*> bodies;

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i+1 );

    // create and initialize the ith link
    devODEBody* body;
    body = new devODEBody( world.WorldID(),                         // world
			   spaceid,                                 // space
			   Rtwi,                                    // pos+ori
			   links[i].Mass(),                         // m   
			   links[i].CenterOfMass(),                 // com
			   links[i].MomentOfInertiaAtCOM(),         // I  
			   geomfiles[i] );

    world.Insert( body );
    bodies.push_back( body );
  }

  // Initialize the joints
  dBodyID b1 = 0;                                // ID of initial proximal link
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the ID of the distal link 
    dBodyID b2 = bodies[i]->BodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    int type = dJointTypeHinge;
    if( links[i].GetType() == robLink::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    devODEJoint* joint;
    joint =  new devODEJoint( world.WorldID(),     // the world ID
			      b1,                  // the proximal body
			      b2,                  // the distal body
			      type,                // the joint type
			      anchor,              // the XYZ position
			      axis,                // the Z axis 
			      qmin,                // the lower limit
			      qmax );              // the upper limit
    joints.push_back( joint );
    world.Insert( joint );

    b1 = b2;                               // proximal is now distal
  }
}

vctDynamicVector<double> devODEManipulator::Read()
{ return GetJointsPositions(); }

void devODEManipulator::Write( const vctDynamicVector<double>& ft )
{ SetForcesTorques( ft ); }

vctDynamicVector<double> devODEManipulator::GetJointsPositions() const {
  vctDynamicVector<double> q( joints.size(), 0.0 );
  for(size_t i=0; i<joints.size(); i++)
    { q[i] =  joints[i]->GetPosition(); }

  return q + qinit;
}

vctDynamicVector<double> devODEManipulator::GetJointsVelocities() const {
  vctDynamicVector<double> qd(joints.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    { qd[i] = joints[i]->GetVelocity(); }
  return qd;
}

void devODEManipulator::SetJointsPositions( const vctDynamicVector<double>& q ){

  // we don't really set the joint position. We use the joint positions to 
  // compute the pos/ori of each body.
  for(size_t i=0; i<joints.size(); i++){
    
    vctFrame4x4<double> Rtwi = ForwardKinematics( q, i+1 );
    dBodyID bid = joints[i]->GetDistalBody();

    dMatrix3 Rwi = { Rtwi[0][0], Rtwi[0][1], Rtwi[0][2], 0.0,
		     Rtwi[1][0], Rtwi[1][1], Rtwi[1][2], 0.0,
		     Rtwi[2][0], Rtwi[2][1], Rtwi[2][2], 0.0 };

    dBodySetPosition( bid, Rtwi[0][3], Rtwi[1][3], Rtwi[2][3] );
    dBodySetRotation( bid, Rwi );

  }

}

void devODEManipulator::SetJointsVelocities(const vctDynamicVector<double>& ){
}

void devODEManipulator::SetForcesTorques( const vctDynamicVector<double>& ft) {
  for(size_t i=0; i<joints.size(); i++ )
    { joints[i]->SetForceTorque( ft[i] ); }
}

void devODEManipulator::SetState( const vctDynamicVector<double>& q,
				  const vctDynamicVector<double>& qd ) {
  SetJointsPositions( q );
  SetJointsVelocities( qd );
}

void devODEManipulator::GetState( vctDynamicVector<double>& q,
				  vctDynamicVector<double>& qd ) const {
  q = GetJointsPositions();
  qd = GetJointsVelocities();
}
