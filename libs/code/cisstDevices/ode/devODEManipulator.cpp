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

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/ode/devODEManipulator.h>

//CMN_IMPLEMENT_SERVICES( devODEManipulator );

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      const vctDynamicVector<double>& qinit ) :

  devManipulator( devname, period, qinit.size() ),
  qinit( qinit ){}

devODEManipulator::devODEManipulator(const std::string& devname,
				     double period,
				     devODEWorld& world,
				     const std::string& robotfilename,
				     const vctDynamicVector<double> qinit,
				     const vctFrame4x4<double>& Rtw0,
				     const std::vector<std::string>& geomfiles):
  devManipulator( devname, period, qinit.size() ),
  qinit( qinit ){

  // Create a temporary manipulator
  robManipulator manipulator( robotfilename, Rtw0 );

  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world.SpaceID() );

  // Initialize the links

  // a temporary vector to hold pointers to bodies
  std::vector<devODEBody*> links;

  for( size_t i=0; i<manipulator.links.size(); i++ ){

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = manipulator.ForwardKinematics( qinit, i+1 );

    // create and initialize the ith link
    devODEBody* link;
    link = new devODEBody( world.WorldID(),                         // world
			   spaceid,                                 // space
			   Rtwi,                                    // pos+ori
			   manipulator.links[i].Mass(),             // m   
			   manipulator.links[i].CenterOfMass(),     // com
			   manipulator.links[i].MomentOfInertiaAtCOM(),  // I  
			   geomfiles[i] );

    world.Insert( link );
    links.push_back( link );
  }

  // Initialize the joints
  dBodyID b1 = 0;                                // ID of initial proximal link
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  for( size_t i=0; i<manipulator.links.size(); i++ ){

    // obtain the ID of the distal link 
    dBodyID b2 = links[i]->BodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = manipulator.ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    int type = dJointTypeHinge;
    if( manipulator.links[i].GetType() == robLink::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = manipulator.links[i].PositionMin();
    double qmax = manipulator.links[i].PositionMax();

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
  //std::cout << q << std::endl;
  return q + qinit;
}

vctDynamicVector<double> devODEManipulator::GetJointsVelocities() const {
  vctDynamicVector<double> qd(joints.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    { qd[i] = joints[i]->GetVelocity(); }
  return qd;
}

void devODEManipulator::SetForcesTorques( const vctDynamicVector<double>& ft) {
  //std::cout << "ft " << ft << std::endl;
  for(size_t i=0; i<joints.size(); i++ )
    { joints[i]->SetForceTorque( ft[i] ); }
}
