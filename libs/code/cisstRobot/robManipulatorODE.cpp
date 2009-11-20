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

#include <cisstRobot/robManipulatorODE.h>
#include <iostream>
using namespace std;
#ifdef CISST_ODE_SUPPORT

robManipulatorODE::robManipulatorODE(dWorldID worldid, 
				     dSpaceID spaceid,
				     const std::string& robotfilename,
				     const std::string& toolfilename,
				     const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt)
  : robManipulator( robotfilename, toolfilename, Rt ){

  // Configure and disable all the bodies
  for( size_t i=0; i<links.size(); i++ ){    
    links[i].Configure( worldid, spaceid );
  }
  if( tool != NULL ){
    tool->Configure( worldid, spaceid );
  }


  // set the arm in the parking position. This is essential since ODE
  // consider that the the "0" position is position when a joint is created
  // so in order for ODE to assign "0" position to a joint, the joint must be
  // in the "0" position beforehand
  vctDynamicVector<double> q( joints.size(), 0.0 );
  ForwardKinematics( q ); // set the bodies position+orientation

  dBodyID b1 = 0;
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0);
  vctFixedSizeVector<double,3> anchor(0.0); // this is the anchor of a joint
  vctFixedSizeVector<double,3> axis(0.0);  // this is its axis

  anchor = Rtw0.Translation(); // the 1st anchor is the XYZ of the base
  axis = Rtw0.Rotation() * z;  // the Z axis of the base

  for( size_t i=0; i<joints.size(); i++ ){
    dBodyID b2 = links[i].BodyID();

    int type = dJointTypeHinge;
    if( links[i].IsPrismatic() )
      type = dJointTypeSlider;

    joints[i].Configure( worldid, b1, b2, type, anchor, axis );

    // this is for DH, not for MDH
    anchor = links[i].Translation();
    axis = links[i].Rotation() * z;

    b1 = b2;
  }

  if( tool != NULL ){
    dJointID fix = dJointCreateFixed( worldid, 0 );
    dJointAttach( fix, links.back().BodyID(), tool->BodyID() );
    dJointSetFixed( fix );
  }

}

vctDynamicVector<double> robManipulatorODE::GetJointsPositions() const {
  vctDynamicVector<double> q(joints.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    q[i] = joints[i].Position();
  return q;
}

vctDynamicVector<double> robManipulatorODE::GetJointsVelocities() const {
  vctDynamicVector<double> qd(joints.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    qd[i] = joints[i].Velocity();
  return qd;
}

void robManipulatorODE::Update(){
  for( size_t i=0; i<links.size(); i++ )
    links[i].Update();
  if( tool != NULL )
    tool->Update();
}

void 
robManipulatorODE::SetJointsForcesTorques( const vctDynamicVector<double>& ft) {
  for(size_t i=0; i<joints.size(); i++ )
    joints[i].ForceTorque( ft[i] );
}

#endif
