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

#include <cisstRobot/ode/robManipulatorODE.h>
#include <iostream>

robManipulatorODE::robManipulatorODE(dWorldID worldid, 
				     dSpaceID spaceid,
				     const std::string& robotfilename,
				     const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt): 

  robManipulator( robotfilename, Rt ){

  // Configure and disable all the bodies
  for( size_t i=0; i<links.size(); i++ ){    
    // Configure is specific to robBodyODE
    links[i].ConfigureBody( worldid, spaceid );
  }

  // set the arm in the "parking" position. This is essential since ODE
  // consider that the the "0" position is the position when a joint is created.
  // So in order for ODE to assign "0" position to a joint, the joint must be
  // in the "0" position beforehand
  vctDynamicVector<double> q( links.size(), 0.0 );
  ForwardKinematics( q ); // set the bodies position+orientation

  dBodyID b1 = 0;
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0);
  vctFixedSizeVector<double,3> anchor(0.0); // this is the anchor of a joint
  vctFixedSizeVector<double,3> axis(0.0);  // this is its axis

  anchor = Rtw0.Translation(); // the 1st anchor is the XYZ of the base
  axis = Rtw0.Rotation() * z;  // the Z axis of the base

  for( size_t i=0; i<links.size(); i++ ){
    dBodyID b2 = links[i].BodyID();

    int type = dJointTypeHinge;
    if( links[i].JointType() == robJointSlider )
      type = dJointTypeSlider;
    
    // JointODEConfigure is specific to robBodyODE
    links[i].ConfigureJoint( worldid, b1, b2, type, anchor, axis );

    // this is for DH, not for MDH
    anchor = links[i].Translation();
    axis = links[i].Rotation() * z;

    b1 = b2;
  }

}

vctDynamicVector<double>
robManipulatorODE::GetJointsPositions() const {

  vctDynamicVector<double> q(links.size(), 0.0);
  for(size_t i=0; i<links.size(); i++)
    q[i] = links[i].GetJointPosition();
  return q;

}

vctDynamicVector<double>
robManipulatorODE::GetJointsVelocities() const {

  vctDynamicVector<double> qd(links.size(), 0.0);
  for(size_t i=0; i<links.size(); i++)
    qd[i] = links[i].GetJointVelocity();
  return qd;

}

void
robManipulatorODE::Update(){

  for( size_t i=0; i<links.size(); i++ )
    links[i].Update();

}

void 
robManipulatorODE::SetJointsForcesTorques( const vctDynamicVector<double>& ft) {

  for(size_t i=0; i<links.size(); i++ )
    links[i].SetJointForceTorque( ft[i] );

}
